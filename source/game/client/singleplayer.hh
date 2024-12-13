// SPDX-License-Identifier: BSD-2-Clause
#pragma once
#include "config/cmake.hh"

#if ENABLE_SINGLEPLAYER

namespace singleplayer
{
void startup(void);
void shutdown(void);
void update(void);
} // namespace singleplayer

#endif /* ENABLE_SINGLEPLAYER */
