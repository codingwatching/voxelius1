// SPDX-License-Identifier: BSD-2-Clause
#pragma once
#include <GLFW/glfw3.h>

struct GlfwMouseButtonEvent final {
    int button {GLFW_KEY_UNKNOWN};
    int action {};
    int mods {};
};
