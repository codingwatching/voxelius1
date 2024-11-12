cmake_minimum_required(VERSION 3.15)
project(VOXELIUS LANGUAGES CXX VERSION 0.0.1.2411)

set(BUILD_CLIENT ON CACHE BOOL "Build client executable")
set(BUILD_SERVER ON CACHE BOOL "Build server executable")

if(WIN32)
    # On Windows we are forced to solve the dependency hell ourselves
    # and the best solution to it is to just bundle everything statically
    # including windowing (GLFW) and C/C++ standard library implementations
    set(GLFW_USE_FETCHCONTENT ON CACHE BOOL "Force the build system to download and build GLFW")
else()
    # On UNIX, UNIX-likes and other non-Windows systems the dependency hell is
    # usually solved by the system package manager, plus this allows the user
    # to update the windowing (GLFW) package to fix, say a critical issue with it
    # without then waiting for a maintainer to push an update to the game package
    set(GLFW_USE_FETCHCONTENT OFF CACHE BOOL "Force the build system to download and build GLFW")
endif()

# Make sure that if we're building external dependencies
# that we download (GLFW_USE_FETCHCONTENT), we are building
# and linking to them statically (solving dependency hell)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# Make sure that we're statically linking with MSVC runtime libraries
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

set(PHYSFS_ARCHIVE_ZIP ON CACHE BOOL "" FORCE)

include(FetchContent)

if(BUILD_CLIENT)
    add_subdirectory(external/glad)
    add_subdirectory(external/imgui)

    if(NOT GLFW_USE_FETCHCONTENT)
        find_package(glfw3 QUIET)
    endif()

    if(NOT glfw3_FOUND OR GLFW_USE_FETCHCONTENT)
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

add_subdirectory(common)
add_subdirectory(config)
add_subdirectory(mathlib)

if(BUILD_CLIENT)
    add_subdirectory(game/client)
endif()

add_subdirectory(game/server)
add_subdirectory(game/shared)
add_subdirectory(launch)

set(CPACK_PACKAGE_NAME "Voxelius")
set(CPACK_PACKAGE_VENDOR "UNTOLABS")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Block game on steroids")
set(CPACK_PACKAGE_CONTACT "https://github.com/untolabs/voxelius/issues")

set(CPACK_PACKAGE_VERSION "${VOXELIUS_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${VOXELIUS_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VOXELIUS_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VOXELIUS_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION_TWEAK "${VOXELIUS_VERSION_TWEAK}")

set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")

if(WIN32)
    if(BUILD_CLIENT)
        # Place client-side game binary in the same
        # directory alongside assets and server binary
        install(TARGETS vclient RUNTIME DESTINATION ".")
    endif()

    if(BUILD_SERVER)
        # Place server-side game binary in the same
        # directory alongside assets and client binary
        install(TARGETS vserver RUNTIME DESTINATION ".")
    endif()

    # Place assets in the same directory alongside
    # client-side and server-side game binaries
    install(DIRECTORY assets/ DESTINATION ".")

    set(CPACK_GENERATOR NSIS)

    # Assign installer interface images
    set(CPACK_NSIS_MUI_HEADERIMAGE "${PROJECT_SOURCE_DIR}\\\\package\\\\header.bmp")
    set(CPACK_NSIS_MUI_WELCOMEFINISHPAGE_BITMAP "${PROJECT_SOURCE_DIR}\\\\package\\\\welcome.bmp")
    set(CPACK_NSIS_MUI_UNWELCOMEFINISHPAGE_BITMAP "${PROJECT_SOURCE_DIR}\\\\package\\\\welcome.bmp")

    # Assign start menu shortcuts
    list(APPEND CPACK_NSIS_MENU_LINKS "vserver.exe" "${CPACK_PACKAGE_NAME} Server")
    list(APPEND CPACK_NSIS_MENU_LINKS "vclient.exe" "${CPACK_PACKAGE_NAME}")
endif()

if(UNIX AND NOT APPLE)
    if(BUILD_CLIENT)
        # Place client-side game binary in an FHS-compliant
        # binary directory alongside the server binary
        install(TARGETS vclient RUNTIME DESTINATION bin)
    endif()

    if(BUILD_SERVER)
        # Place server-side game binary in an FHS-compliant
        # binary directory alongside the client binary
        install(TARGETS vserver RUNTIME DESTINATION bin)
    endif()

    # Place assets in an FHS-compliant directory for application
    # specific data that is not meant to be edited by regular userss
    install(DIRECTORY assets/ DESTINATION share/voxelius)

    if(glfw3_FOUND AND NOT GLFW_FORCE_FETCHCONTENT)
        # We're building using system GLFW distribution
        # so we must include it as a dependency; this is really
        # the only external library we might link to dynamically
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "libglfw3 (>= 3.3.8)")
    endif()

    set(CPACK_GENERATOR DEB)
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_CONTACT}")
endif()

include(CPack)
