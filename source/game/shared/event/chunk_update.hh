// SPDX-License-Identifier: BSD-2-Clause
#pragma once
#include "shared/world/chunk.hh"
#include "shared/world/chunk_coord.hh"

struct ChunkUpdateEvent final {
    ChunkCoord coord {};
    Chunk *chunk {};
};
