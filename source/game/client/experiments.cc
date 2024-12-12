// SPDX-License-Identifier: BSD-2-Clause
#include "client/precompiled.hh"
#include "client/experiments.hh"

#if ENABLE_EXPERIMENTS

#include "shared/game_voxels.hh"
#include "shared/world.hh"

#include "client/event/glfw_mouse_button.hh"
#include "client/event/glfw_scroll.hh"

#include "client/chat.hh"
#include "client/globals.hh"
#include "client/player_target.hh"


static std::size_t place_index = 0;
static std::vector<Voxel> place_voxels = {};

static void on_glfw_mouse_button(const GlfwMouseButtonEvent &event)
{
    if(!globals::gui_screen && globals::registry.valid(globals::player)) {
        if((event.action == GLFW_PRESS) && (player_target::voxel != NULL_VOXEL)) {
            if(event.button == GLFW_MOUSE_BUTTON_LEFT) {
                world::set_voxel(NULL_VOXEL, player_target::vvec);
                return;
            }

            if(event.button == GLFW_MOUSE_BUTTON_RIGHT) {
                world::set_voxel(place_voxels[place_index], player_target::vvec + player_target::vnormal);
                return;
            }
        }
    }
}

static void on_glfw_scroll(const GlfwScrollEvent &event)
{
    if(!globals::gui_screen && globals::registry.valid(globals::player)) {
        if(const auto delta = cxpr::sign<int>(event.dy)) {
            if((delta > 0) && (place_index > 0))
                place_index -= 1;
            if((delta < 0) && (place_index < (place_voxels.size() - 1)))
                place_index += 1;
            const auto info = vdef::find(place_voxels[place_index]);
            client_chat::print(fmt::format("[debug] {}/{}", info ? info->name : std::string("nullptr"), info ? info->state : std::string("nullptr")));
        }
    }
}

void experiments::init(void)
{
    globals::dispatcher.sink<GlfwMouseButtonEvent>().connect<&on_glfw_mouse_button>();
    globals::dispatcher.sink<GlfwScrollEvent>().connect<&on_glfw_scroll>();
}

void experiments::init_late(void)
{
    place_voxels.push_back(game_voxels::cobble);
    place_voxels.push_back(game_voxels::dirt);
    place_voxels.push_back(game_voxels::grass);
    place_voxels.push_back(game_voxels::stone);
    place_voxels.push_back(game_voxels::vtest);
    place_voxels.push_back(game_voxels::oak_leaves);
    place_voxels.push_back(game_voxels::oak_planks);
    place_voxels.push_back(game_voxels::oak_wood);
    place_voxels.push_back(game_voxels::glass);
}

void experiments::deinit(void)
{

}

void experiments::update(void)
{

}

void experiments::update_late(void)
{

}

#endif /* ENABLE_EXPERIMENTS */
