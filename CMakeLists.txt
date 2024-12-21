cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(voxelius LANGUAGES CXX VERSION 0.0.1.2452)

set(CPACK_PACKAGE_NAME "voxelius")
set(CPACK_PACKAGE_VENDOR "untolabs")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Block game on steroids")
set(CPACK_PACKAGE_CONTACT "https://github.com/untolabs/voxelius/issues")

set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION_TWEAK ${PROJECT_VERSION_TWEAK})

set(CPACK_PACKAGE_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")

# This enables solution directories if the feature is applicable
# to the target build environment. This allows built-in pseudo-targets
# like ALL_BUILD and ZERO_CHECK to be moved out of sight into a folder
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(BUILD_CLIENT ON CACHE BOOL "Build Voxelius client executable")
set(BUILD_SERVER ON CACHE BOOL "Build Voxelius server executable")

set(ENABLE_EXPERIMENTS OFF CACHE BOOL "Enable basic experimental features")

# Ensure we're statically linking to all the dependencies that we
# pull and build by ourselves; I cannot guarantee that the packaging
# scripts for non-Windows systems will simply work out of the box
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

# Ensure we're also statically linking with MSVC runtime libraries; this way
# the game's binaries can be distributed without needing VC++ distributables
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Output game binaries into a single directory for ease of access
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

# Require ISO C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# When we're building GLFW manually, by default
# it will also generate documentation, test binaries
# and maybe (just maybe) compile examples and mess with
# the install commands in our own script; so we just disable everything
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

# Enable ZIP archive support for PhysFS
set(PHYSFS_ARCHIVE_ZIP ON CACHE BOOL "" FORCE)

include(FetchContent)

if(BUILD_CLIENT)
    add_subdirectory(external/glad)
    add_subdirectory(external/imgui)

    FetchContent_Declare(glfw3 GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4 GIT_SHALLOW TRUE GIT_PROGRESS TRUE)
    FetchContent_MakeAvailable(glfw3)
endif()

add_subdirectory(assets)

add_subdirectory(external/emhash)
add_subdirectory(external/enet)
add_subdirectory(external/entt)
add_subdirectory(external/fastnoiselite)
add_subdirectory(external/miniz)
add_subdirectory(external/parson)
add_subdirectory(external/physfs)
add_subdirectory(external/spdlog)
add_subdirectory(external/stb)
add_subdirectory(external/thread_pool)

add_subdirectory(source/config)
add_subdirectory(source/common)
add_subdirectory(source/mathlib)

add_subdirectory(source/game/client)
add_subdirectory(source/game/server)
add_subdirectory(source/game/shared)

include(CPack)
