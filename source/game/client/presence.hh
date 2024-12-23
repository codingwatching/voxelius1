// SPDX-License-Identifier: BSD-2-Clause
#pragma once

namespace presence
{
void init(void);
void deinit(void);
} // namespace presence

namespace presence
{
void send_main_menu(void);
void send_playing_multiplayer(void);
void send_playing_singleplayer(void);
} // namespace presence
