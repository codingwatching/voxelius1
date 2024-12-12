if(BUILD_CLIENT)
    add_executable(vclient
        "${CMAKE_CURRENT_LIST_DIR}/launch.cc"
        "${CMAKE_CURRENT_LIST_DIR}/precompiled.hh")
    target_compile_definitions(vclient PRIVATE LAUNCH_CLIENT)
    target_include_directories(vclient PRIVATE "${PROJECT_SOURCE_DIR}/source")
    target_precompile_headers(vclient PRIVATE "${CMAKE_CURRENT_LIST_DIR}/precompiled.hh")
    target_link_libraries(vclient PRIVATE client shared)

    if(WIN32)
        enable_language(RC)
        target_sources(vclient PRIVATE "${CMAKE_CURRENT_LIST_DIR}/vclient.rc")
    endif()
endif()

if(BUILD_SERVER)
    add_executable(vserver
        "${CMAKE_CURRENT_LIST_DIR}/launch.cc"
        "${CMAKE_CURRENT_LIST_DIR}/precompiled.hh")
    target_compile_definitions(vserver PRIVATE LAUNCH_SERVER)
    target_include_directories(vserver PRIVATE "${PROJECT_SOURCE_DIR}/source")
    target_precompile_headers(vserver PRIVATE "${CMAKE_CURRENT_LIST_DIR}/precompiled.hh")
    target_link_libraries(vserver PRIVATE server shared)

    if(WIN32)
        enable_language(RC)
        target_sources(vserver PRIVATE "${CMAKE_CURRENT_LIST_DIR}/vserver.rc")
    endif()
endif()
