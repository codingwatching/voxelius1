// SPDX-License-Identifier: BSD-2-Clause
#include "client/precompiled.hh"
#include "client/game.hh"

#include "config/cmake.hh"

#include "common/config.hh"
#include "common/epoch.hh"
#include "common/fstools.hh"

#include "shared/game_voxels.hh"
#include "shared/protocol.hh"
#include "shared/ray_dda.hh"
#include "shared/world.hh"

#include "client/entity/player_look.hh"
#include "client/entity/player_move.hh"

#include "client/event/glfw_framebuffer_size.hh"

#include "client/background.hh"
#include "client/const.hh"
#include "client/chat.hh"
#include "client/chunk_mesher.hh"
#include "client/chunk_renderer.hh"
#include "client/chunk_visibility.hh"
#include "client/crosshair.hh"
#include "client/globals.hh"
#include "client/gui_screen.hh"
#include "client/keyboard.hh"
#include "client/keynames.hh"
#include "client/language.hh"
#include "client/main_menu.hh"
#include "client/message_box.hh"
#include "client/metrics.hh"
#include "client/mouse.hh"
#include "client/outline.hh"
#include "client/play_menu.hh"
#include "client/player_list.hh"
#include "client/player_target.hh"
#include "client/progress.hh"
#include "client/receive.hh"
#include "client/screenshot.hh"
#include "client/session.hh"
#include "client/settings.hh"
#include "client/skybox.hh"
#include "client/splash.hh"
#include "client/toggles.hh"
#include "client/view.hh"
#include "client/voxel_anims.hh"
#include "client/voxel_atlas.hh"

#if ENABLE_EXPERIMENTS
#include "client/experiments.hh"
#endif /* ENABLE_EXPERIMENTS */

// Debug
#include "shared/entity/head.hh"
#include "shared/entity/player.hh"
#include "shared/entity/transform.hh"
#include "shared/entity/velocity.hh"


bool client_game::streamer_mode = false;
bool client_game::vertical_sync = true;
bool client_game::world_curvature = true;
unsigned int client_game::pixel_size = 4U;
unsigned int client_game::fog_mode = 1U;

std::string client_game::username = "player";
std::uint64_t client_game::player_uid = UINT64_MAX;

static void on_glfw_framebuffer_size(const GlfwFramebufferSizeEvent &event)
{
    if(globals::world_fbo) {
        glDeleteRenderbuffers(1, &globals::world_fbo_depth);
        glDeleteTextures(1, &globals::world_fbo_color);
        glDeleteFramebuffers(1, &globals::world_fbo);
    }

    glGenFramebuffers(1, &globals::world_fbo);
    glGenTextures(1, &globals::world_fbo_color);
    glGenRenderbuffers(1, &globals::world_fbo_depth);

    glBindTexture(GL_TEXTURE_2D, globals::world_fbo_color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, event.width, event.height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

    glBindRenderbuffer(GL_RENDERBUFFER, globals::world_fbo_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, event.width, event.height);

    glBindFramebuffer(GL_FRAMEBUFFER, globals::world_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, globals::world_fbo_color, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, globals::world_fbo_depth);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        spdlog::critical("opengl: world framebuffer is incomplete");
        glDeleteRenderbuffers(1, &globals::world_fbo_depth);
        glDeleteTextures(1, &globals::world_fbo_color);
        glDeleteFramebuffers(1, &globals::world_fbo);
        std::terminate();
    }

    const float width_float = event.width;
    const float height_float = event.height;
    const unsigned int wscale = cxpr::max(1U, cxpr::floor<unsigned int>(width_float / static_cast<float>(BASE_WIDTH)));
    const unsigned int hscale = cxpr::max(1U, cxpr::floor<unsigned int>(height_float / static_cast<float>(BASE_HEIGHT)));
    const unsigned int scale = cxpr::min(wscale, hscale);

    if(globals::gui_scale != scale) {
        ImGuiIO &io = ImGui::GetIO();
        ImGuiStyle &style = ImGui::GetStyle();

        ImFontConfig font_config = {};
        font_config.FontDataOwnedByAtlas = false;

        io.Fonts->Clear();

        ImFontGlyphRangesBuilder builder = {};
        std::vector<uint8_t> fontbin = {};

        // This should cover a hefty range of glyph ranges.
        // UNDONE: just slap the whole UNICODE Plane-0 here?
        builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
        builder.AddRanges(io.Fonts->GetGlyphRangesCyrillic());
        builder.AddRanges(io.Fonts->GetGlyphRangesGreek());
        builder.AddRanges(io.Fonts->GetGlyphRangesJapanese());

        ImVector<ImWchar> ranges = {};
        builder.BuildRanges(&ranges);

        if(!fstools::read_bytes("fonts/unscii-16.ttf", fontbin))
            std::terminate();
        ImGui::GetIO().FontDefault = io.Fonts->AddFontFromMemoryTTF(fontbin.data(), fontbin.size(), 16.0f * scale, &font_config, ranges.Data);
        globals::font_chat = io.Fonts->AddFontFromMemoryTTF(fontbin.data(), fontbin.size(), 8.0f * scale, &font_config, ranges.Data);

        if(!fstools::read_bytes("fonts/unscii-8.ttf", fontbin))
            std::terminate();
        globals::font_debug = io.Fonts->AddFontFromMemoryTTF(fontbin.data(), fontbin.size(), 4.0f * scale, &font_config);

        // This should be here!!! Just calling Build()
        // on the font atlas does not invalidate internal
        // device objects defined by the implementation!!!
        ImGui_ImplOpenGL3_CreateDeviceObjects();

        if(globals::gui_scale) {
            // Well, ImGuiStyle::ScaleAllSizes indeed takes
            // the scale values as a RELATIVE scaling, not as
            // absolute. So I have to make a special crutch
            style.ScaleAllSizes(static_cast<float>(scale) / static_cast<float>(globals::gui_scale));
        }

        globals::gui_scale = scale;
    }
}

void client_game::init(void)
{
    splash::init();
    splash::render(std::string());

    client_game::player_uid = epoch::microseconds();
    
    Config::add(globals::client_config, "game.streamer_mode", client_game::streamer_mode);
    Config::add(globals::client_config, "game.vertical_sync", client_game::vertical_sync);
    Config::add(globals::client_config, "game.world_curvature", client_game::world_curvature);
    Config::add(globals::client_config, "game.pixel_size", client_game::pixel_size);
    Config::add(globals::client_config, "game.fog_mode", client_game::fog_mode);

    Config::add(globals::client_config, "game.username", client_game::username);
    Config::add(globals::client_config, "game.player_uid", client_game::player_uid);

    settings::add_checkbox(1, settings::VIDEO_GUI, "game.streamer_mode", client_game::streamer_mode, true);
    settings::add_checkbox(5, settings::VIDEO, "game.vertical_sync", client_game::vertical_sync, false);
    settings::add_checkbox(4, settings::VIDEO, "game.world_curvature", client_game::world_curvature, true);
    settings::add_slider(1, settings::VIDEO, "game.pixel_size", client_game::pixel_size, 1U, 4U, true);
    settings::add_stepper(3, settings::VIDEO, "game.fog_mode", client_game::fog_mode, 3U, false);

    settings::add_input(1, settings::GENERAL, "game.username", client_game::username, true, false);

    globals::client_host = enet_host_create(nullptr, 1, 1, 0, 0);

    if(!globals::client_host) {
        spdlog::critical("game: unable to setup an ENet host");
        std::terminate();
    }

    language::init();

    session::init();

    player_move::init();
    player_target::init();

    keynames::init();
    keyboard::init();
    mouse::init();

    screenshot::init();

    view::init();

    voxel_anims::init();

    chunk_mesher::init();
    chunk_renderer::init();

    crosshair::init();

    skybox::init();

    outline::init();

    world::init();

    ImGuiStyle &style = ImGui::GetStyle();

    // Black buttons on a dark background
    // may be harder to read than the text on them
    style.FrameBorderSize = 1.0;
    style.TabBorderSize = 1.0;

    // Rounding on elements looks cool but I am
    // aiming for a more or less blocky and
    // visually simple HiDPI-friendly UI style
    style.TabRounding       = 0.0f;
    style.GrabRounding      = 0.0f;
    style.ChildRounding     = 0.0f;
    style.FrameRounding     = 0.0f;
    style.PopupRounding     = 0.0f;
    style.WindowRounding    = 0.0f;
    style.ScrollbarRounding = 0.0f;

    style.Colors[ImGuiCol_Text]                     = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]             = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_WindowBg]                 = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
    style.Colors[ImGuiCol_ChildBg]                  = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg]                  = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    style.Colors[ImGuiCol_Border]                   = ImVec4(0.79f, 0.79f, 0.79f, 0.50f);
    style.Colors[ImGuiCol_BorderShadow]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]                  = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
    style.Colors[ImGuiCol_FrameBgHovered]           = ImVec4(0.36f, 0.36f, 0.36f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive]            = ImVec4(0.63f, 0.63f, 0.63f, 0.67f);
    style.Colors[ImGuiCol_TitleBg]                  = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive]            = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]         = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    style.Colors[ImGuiCol_MenuBarBg]                = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg]              = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab]            = ImVec4(0.00f, 0.00f, 0.00f, 0.75f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]     = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]      = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_CheckMark]                = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]               = ImVec4(0.81f, 0.81f, 0.81f, 0.75f);
    style.Colors[ImGuiCol_SliderGrabActive]         = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_Button]                   = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered]            = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive]             = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_Header]                   = ImVec4(0.00f, 0.00f, 0.00f, 0.75f);
    style.Colors[ImGuiCol_HeaderHovered]            = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive]             = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_Separator]                = ImVec4(0.49f, 0.49f, 0.49f, 0.50f);
    style.Colors[ImGuiCol_SeparatorHovered]         = ImVec4(0.56f, 0.56f, 0.56f, 0.78f);
    style.Colors[ImGuiCol_SeparatorActive]          = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]               = ImVec4(0.34f, 0.34f, 0.34f, 0.20f);
    style.Colors[ImGuiCol_ResizeGripHovered]        = ImVec4(0.57f, 0.57f, 0.57f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive]         = ImVec4(1.00f, 1.00f, 1.00f, 0.95f);
    style.Colors[ImGuiCol_Tab]                      = ImVec4(0.00f, 0.00f, 0.00f, 0.75f);
    style.Colors[ImGuiCol_TabHovered]               = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    style.Colors[ImGuiCol_TabActive]                = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused]             = ImVec4(0.13f, 0.13f, 0.13f, 0.97f);
    style.Colors[ImGuiCol_TabUnfocusedActive]       = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
    style.Colors[ImGuiCol_PlotLines]                = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]         = ImVec4(0.69f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]            = ImVec4(0.00f, 1.00f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered]     = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TableHeaderBg]            = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_TableBorderStrong]        = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_TableBorderLight]         = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_TableRowBg]               = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_TableRowBgAlt]            = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    style.Colors[ImGuiCol_TextSelectedBg]           = ImVec4(0.61f, 0.61f, 0.61f, 0.35f);
    style.Colors[ImGuiCol_DragDropTarget]           = ImVec4(1.00f, 1.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_NavHighlight]             = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_NavWindowingHighlight]    = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    style.Colors[ImGuiCol_NavWindowingDimBg]        = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    style.Colors[ImGuiCol_ModalWindowDimBg]         = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

    // Making my own Game UI for Source Engine
    // taught me one important thing: dimensions
    // of UI elements must be calculated at semi-runtime
    // so there's simply no point for an INI file.
    ImGui::GetIO().IniFilename = nullptr;

    toggles::init();
    metrics::init();

    background::init();

    player_list::init();

    client_chat::init();

    main_menu::init();
    play_menu::init();
    settings::init();
    progress::init();
    message_box::init();

#if ENABLE_EXPERIMENTS
    experiments::init();
#endif /* ENABLE_EXPERIMENTS */

    globals::gui_keybind_ptr = nullptr;
    globals::gui_scale = 0U;
    globals::gui_screen = GUI_MAIN_MENU;

    globals::dispatcher.sink<GlfwFramebufferSizeEvent>().connect<&on_glfw_framebuffer_size>();
}

void client_game::init_late(void)
{
    language::init_late();

    settings::init_late();

    client_chat::init_late();

    game_voxels::populate();

#if ENABLE_EXPERIMENTS
    experiments::init_late();
#endif /* ENABLE_EXPERIMENTS */

    std::size_t max_texture_count = 0;

    // Figure out the total texture count
    // NOTE: this is very debug, early and a quite
    // conservative limit choice; there must be a better
    // way to make this limit way smaller than it currently is
    for(const VoxelInfo &info : vdef::voxels) {
        for(const VoxelTexture &vtex : info.textures) {
            max_texture_count += vtex.paths.size();
        }
    }

    // UNDONE: asset packs for non-16x16 stuff
    voxel_atlas::create(16, 16, max_texture_count);

    for(VoxelInfo &info : vdef::voxels) {
        for(VoxelTexture &vtex : info.textures) {
            if(AtlasStrip *strip = voxel_atlas::find_or_load(vtex.paths)) {
                vtex.cached_offset = strip->offset;
                vtex.cached_plane = strip->plane;
                continue;
            }
            
            spdlog::critical("debug_session: {}: failed to load atlas strips", info.name);
            std::terminate();
        }
    }

    voxel_atlas::generate_mipmaps();

    client_receive::init();

    splash::init_late();
}

void client_game::deinit(void)
{
    if(globals::session_peer) {
        session::disconnect("protocol.client_shutdown");
        while(enet_host_service(globals::client_host, nullptr, 50));
    }

#if ENABLE_EXPERIMENTS
    experiments::deinit();
#endif /* ENABLE_EXPERIMENTS */

    main_menu::deinit();

    play_menu::deinit();

    voxel_atlas::destroy();

    glDeleteRenderbuffers(1, &globals::world_fbo_depth);
    glDeleteTextures(1, &globals::world_fbo_color);
    glDeleteFramebuffers(1, &globals::world_fbo);

    background::deinit();

    outline::deinit();

    crosshair::deinit();

    chunk_renderer::deinit();
    chunk_mesher::deinit();

    globals::registry.clear();

    enet_host_destroy(globals::client_host);
}

void client_game::update(void)
{
#if ENABLE_EXPERIMENTS
    experiments::update();
#endif /* ENABLE_EXPERIMENTS */

    player_move::update();
    player_target::update();

    VelocityComponent::update();
    TransformComponent::update();

    view::update();

    voxel_anims::update();

    chunk_mesher::update();

    chunk_visibility::update();
    
    client_chat::update();
}

void client_game::update_late(void)
{
#if ENABLE_EXPERIMENTS
    experiments::update_late();
#endif /* ENABLE_EXPERIMENTS */

    mouse::update_late();

    if(client_game::vertical_sync)
        glfwSwapInterval(1);
    else glfwSwapInterval(0);

    ENetEvent event = {};

    while(enet_host_service(globals::client_host, &event, 0) > 0) {
        if(event.type == ENET_EVENT_TYPE_CONNECT) {
            session::send_login_request();
            continue;
        }

        if(event.type == ENET_EVENT_TYPE_DISCONNECT) {
            session::invalidate();
            continue;
        }

        if(event.type == ENET_EVENT_TYPE_RECEIVE) {
            protocol::receive(event.packet, event.peer);
            enet_packet_destroy(event.packet);
            continue;
        }
    }

    if(globals::session_peer && (globals::curtime >= globals::session_send_time)) {
        globals::session_send_time = globals::curtime + globals::session_tick_dt;

        if(globals::registry.valid(globals::player)) {
            protocol::send_entity_head(globals::session_peer, nullptr, globals::player);
            protocol::send_entity_transform(globals::session_peer, nullptr, globals::player);
            protocol::send_entity_velocity(globals::session_peer, nullptr, globals::player);
        }
    }

    play_menu::update_late();
}

void client_game::render(void)
{
    const int scaled_width = globals::width / cxpr::max(1U, client_game::pixel_size);
    const int scaled_height = globals::height / cxpr::max(1U, client_game::pixel_size);

    glViewport(0, 0, scaled_width, scaled_height);
    glBindFramebuffer(GL_FRAMEBUFFER, globals::world_fbo);
    glClearColor(skybox::fog_color[0], skybox::fog_color[1], skybox::fog_color[2], 1.000f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    chunk_renderer::render();

    player_target::render();

    const auto group = globals::registry.group(entt::get<PlayerComponent, HeadComponent, TransformComponent>);

    outline::prepare();

    for(const auto [entity, head, transform] : group.each()) {
        if(entity != globals::player) {
            WorldCoord wpos = transform.position;
            wpos.local += head.offset;

            WorldCoord wpos_cube = wpos;
            wpos_cube.local -= 0.5f;

            Vec3f forward = {};
            Vec3angles::vectors(head.angles + transform.angles, &forward, nullptr, nullptr);
            forward *= 2.0f;

            glEnable(GL_DEPTH_TEST);

            outline::cube(wpos_cube, Vec3f(1.0f));
            outline::line(wpos, forward);
        }
    }

    glViewport(0, 0, globals::width, globals::height);
    glClearColor(0.000f, 0.000f, 0.000f, 1.000f);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, globals::world_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, scaled_width, scaled_height, 0, 0, globals::width, globals::height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    if((globals::gui_screen == GUI_SCREEN_NONE) || (globals::gui_screen == GUI_CHAT)) {
        crosshair::render();
    }
}

void client_game::layout(void)
{
    if(!globals::registry.valid(globals::player)) {
        background::render();
    }

    if(!globals::gui_screen || (globals::gui_screen == GUI_CHAT) || (globals::gui_screen == GUI_DEBUG_WINDOW)) {
        if(toggles::draw_metrics) {
            // This contains Minecraft-esque debug information
            // about the hardware, world state and other
            // things that might be uesful
            metrics::layout();
        }
    }

    if(globals::registry.valid(globals::player)) {
        client_chat::layout();
        player_list::layout();
    }

    if(globals::gui_screen) {
        if(globals::registry.valid(globals::player) && (globals::gui_screen != GUI_CHAT) && (globals::gui_screen != GUI_DEBUG_WINDOW)) {
            const float width_f = static_cast<float>(globals::width);
            const float height_f = static_cast<float>(globals::height);
            const ImU32 splash = ImGui::GetColorU32(ImVec4(0.00f, 0.00f, 0.00f, 0.75f));
            ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(), ImVec2(width_f, height_f), splash);
        }

        switch(globals::gui_screen) {
            case GUI_MAIN_MENU:
                main_menu::layout();
                break;
            case GUI_PLAY_MENU:
                play_menu::layout();
                break;
            case GUI_SETTINGS:
                settings::layout();
                break;
            case GUI_PROGRESS:
                progress::layout();
                break;
            case GUI_MESSAGE_BOX:
                message_box::layout();
                break;
        }
    }
}