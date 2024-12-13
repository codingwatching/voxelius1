// SPDX-License-Identifier: BSD-2-Clause
#pragma once
#include "shared/chunk_coord.hh"

namespace worldgen
{
extern unsigned int max_chunks_per_tick;
extern std::uint64_t seed;
} // namespace worldgen

namespace worldgen
{
void init(void);
void init_late(void);
void deinit(void);
void update(void);
} // namespace worldgen

namespace worldgen
{
void generate(const ChunkCoord &cpos);
} // namespace worldgen
