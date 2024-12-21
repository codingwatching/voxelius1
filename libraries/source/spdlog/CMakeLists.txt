cmake_minimum_required(VERSION 3.15)
project(spdlog VERSION 1.14.1 LANGUAGES CXX)
add_library(spdlog STATIC
    "${CMAKE_CURRENT_LIST_DIR}/async.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/bundled_fmtlib_format.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/cfg.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/color_sinks.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/file_sinks.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/spdlog.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/stdout_sinks.cpp")
target_compile_definitions(spdlog PUBLIC SPDLOG_COMPILED_LIB)
target_include_directories(spdlog PUBLIC "${LIBRARIES_INCLUDE_DIR}")
set_target_properties(spdlog PROPERTIES FOLDER External)

find_package(Threads REQUIRED)
target_link_libraries(spdlog PUBLIC Threads::Threads)
