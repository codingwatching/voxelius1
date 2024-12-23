// SPDX-License-Identifier: BSD-2-Clause
#include "client/precompiled.hh"
#include "client/session.hh"

#include "common/config.hh"

#include "shared/entity/head.hh"
#include "shared/entity/player.hh"
#include "shared/entity/transform.hh"
#include "shared/entity/velocity.hh"

#include "shared/event/chunk_update.hh"
#include "shared/event/voxel_set.hh"

#include "shared/world/chunk_coord.hh"
#include "shared/world/local_coord.hh"
#include "shared/world/universe.hh"
#include "shared/world/unloader.hh"
#include "shared/world/vdef.hh"
#include "shared/world/voxel_coord.hh"
#include "shared/world/world.hh"

#include "shared/worldgen/worldgen.hh"

#include "shared/protocol.hh"

#include "client/gui/chat.hh"
#include "client/gui/gui_screen.hh"
#include "client/gui/message_box.hh"
#include "client/gui/progress.hh"

#include "client/world/chunk_visibility.hh"

#include "client/globals.hh"
#include "client/login.hh"
#include "client/view.hh"


static void on_login_response_packet(const protocol::LoginResponse &packet)
{
    spdlog::info("session: assigned session_id={}", packet.session_id);
    spdlog::info("session: assigned username: {}", packet.username);
    spdlog::info("session: server ticks at {} TPS", packet.tickrate);
    
    globals::session_id = packet.session_id;
    globals::session_tick_dt = static_cast<std::uint64_t>(1000000.0f / static_cast<float>(cxpr::max<std::uint16_t>(10, packet.tickrate)));
    globals::session_send_time = 0;
    globals::session_username = packet.username;
    
    progress::set_title("connecting.loading_world");
}

static void on_disconnect_packet(const protocol::Disconnect &packet)
{
    enet_peer_disconnect(globals::session_peer, 0);

    spdlog::info("session: disconnected: {}", packet.reason);

    client_chat::clear();

    globals::session_peer = nullptr;
    globals::session_id = UINT16_MAX;
    globals::session_tick_dt = UINT64_MAX;
    globals::session_send_time = UINT64_MAX;
    globals::session_username = std::string();

    globals::player = entt::null;
    globals::registry.clear();
    chunk_visibility::cleanup();

    message_box::reset();
    message_box::set_title("disconnected.disconnected");
    message_box::set_subtitle(packet.reason);
    message_box::add_button("disconnected.back", [](void) {
        globals::gui_screen = GUI_PLAY_MENU;
    });

    globals::gui_screen = GUI_MESSAGE_BOX;

    globals::is_singleplayer = true;
}

static void on_set_voxel_packet(const protocol::SetVoxel &packet)
{
    const auto cpos = VoxelCoord::to_chunk(packet.coord);
    const auto lpos = VoxelCoord::to_local(packet.coord);
    const auto index = LocalCoord::to_index(lpos);

    if(Chunk *chunk = world::find(cpos)) {
        if(chunk->voxels[index] != packet.voxel) {
            chunk->voxels[index] = packet.voxel;
            
            ChunkUpdateEvent event = {};
            event.coord = cpos;
            event.chunk = chunk;
            
            // Send a generic ChunkUpdate event to shake
            // up the mesher; directly calling world::set_voxel
            // here would result in a networked feedback loop
            // caused by event handler below tripping
            globals::dispatcher.trigger(event);
        }
    }
}

// NOTE: [session] is a good place for this since [receive]
// handles entity data sent by the server and [session] handles
// everything else network related that is not player movement
static void on_voxel_set(const VoxelSetEvent &event)
{
    if(globals::is_singleplayer) {
        // We're not sending anything to the
        // server because there is no server
        // to send things to in the first place
        return;
    }

    if(globals::session_peer) {
        // Propagate changes to the server
        // FIXME: should we also validate things here or wait for the server to do so
        protocol::send_set_voxel(globals::session_peer, nullptr, event.vpos, event.voxel);
    }
}

void session::init(void)
{
    globals::session_peer = nullptr;
    globals::session_id = UINT16_MAX;
    globals::session_tick_dt = UINT64_MAX;
    globals::session_send_time = UINT64_MAX;
    globals::session_username = std::string();
    
    globals::is_singleplayer = true;

    globals::dispatcher.sink<protocol::LoginResponse>().connect<&on_login_response_packet>();
    globals::dispatcher.sink<protocol::Disconnect>().connect<&on_disconnect_packet>();
    globals::dispatcher.sink<protocol::SetVoxel>().connect<&on_set_voxel_packet>();

    globals::dispatcher.sink<VoxelSetEvent>().connect<&on_voxel_set>();
}

void session::deinit(void)
{
    if(globals::is_singleplayer) {
        session::sp::unload_world();
        return;
    }

    session::mp::disconnect("protocol.client_shutdown");
    globals::session_send_time = UINT64_MAX;
}

void session::invalidate(void)
{
    if(globals::session_peer) {
        enet_peer_reset(globals::session_peer);
        
        message_box::reset();
        message_box::set_title("disconnected.disconnected");
        message_box::set_subtitle("enet.peer_connection_timeout");
        message_box::add_button("disconnected.back", [](void) {
            globals::gui_screen = GUI_PLAY_MENU;
        });

        globals::gui_screen = GUI_MESSAGE_BOX;
    }

    client_chat::clear();

    globals::session_peer = nullptr;
    globals::session_id = UINT16_MAX;
    globals::session_tick_dt = UINT64_MAX;
    globals::session_send_time = UINT64_MAX;
    globals::session_username = std::string();

    if(globals::registry.valid(globals::player))
        globals::player = entt::null;
    globals::registry.clear();
    chunk_visibility::cleanup();
}

void session::mp::connect(const std::string &host, std::uint16_t port)
{
    ENetAddress address = {};
    enet_address_set_host(&address, host.c_str());
    address.port = port;
    
    globals::session_peer = enet_host_connect(globals::client_host, &address, 1, 0);
    globals::session_id = UINT16_MAX;
    globals::session_tick_dt = UINT64_MAX;
    globals::session_send_time = UINT64_MAX;
    globals::session_username = std::string();

    globals::is_singleplayer = false;

    if(!globals::session_peer) {
        message_box::reset();
        message_box::set_title("disconnected.disconnected");
        message_box::set_subtitle("enet.peer_connection_failed");
        message_box::add_button("disconnected.back", [](void) {
            globals::gui_screen = GUI_PLAY_MENU;
        });

        globals::gui_screen = GUI_MESSAGE_BOX;

        return;
    }

    progress::reset();
    progress::set_title("connecting.connecting");
    progress::set_button("connecting.cancel_button", [](void) {
        enet_peer_disconnect(globals::session_peer, 0);

        globals::session_peer = nullptr;
        globals::session_id = UINT16_MAX;
        globals::session_tick_dt = UINT64_MAX;
        globals::session_send_time = UINT64_MAX;
        globals::session_username = std::string();

        globals::is_singleplayer = true;

        if(globals::registry.valid(globals::player))
            globals::player = entt::null;
        globals::registry.clear();
        chunk_visibility::cleanup();

        globals::gui_screen = GUI_PLAY_MENU;
    });

    globals::gui_screen = GUI_PROGRESS;
}

void session::mp::disconnect(const std::string &reason)
{
    if(globals::session_peer) {
        protocol::send_disconnect(globals::session_peer, nullptr, reason);

        enet_host_flush(globals::client_host);
        enet_host_service(globals::client_host, nullptr, 50);
        enet_peer_reset(globals::session_peer);

        globals::session_peer = nullptr;
        globals::session_id = UINT16_MAX;
        globals::session_tick_dt = UINT64_MAX;
        globals::session_username = std::string();

        globals::is_singleplayer = true;

        globals::player = entt::null;
        globals::registry.clear();
        chunk_visibility::cleanup();
        
        client_chat::clear();
    }
}

void session::mp::send_login_request(void)
{
    protocol::LoginRequest packet = {};
    packet.version = protocol::VERSION;
    packet.vdef_checksum = vdef::calc_checksum();
    packet.identity = login::identity;
    packet.username = login::username;
    protocol::send(globals::session_peer, nullptr, packet);
    
    progress::set_title("connecting.logging_in");
    globals::gui_screen = GUI_PROGRESS;
}

void session::sp::update(void)
{

}

void session::sp::update_late(void)
{
    if(globals::is_singleplayer && globals::registry.valid(globals::player)) {
        unloader::update_late();
    }
}

void session::sp::load_world(const std::string &universe_directory)
{
    session::invalidate();

    universe::setup("debug");

    unloader::init_late(view::max_distance);

    globals::player = globals::registry.create();
    globals::registry.emplace<HeadComponent>(globals::player);
    globals::registry.emplace<PlayerComponent>(globals::player);
    globals::registry.emplace<TransformComponent>(globals::player);
    globals::registry.emplace<VelocityComponent>(globals::player);

    globals::gui_screen = GUI_SCREEN_NONE;
}

void session::sp::unload_world(void)
{
    universe::save_everything();

    session::invalidate();
}
