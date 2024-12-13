add_executable(vclient
    "${CMAKE_CURRENT_LIST_DIR}/background.cc"
    "${CMAKE_CURRENT_LIST_DIR}/background.hh"
    "${CMAKE_CURRENT_LIST_DIR}/chat.cc"
    "${CMAKE_CURRENT_LIST_DIR}/chat.hh"
    "${CMAKE_CURRENT_LIST_DIR}/chunk_mesher.cc"
    "${CMAKE_CURRENT_LIST_DIR}/chunk_mesher.hh"
    "${CMAKE_CURRENT_LIST_DIR}/chunk_quad.hh"
    "${CMAKE_CURRENT_LIST_DIR}/chunk_renderer.cc"
    "${CMAKE_CURRENT_LIST_DIR}/chunk_renderer.hh"
    "${CMAKE_CURRENT_LIST_DIR}/chunk_vbo.hh"
    "${CMAKE_CURRENT_LIST_DIR}/chunk_visibility.cc"
    "${CMAKE_CURRENT_LIST_DIR}/chunk_visibility.hh"
    "${CMAKE_CURRENT_LIST_DIR}/const.hh"
    "${CMAKE_CURRENT_LIST_DIR}/crosshair.cc"
    "${CMAKE_CURRENT_LIST_DIR}/crosshair.hh"
    "${CMAKE_CURRENT_LIST_DIR}/experiments.cc"
    "${CMAKE_CURRENT_LIST_DIR}/experiments.hh"
    "${CMAKE_CURRENT_LIST_DIR}/entity/player_look.cc"
    "${CMAKE_CURRENT_LIST_DIR}/entity/player_look.hh"
    "${CMAKE_CURRENT_LIST_DIR}/entity/player_move.cc"
    "${CMAKE_CURRENT_LIST_DIR}/entity/player_move.hh"
    "${CMAKE_CURRENT_LIST_DIR}/event/glfw_cursor_pos.hh"
    "${CMAKE_CURRENT_LIST_DIR}/event/glfw_framebuffer_size.hh"
    "${CMAKE_CURRENT_LIST_DIR}/event/glfw_key.hh"
    "${CMAKE_CURRENT_LIST_DIR}/event/glfw_mouse_button.hh"
    "${CMAKE_CURRENT_LIST_DIR}/event/glfw_scroll.hh"
    "${CMAKE_CURRENT_LIST_DIR}/event/language_set.hh"
    "${CMAKE_CURRENT_LIST_DIR}/game.cc"
    "${CMAKE_CURRENT_LIST_DIR}/game.hh"
    "${CMAKE_CURRENT_LIST_DIR}/globals.cc"
    "${CMAKE_CURRENT_LIST_DIR}/globals.hh"
    "${CMAKE_CURRENT_LIST_DIR}/gui_screen.hh"
    "${CMAKE_CURRENT_LIST_DIR}/keyboard.cc"
    "${CMAKE_CURRENT_LIST_DIR}/keyboard.hh"
    "${CMAKE_CURRENT_LIST_DIR}/keynames.cc"
    "${CMAKE_CURRENT_LIST_DIR}/keynames.hh"
    "${CMAKE_CURRENT_LIST_DIR}/language.cc"
    "${CMAKE_CURRENT_LIST_DIR}/language.hh"
    "${CMAKE_CURRENT_LIST_DIR}/main.cc"
    "${CMAKE_CURRENT_LIST_DIR}/main_menu.cc"
    "${CMAKE_CURRENT_LIST_DIR}/main_menu.hh"
    "${CMAKE_CURRENT_LIST_DIR}/message_box.cc"
    "${CMAKE_CURRENT_LIST_DIR}/message_box.hh"
    "${CMAKE_CURRENT_LIST_DIR}/metrics.cc"
    "${CMAKE_CURRENT_LIST_DIR}/metrics.hh"
    "${CMAKE_CURRENT_LIST_DIR}/mouse.cc"
    "${CMAKE_CURRENT_LIST_DIR}/mouse.hh"
    "${CMAKE_CURRENT_LIST_DIR}/outline.cc"
    "${CMAKE_CURRENT_LIST_DIR}/outline.hh"
    "${CMAKE_CURRENT_LIST_DIR}/player_list.cc"
    "${CMAKE_CURRENT_LIST_DIR}/player_list.hh"
    "${CMAKE_CURRENT_LIST_DIR}/player_target.cc"
    "${CMAKE_CURRENT_LIST_DIR}/player_target.hh"
    "${CMAKE_CURRENT_LIST_DIR}/precompiled.hh"
    "${CMAKE_CURRENT_LIST_DIR}/play_menu.cc"
    "${CMAKE_CURRENT_LIST_DIR}/play_menu.hh"
    "${CMAKE_CURRENT_LIST_DIR}/progress.cc"
    "${CMAKE_CURRENT_LIST_DIR}/progress.hh"
    "${CMAKE_CURRENT_LIST_DIR}/receive.cc"
    "${CMAKE_CURRENT_LIST_DIR}/receive.hh"
    "${CMAKE_CURRENT_LIST_DIR}/screenshot.cc"
    "${CMAKE_CURRENT_LIST_DIR}/screenshot.hh"
    "${CMAKE_CURRENT_LIST_DIR}/session.cc"
    "${CMAKE_CURRENT_LIST_DIR}/session.hh"
    "${CMAKE_CURRENT_LIST_DIR}/settings.cc"
    "${CMAKE_CURRENT_LIST_DIR}/settings.hh"
    "${CMAKE_CURRENT_LIST_DIR}/skybox.cc"
    "${CMAKE_CURRENT_LIST_DIR}/skybox.hh"
    "${CMAKE_CURRENT_LIST_DIR}/splash.cc"
    "${CMAKE_CURRENT_LIST_DIR}/splash.hh"
    "${CMAKE_CURRENT_LIST_DIR}/toggles.cc"
    "${CMAKE_CURRENT_LIST_DIR}/toggles.hh"
    "${CMAKE_CURRENT_LIST_DIR}/varied_program.cc"
    "${CMAKE_CURRENT_LIST_DIR}/varied_program.hh"
    "${CMAKE_CURRENT_LIST_DIR}/view.cc"
    "${CMAKE_CURRENT_LIST_DIR}/view.hh"
    "${CMAKE_CURRENT_LIST_DIR}/voxel_anims.cc"
    "${CMAKE_CURRENT_LIST_DIR}/voxel_anims.hh"
    "${CMAKE_CURRENT_LIST_DIR}/voxel_atlas.cc"
    "${CMAKE_CURRENT_LIST_DIR}/voxel_atlas.hh")
target_compile_definitions(vclient PUBLIC GLFW_INCLUDE_NONE)
target_include_directories(vclient PRIVATE "${PROJECT_SOURCE_DIR}/source")
target_include_directories(vclient PRIVATE "${PROJECT_SOURCE_DIR}/source/game")
target_precompile_headers(vclient PRIVATE "${CMAKE_CURRENT_LIST_DIR}/precompiled.hh")
target_link_libraries(vclient PUBLIC glad glfw imgui imgui_glfw imgui_opengl3)
target_link_libraries(vclient PUBLIC shared)

if(WIN32 AND MSVC)
    # GLFW defines APIENTRY and ENet includes
    # Windows API headers which also define APIENTRY
    target_compile_options(vclient PRIVATE /wd4005)
endif()

if(WIN32)
    enable_language(RC)
    target_sources(vclient PRIVATE "${PROJECT_SOURCE_DIR}/resource/vclient.rc")
endif()
