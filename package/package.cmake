set(CPACK_PACKAGE_NAME "Voxelius")
set(CPACK_PACKAGE_VENDOR "UNTOLABS")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Block game on steroids")
set(CPACK_PACKAGE_CONTACT "https://github.com/untolabs/voxelius/issues")

set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION_TWEAK "${PROJECT_VERSION_TWEAK}")

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
    set(CPACK_NSIS_MUI_HEADERIMAGE "${PROJECT_SOURCE_DIR}\\\\package\\\\nsis_header.bmp")
    set(CPACK_NSIS_MUI_WELCOMEFINISHPAGE_BITMAP "${PROJECT_SOURCE_DIR}\\\\package\\\\nsis_welcome.bmp")
    set(CPACK_NSIS_MUI_UNWELCOMEFINISHPAGE_BITMAP "${PROJECT_SOURCE_DIR}\\\\package\\\\nsis_welcome.bmp")

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
