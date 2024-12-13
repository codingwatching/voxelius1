// SPDX-License-Identifier: BSD-2-Clause
#pragma once
#include "shared/chunk_coord.hh"

namespace worldgen
{
void setup(std::uint64_t seed);
void update(void);
} // namespace worldgen

namespace worldgen
{
void generate(const ChunkCoord &cpos);
} // namespace worldgen
