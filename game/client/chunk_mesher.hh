// SPDX-License-Identifier: BSD-2-Clause
#pragma once
#include <game/client/chunk_vbo.hh>
#include <vector>

struct ChunkMeshComponent final {
    std::vector<ChunkVBO> quad {};
};

namespace chunk_mesher
{
void init(void);
void deinit(void);
void update(void);
} // namespace chunk_mesher
