cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(voxelius LANGUAGES CXX VERSION 2024.12.001)

set(BUILD_CLIENT ON CACHE BOOL "Build Voxelius client executable")
set(BUILD_SERVER ON CACHE BOOL "Build Voxelius server executable")

set(ENABLE_EXPERIMENTS OFF CACHE BOOL "Enable generic experimental features")
set(ENABLE_SINGLEPLAYER OFF CACHE BOOL "Enable singleplayer support (experimental)")
set(ENABLE_VOX2_WORLDGEN OFF CACHE BOOL "Enable extended world generator (Vox2) implementation")
set(ENABLE_PROTOCOL_PARANOIA OFF CACHE BOOL "Enable additional network protocol sanity checks")

if(WIN32)
    # On Windows we cannot really guarantee the target system has
    # <LIBRARY> installed as a DLL, so we have to solve dependency hell
    # ourselves by pulling and compiling a static version of <LIBRARY>
    set(BUILD_USE_FETCHCONTENT ON CACHE BOOL "Use source distributions of certain dependencies")
else()
    # On almost all non-Windows systems it's pretty safe to assume the
    # target system will have the package installed mainly because it should
    # be all figure out by the package manager; see below for the CPack configuration
    set(BUILD_USE_FETCHCONTENT OFF CACHE BOOL "Use source distributions of certain dependencies")
endif()

# Ensure we're statically linking to all the dependencies that we
# pull and build by ourselves (see BUILD_USE_FETCHCONTENT); I cannot
# guarantee that the packaging scripts will work out of the box
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

# Ensure we're also statically linking with MSVC runtime libraries; this way
# the game's binaries can be distributed without needing VC++ distributables
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Output game binaries into a single directory for ease of access
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

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

    if(NOT BUILD_USE_FETCHCONTENT)
        find_package(glfw3 QUIET)
    endif()

    if(NOT glfw3_FOUND OR BUILD_USE_FETCHCONTENT)
        FetchContent_Declare(glfw3 GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4 GIT_SHALLOW TRUE GIT_PROGRESS TRUE)
        FetchContent_MakeAvailable(glfw3)
    endif()
endif()

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
add_subdirectory(source/launch)

include(package/config.cmake)
include(CPack)
