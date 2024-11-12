// SPDX-License-Identifier: BSD-2-Clause
#pragma once
#include <game/shared/chunk.hh>
#include <game/shared/local_coord.hh>
#include <game/shared/voxel_coord.hh>

struct VoxelSetEvent final {
    std::size_t index {};
    ChunkCoord cpos {};
    LocalCoord lpos {};
    VoxelCoord vpos {};
    Chunk *chunk {};
    Voxel voxel {};    
};
