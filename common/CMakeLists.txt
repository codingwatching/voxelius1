add_library(common STATIC
    "${CMAKE_CURRENT_LIST_DIR}/cmdline.cc"
    "${CMAKE_CURRENT_LIST_DIR}/config.cc"
    "${CMAKE_CURRENT_LIST_DIR}/crc64.cc"
    "${CMAKE_CURRENT_LIST_DIR}/epoch.cc"
    "${CMAKE_CURRENT_LIST_DIR}/fstools.cc"
    "${CMAKE_CURRENT_LIST_DIR}/image.cc"
    "${CMAKE_CURRENT_LIST_DIR}/packet_buffer.cc"
    "${CMAKE_CURRENT_LIST_DIR}/strtools.cc")
target_include_directories(common PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(common PUBLIC mathlib physfs spdlog stb)

if(WIN32)
    target_compile_definitions(common PUBLIC _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(common PUBLIC NOMINMAX)
endif()

if(MSVC)
    target_compile_options(common PUBLIC /wd4018)
    target_compile_options(common PUBLIC /wd4244)
    target_compile_options(common PUBLIC /wd4267)
endif()
