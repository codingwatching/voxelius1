// SPDX-License-Identifier: BSD-2-Clause
#pragma once
#include "shared/chunk.hh"
#include "shared/chunk_coord.hh"

namespace overworld
{
void init_late(std::uint64_t seed);
} // namespace overworld

namespace overworld
{
void generate_terrain(const ChunkCoord &cpos, VoxelStorage &voxels);
void generate_surface(const ChunkCoord &cpos, VoxelStorage &voxels);
void generate_carvers(const ChunkCoord &cpos, VoxelStorage &voxels);
void generate_features(const ChunkCoord &cpos, VoxelStorage &voxels);
} // namespace overworld
