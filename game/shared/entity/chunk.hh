// SPDX-License-Identifier: BSD-2-Clause
#pragma once
#include <game/shared/chunk.hh>
#include <game/shared/chunk_coord.hh>

struct ChunkComponent final {
    ChunkCoord coord {};
    Chunk *chunk {};
};
