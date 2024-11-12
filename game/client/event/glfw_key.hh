// SPDX-License-Identifier: BSD-2-Clause
#pragma once
#include <GLFW/glfw3.h>

struct GlfwKeyEvent final {
    int key {GLFW_KEY_UNKNOWN};
    int scancode {};
    int action {};
    int mods {};
};
