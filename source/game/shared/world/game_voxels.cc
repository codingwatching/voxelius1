// SPDX-License-Identifier: BSD-2-Clause
#include "shared/precompiled.hh"
#include "shared/world/game_voxels.hh"

#include "shared/world/vdef.hh"


VoxelID game_voxels::cobble = NULL_VOXEL;
VoxelID game_voxels::dirt = NULL_VOXEL;
VoxelID game_voxels::grass = NULL_VOXEL;
VoxelID game_voxels::stone = NULL_VOXEL;
VoxelID game_voxels::vtest = NULL_VOXEL;
VoxelID game_voxels::vtest_ck = NULL_VOXEL;
VoxelID game_voxels::oak_leaves = NULL_VOXEL;
VoxelID game_voxels::oak_planks = NULL_VOXEL;
VoxelID game_voxels::oak_wood = NULL_VOXEL;
VoxelID game_voxels::glass = NULL_VOXEL;

void game_voxels::populate(void)
{
    game_voxels::stone = vdef::create("stone", VoxelType::Cube).add_default_state().build();
    game_voxels::cobble = vdef::create("cobble", VoxelType::Cube).add_default_state().build();
    game_voxels::grass = vdef::create("grass", VoxelType::Cube).add_default_state().build();
    game_voxels::dirt = vdef::create("dirt", VoxelType::Cube).add_default_state().build();
    game_voxels::vtest = vdef::create("vtest", VoxelType::Cube).add_default_state().add_state("chromakey").build();
    game_voxels::vtest_ck = game_voxels::vtest + 1;

    const VoxelID leaves_base = vdef::create("leaves", VoxelType::Cube).add_state("oak").build();
    const VoxelID planks_base = vdef::create("planks", VoxelType::Cube).add_state("oak").build();
    const VoxelID wood_base = vdef::create("wood", VoxelType::Cube).add_state("oak").build();

    game_voxels::oak_leaves = leaves_base + 0;
    game_voxels::oak_planks = planks_base + 0;
    game_voxels::oak_wood = wood_base + 0;

    game_voxels::glass = vdef::create("glass", VoxelType::Cube).add_default_state().build();
}
