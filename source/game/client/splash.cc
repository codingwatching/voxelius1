// SPDX-License-Identifier: BSD-2-Clause
#include "client/precompiled.hh"
#include "client/splash.hh"

#include "mathlib/constexpr.hh"

#include "common/epoch.hh"
#include "common/cmdline.hh"
#include "common/image.hh"

#include "client/event/glfw_key.hh"
#include "client/event/glfw_mouse_button.hh"
#include "client/event/glfw_scroll.hh"

#include "client/globals.hh"
#include "client/language.hh"


constexpr static ImGuiWindowFlags WINDOW_FLAGS = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration;

constexpr static std::size_t DELAY_MICROSECONDS = 2000000;
constexpr static const char *TEXTURE_PATH = "textures/gui/untolabs.png";

static GLuint texture;
static float texture_aspect;
static float texture_alpha;
static std::uint64_t end_time;

static void on_glfw_key(const GlfwKeyEvent &event)
{
    end_time = UINT64_C(0);
}

static void on_glfw_mouse_button(const GlfwMouseButtonEvent &event)
{
    end_time = UINT64_C(0);
}

static void on_glfw_scroll(const GlfwScrollEvent &event)
{
    end_time = UINT64_C(0);
}

void splash::init(void)
{
    Image image = {};

    if(cmdline::contains("nosplash")) {
        texture = 0;
        texture_aspect = 0.0f;
        texture_alpha = 0.0f;
        return;
    }

    if(!image.load_rgba(image, TEXTURE_PATH, false)) {
        texture = 0;
        texture_aspect = 0.0f;
        texture_alpha = 0.0f;
        return;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    texture_aspect = static_cast<float>(image.width) / static_cast<float>(image.height);
    texture_alpha = 1.0f;

    Image::unload(image);
}

void splash::init_late(void)
{
    if(!texture) {
        // We don't have to waste time
        // rendering the missing splash texture
        return;
    }

    end_time = epoch::microseconds() + DELAY_MICROSECONDS;

    globals::dispatcher.sink<GlfwKeyEvent>().connect<&on_glfw_key>();
    globals::dispatcher.sink<GlfwMouseButtonEvent>().connect<&on_glfw_mouse_button>();
    globals::dispatcher.sink<GlfwScrollEvent>().connect<&on_glfw_scroll>();

    const std::string skip_status = language::resolve("splash.skip_status");

    while(!glfwWindowShouldClose(globals::window)) {
        const std::uint64_t curtime = epoch::microseconds();
        const std::uint64_t remains = end_time - curtime;

        if(curtime >= end_time)
            break;

        texture_alpha = cxpr::smoothstep(0.15f, 0.5f, static_cast<float>(remains) / static_cast<float>(DELAY_MICROSECONDS));
        splash::render(skip_status);
    }

    globals::dispatcher.sink<GlfwKeyEvent>().disconnect<&on_glfw_key>();
    globals::dispatcher.sink<GlfwMouseButtonEvent>().disconnect<&on_glfw_mouse_button>();
    globals::dispatcher.sink<GlfwScrollEvent>().disconnect<&on_glfw_scroll>();

    glDeleteTextures(1, &texture);

    texture = 0;
    texture_aspect = 0.0f;
    texture_alpha = 0.0f;
    end_time = UINT64_C(0);
}

void splash::render(const std::string &status)
{
    if(!texture) {
        // We don't have to waste time
        // rendering the missing splash texture
        return;
    }

    // The splash is rendered outside the main
    // render loop, so we have to manually begin
    // and render both window and ImGui frames
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    const ImVec2 window_start = ImVec2(0.0f, 0.0f);
    const ImVec2 window_size = ImVec2(viewport->Size.x, viewport->Size.y);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, viewport->Size.x, viewport->Size.y);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui::SetNextWindowPos(window_start);
    ImGui::SetNextWindowSize(window_size);

    if(ImGui::Begin("###splash", nullptr, WINDOW_FLAGS)) {
        const float image_width = 0.80f * viewport->Size.x;
        const float image_height = image_width / texture_aspect;
        const ImVec2 image_size = ImVec2(image_width, image_height);

        const float image_x = 0.5f * (viewport->Size.x - image_width);
        const float image_y = 0.5f * (viewport->Size.y - image_height);
        const ImVec2 image_pos = ImVec2(image_x, image_y);

        if(!status.empty()) {
            ImGui::PushFont(globals::font_chat);
            ImGui::SetCursorPos(ImVec2(16.0f, 16.0f));
            ImGui::TextDisabled("%s", status.c_str());
            ImGui::PopFont();
        }

        const ImVec2 uv_a = ImVec2(0.0f, 0.0f);
        const ImVec2 uv_b = ImVec2(1.0f, 1.0f);
        const ImVec4 tint = ImVec4(1.0f, 1.0f, 1.0f, texture_alpha);
        ImTextureID texture_id = reinterpret_cast<ImTextureID>(static_cast<std::uintptr_t>(texture));

        ImGui::SetCursorPos(image_pos);
        ImGui::Image(texture_id, image_size, uv_a, uv_b, tint);
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(globals::window);
    glfwPollEvents();
}
