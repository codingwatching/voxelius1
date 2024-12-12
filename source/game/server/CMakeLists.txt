add_library(server STATIC
    "${CMAKE_CURRENT_LIST_DIR}/chat.cc"
    "${CMAKE_CURRENT_LIST_DIR}/chat.hh"
    "${CMAKE_CURRENT_LIST_DIR}/game.cc"
    "${CMAKE_CURRENT_LIST_DIR}/game.hh"
    "${CMAKE_CURRENT_LIST_DIR}/globals.cc"
    "${CMAKE_CURRENT_LIST_DIR}/globals.hh"
    "${CMAKE_CURRENT_LIST_DIR}/main.cc"
    "${CMAKE_CURRENT_LIST_DIR}/main.hh"
    "${CMAKE_CURRENT_LIST_DIR}/precompiled.hh"
    "${CMAKE_CURRENT_LIST_DIR}/receive.cc"
    "${CMAKE_CURRENT_LIST_DIR}/receive.hh"
    "${CMAKE_CURRENT_LIST_DIR}/sessions.cc"
    "${CMAKE_CURRENT_LIST_DIR}/sessions.hh"
    "${CMAKE_CURRENT_LIST_DIR}/status.cc"
    "${CMAKE_CURRENT_LIST_DIR}/status.hh")
target_include_directories(server PRIVATE "${PROJECT_SOURCE_DIR}/source")
target_include_directories(server PRIVATE "${PROJECT_SOURCE_DIR}/source/game")
target_precompile_headers(server PRIVATE "${CMAKE_CURRENT_LIST_DIR}/precompiled.hh")
target_link_libraries(server PUBLIC shared)
