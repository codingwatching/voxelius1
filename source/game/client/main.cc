// SPDX-License-Identifier: BSD-2-Clause
#include "client/precompiled.hh"

#include "config/cmake.hh"

#include "mathlib/constexpr.hh"

#include "common/cmdline.hh"
#include "common/config.hh"
#include "common/epoch.hh"
#include "common/image.hh"
#include "common/strtools.hh"

#include "shared/motd.hh"
#include "shared/setup.hh"

#include "client/event/glfw_cursor_pos.hh"
#include "client/event/glfw_framebuffer_size.hh"
#include "client/event/glfw_key.hh"
#include "client/event/glfw_mouse_button.hh"
#include "client/event/glfw_scroll.hh"

#include "client/const.hh"
#include "client/game.hh"
#include "client/globals.hh"


#if defined(_WIN32)
extern "C" __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
extern "C" __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
#endif

static void on_glfw_error(int code, const char *message)
{
    spdlog::error("glfw: {}", message);
}

static void on_glfw_char(GLFWwindow *window, unsigned int codepoint)
{
    ImGui_ImplGlfw_CharCallback(window, codepoint);
}

static void on_glfw_cursor_enter(GLFWwindow *window, int entered)
{
    ImGui_ImplGlfw_CursorEnterCallback(window, entered);
}

static void on_glfw_cursor_pos(GLFWwindow *window, double xpos, double ypos)
{
    GlfwCursorPosEvent event = {};
    event.xpos = static_cast<float>(xpos);
    event.ypos = static_cast<float>(ypos);
    globals::dispatcher.trigger(event);

    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
}

static void on_glfw_framebuffer_size(GLFWwindow *window, int width, int height)
{
    if(glfwGetWindowAttrib(window, GLFW_ICONIFIED)) {
        // Don't do anything if the window was just
        // iconified (minimized); as it turns out minimized
        // windows on WIN32 seem to be forced into 0x0
        return;
    }
    
    globals::width = width;
    globals::height = height;
    globals::aspect = static_cast<float>(width) / static_cast<float>(height);

    GlfwFramebufferSizeEvent fb_event = {};
    fb_event.width = globals::width;
    fb_event.height = globals::height;
    fb_event.aspect = globals::aspect;
    globals::dispatcher.trigger(fb_event);
}

static void on_glfw_key(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    GlfwKeyEvent event = {};
    event.key = key;
    event.scancode = scancode;
    event.action = action;
    event.mods = mods;
    globals::dispatcher.trigger(event);

    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}

static void on_glfw_monitor_event(GLFWmonitor *monitor, int event)
{
    ImGui_ImplGlfw_MonitorCallback(monitor, event);
}

static void on_glfw_mouse_button(GLFWwindow *window, int button, int action, int mods)
{
    GlfwMouseButtonEvent event = {};
    event.button = button;
    event.action = action;
    event.mods = mods;
    globals::dispatcher.trigger(event);

    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}

static void on_glfw_scroll(GLFWwindow *window, double dx, double dy)
{
    GlfwScrollEvent event = {};
    event.dx = static_cast<float>(dx);
    event.dy = static_cast<float>(dy);
    globals::dispatcher.trigger(event);

    ImGui_ImplGlfw_ScrollCallback(window, dx, dy);
}

static void on_glfw_window_focus(GLFWwindow *window, int focused)
{
    ImGui_ImplGlfw_WindowFocusCallback(window, focused);
}

static void on_opengl_message(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *param)
{
    spdlog::info("opengl: {}", reinterpret_cast<const char *>(message));
}

int main(int argc, char **argv)
{
    cmdline::append(argc, argv);

    shared::setup(argc, argv);

#if defined(_WIN32)
#if defined(NDEBUG)
    if(GetConsoleWindow() && !cmdline::contains("preserve-winconsole")) {
        // Hide the console window on release builds
        // unless explicitly specified to preserve it instead
        FreeConsole();
    }
#else
    if(GetConsoleWindow() && cmdline::contains("hide-winconsole")) {
        // Do NOT hide the console window on debug builds
        // unless explicitly specified to hide it instead
        FreeConsole();
    }
#endif
#endif

    spdlog::info("client: game version: {}", PROJECT_VERSION_STRING);

    glfwSetErrorCallback(&on_glfw_error);

    if(!glfwInit()) {
        spdlog::critical("glfw: init failed");
        std::terminate();
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_SAMPLES, 0);

    globals::window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "Client", nullptr, nullptr);

    if(!globals::window) {
        spdlog::critical("glfw: failed to open a window");
        std::terminate();
    }

    // The UI is scaled against a resolution defined by BASE_WIDTH and BASE_HEIGHT
    // constants. However, UI scale of 1 doesn't look that good, so the window size is
    // limited to a resolution that allows at least UI scale of 2 and is defined by MIN_WIDTH and MIN_HEIGHT.
    glfwSetWindowSizeLimits(globals::window, MIN_WIDTH, MIN_HEIGHT, GLFW_DONT_CARE, GLFW_DONT_CARE);

    glfwSetCharCallback(globals::window, &on_glfw_char);
    glfwSetCursorEnterCallback(globals::window, &on_glfw_cursor_enter);
    glfwSetCursorPosCallback(globals::window, &on_glfw_cursor_pos);
    glfwSetFramebufferSizeCallback(globals::window, &on_glfw_framebuffer_size);
    glfwSetKeyCallback(globals::window, &on_glfw_key);
    glfwSetMouseButtonCallback(globals::window, &on_glfw_mouse_button);
    glfwSetScrollCallback(globals::window, &on_glfw_scroll);
    glfwSetWindowFocusCallback(globals::window, &on_glfw_window_focus);

    glfwSetMonitorCallback(&on_glfw_monitor_event);

    Image image = {};
    GLFWimage icon = {};

    if(Image::load_rgba(image, "textures/gui/window_icon.png", false)) {
        icon.width = image.width;
        icon.height = image.height;
        icon.pixels = reinterpret_cast<unsigned char *>(image.pixels);

        glfwSetWindowIcon(globals::window, 1, &icon);

        Image::unload(image);
    }

    glfwMakeContextCurrent(globals::window);
    glfwSwapInterval(1);

    if(!gladLoadGL(&glfwGetProcAddress)) {
        spdlog::critical("glad: failed to load function pointers");
        std::terminate();
    }

    if(GLAD_GL_KHR_debug) {
        if(!cmdline::contains("no-gl-debug")) {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(&on_opengl_message, nullptr);

            // NVIDIA drivers print additional buffer information
            // to the debug output that programmers might find useful.
            static const uint32_t ignore_nvidia_131185 = 131185;
            glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, 1, &ignore_nvidia_131185, GL_FALSE);
        }
        else {
            spdlog::warn("glad: no-gl-debug command line parameter found");
            spdlog::warn("glad: OpenGL errors will not be logged");
        }
    }
    else {
        spdlog::warn("glad: KHR_debug extension not supported");
        spdlog::warn("glad: OpenGL errors will not be logged");
    }

    spdlog::info("opengl: version: {}", reinterpret_cast<const char *>(glGetString(GL_VERSION)));
    spdlog::info("opengl: renderer: {}", reinterpret_cast<const char *>(glGetString(GL_RENDERER)));

    motd::init("motds/client.txt");

    const std::string title = fmt::format("Voxelius {}: {}", PROJECT_VERSION_STRING, motd::get());
    glfwSetWindowTitle(globals::window, title.c_str());

    glDisable(GL_MULTISAMPLE);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(globals::window, false);
    ImGui_ImplOpenGL3_Init(nullptr);

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    globals::frametime = 0.0f;
    globals::frametime_avg = 0.0f;
    globals::frametime_us = 0;
    globals::curtime = epoch::microseconds();
    globals::framecount = 0;

    std::string mode_str;

    if(cmdline::contains("fullscreen")) {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(globals::window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
    else if(cmdline::get_value("mode", mode_str)) {
        int mode_width = DEFAULT_WIDTH;
        int mode_height = DEFAULT_HEIGHT;

        std::sscanf(mode_str.c_str(), "%dx%d", &mode_width, &mode_height);

        mode_width = cxpr::max(mode_width, MIN_WIDTH);
        mode_height = cxpr::max(mode_height, MIN_HEIGHT);

        glfwSetWindowSize(globals::window, mode_width, mode_height);
    }

    client_game::init();

    int wwidth, wheight;
    glfwGetFramebufferSize(globals::window, &wwidth, &wheight);
    on_glfw_framebuffer_size(globals::window, wwidth, wheight);

    Config::load(globals::client_config, "client.conf");

    client_game::init_late();

    std::uint64_t last_curtime = globals::curtime;

    while(!glfwWindowShouldClose(globals::window)) {
        globals::curtime = epoch::microseconds();
        globals::frametime_us = globals::curtime - last_curtime;
        globals::frametime = static_cast<float>(globals::frametime_us) / 1000000.0f;
        globals::frametime_avg += globals::frametime;
        globals::frametime_avg *= 0.5f;

        globals::num_drawcalls = 0;
        globals::num_triangles = 0;

        last_curtime = globals::curtime;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        client_game::update();

        if(!glfwGetWindowAttrib(globals::window, GLFW_ICONIFIED)) {
            glDisable(GL_BLEND);

            glDisable(GL_DEPTH_TEST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, globals::width, globals::height);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // Make sure there is no stray program object
            // being bound to the context. Usually third-party
            // overlay software (such as RivaTuner) injects itself
            // into the rendering loop and binds internal objects,
            // which creates an incomprehensible visual mess
            glUseProgram(0);

            client_game::render();

            glDisable(GL_DEPTH_TEST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, globals::width, globals::height);

            // All the 2D rendering goes through ImGui, and it being
            // an immediate-mode solution makes it hard to separate
            // rendering and UI logic updates, so this here function
            // acts as the definitive UI rendering/logic callback
            client_game::layout();
        }

        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(globals::window);

        client_game::update_late();

        glfwPollEvents();

        // EnTT provides two ways of dispatching events:
        // queued and immediate. When glfwPollEvents() is
        // called, immediate events are triggered across
        // the application, whilst queued ones are triggered
        // later by calling entt::dispatcher::update()
        globals::dispatcher.update();

        globals::framecount += 1;
    }

    spdlog::info("client: shutdown after {} frames", globals::framecount);
    spdlog::info("client: average framerate: {:.03f} FPS", 1.0f / globals::frametime_avg);
    spdlog::info("client: average frametime: {:.03f} ms", 1000.0f * globals::frametime_avg);

    client_game::deinit();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(globals::window);
    glfwTerminate();

    Config::save(globals::client_config, "client.conf");

    shared::desetup();

    return 0;
}