// SPDX-License-Identifier: BSD-2-Clause
#pragma once

constexpr static const char *DISCORD_APPID = "1277871547935100928";
constexpr static const char *DISCORD_VCLIENT_ICON = "vclient";
constexpr static const char *DISCORD_MP_ICON = "multiplayer";
constexpr static const char *DISCORD_SP_ICON = "singleplayer";

namespace discord_rpc
{
void init(void);
void deinit(void);
} // namespace discord_rpc

namespace discord_rpc
{
void send_main_menu(void);
void send_playing_multiplayer(void);
void send_playing_singleplayer(void);
} // namespace discord_rpc
