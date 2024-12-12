// SPDX-License-Identifier: BSD-2-Clause
#pragma once
#include "shared/chunk.hh"
#include "shared/chunk_coord.hh"

struct ChunkComponent final {
    ChunkCoord coord {};
    Chunk *chunk {};
};
