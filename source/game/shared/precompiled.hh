// SPDX-License-Identifier: BSD-2-Clause
#pragma once

#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdint>

#include <algorithm>
#include <array>
#include <chrono>
#include <filesystem>
#include <functional>
#include <random>
#include <sstream>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

// FIXME: including hash_set8.hpp is fucked up whenever
// hash_table8.hpp is included. It doesn't even compile
// possibly due some function re-definitions. Too bad!
#include <emhash/hash_table8.hpp>

#include <enet/enet.h>

#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>

#include <FastNoiseLite.h>

#include <miniz.h>

#include <parson.h>

#include <physfs.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#if defined(_WIN32)
#include <spdlog/sinks/msvc_sink.h>
#endif

#if defined(__unix__)
#include <spdlog/sinks/syslog_sink.h>
#endif

#include <stb_image.h>
#include <stb_image_write.h>
