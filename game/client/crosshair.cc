// SPDX-License-Identifier: BSD-2-Clause
// Copyright (C) 2024, untodesu
#include <common/image.hh>
#include <game/client/event/glfw_framebuffer_size.hh>
#include <game/client/crosshair.hh>
#include <game/client/globals.hh>
#include <game/client/varied_program.hh>
#include <mathlib/mat4x4f.hh>
#include <mathlib/vec2f.hh>
#include <spdlog/spdlog.h>

static VariedProgram program = {};
static std::size_t u_texture = {};
static GLuint vaobj = {};
static GLuint vbo = {};

static int texture_width = {};
static int texture_height = {};
static GLuint texture = {};

void crosshair::init(void)
{
    if(!VariedProgram::setup(program, "shaders/crosshair.vert", "shaders/crosshair.frag")) {
        spdlog::critical("crosshair: program setup failed");
        std::terminate();
    }

    u_texture = VariedProgram::add_uniform(program, "u_Texture");

    const Vec2f vertices[4] = {
        Vec2f(0.0f, 1.0f),
        Vec2f(0.0f, 0.0f),
        Vec2f(1.0f, 1.0f),
        Vec2f(1.0f, 0.0f),
    };

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vaobj);
    glBindVertexArray(vaobj);

    glEnableVertexAttribArray(0);
    glVertexAttribDivisor(0, 0);
    glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(Vec2f), nullptr);

    Image image = {};
    if(!Image::load_rgba(image, "textures/gui/crosshair.png", true)) {
        std::terminate();
    }

    texture_width = image.width;
    texture_height = image.height;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture_width, texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    Image::unload(image);
}

void crosshair::deinit(void)
{
    glDeleteTextures(1, &texture);
    glDeleteVertexArrays(1, &vaobj);
    glDeleteBuffers(1, &vbo);
    VariedProgram::destroy(program);
}

void crosshair::render(void)
{
    if(!VariedProgram::update(program)) {
        spdlog::critical("crosshair: program update failed");
        std::terminate();
    }

    const int center_x = globals::width / 2;
    const int center_y = globals::height / 2;
    const int scaled_width = cxpr::max<int>(texture_width, globals::gui_scale * texture_width / 2);
    const int scaled_height = cxpr::max<int>(texture_height, globals::gui_scale * texture_height / 2);

    const int offset_x = center_x - scaled_width / 2;
    const int offset_y = center_y - scaled_height / 2;

    glDisable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(offset_x, offset_y, scaled_width, scaled_height);

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glUseProgram(program.handle);
    glUniform1i(program.uniforms[u_texture].location, 0); // GL_TEXTURE0

    glBindVertexArray(vaobj);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glViewport(0, 0, globals::width, globals::height);
}
