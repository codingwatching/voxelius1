// SPDX-License-Identifier: BSD-2-Clause
#include "client/precompiled.hh"
#include "client/singleplayer.hh"

#include "shared/entity/head.hh"
#include "shared/entity/player.hh"
#include "shared/entity/transform.hh"
#include "shared/entity/velocity.hh"

#include "shared/worldgen.hh"

#include "client/globals.hh"
#include "client/gui_screen.hh"
#include "client/session.hh"

#if ENABLE_SINGLEPLAYER

void singleplayer::startup(void)
{
    if(globals::session_peer) {
        session::disconnect("protocol.client_disconnect");
        while(enet_host_service(globals::client_host, nullptr, 50));
        session::invalidate();
    }

    worldgen::max_chunks_per_tick = 16U;
    worldgen::seed = UINT64_C(32);

    worldgen::init();
    worldgen::init_late();

    constexpr int WSIZE = 16;

    for(int x = -WSIZE; x < WSIZE; x += 1) {
        for(int z = -WSIZE; z < WSIZE; z += 1) {
            for(int y = -3; y < 4; y += 1) {
                worldgen::generate(ChunkCoord(x, y, z));
            }
        }
    }

    globals::player = globals::registry.create();
    globals::registry.emplace<HeadComponent>(globals::player);
    globals::registry.emplace<PlayerComponent>(globals::player);
    globals::registry.emplace<TransformComponent>(globals::player);
    globals::registry.emplace<VelocityComponent>(globals::player);

    globals::gui_screen = GUI_SCREEN_NONE;
}

void singleplayer::shutdown(void)
{
    session::invalidate();
    worldgen::deinit();
}

void singleplayer::update(void)
{
    if(globals::registry.valid(globals::player) && !globals::session_peer) {
        worldgen::update();
    }
}

#endif /* ENABLE_SINGLEPLAYER */
