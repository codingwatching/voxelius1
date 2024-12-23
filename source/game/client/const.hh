// SPDX-License-Identifier: BSD-2-Clause
#pragma once

constexpr static int BASE_WIDTH = 320;
constexpr static int BASE_HEIGHT = 240;

constexpr static int MIN_WIDTH = 2 * BASE_WIDTH;
constexpr static int MIN_HEIGHT = 2 * BASE_HEIGHT;

constexpr static int DEFAULT_WIDTH = 720;
constexpr static int DEFAULT_HEIGHT = 480;

static_assert(DEFAULT_WIDTH >= MIN_WIDTH);
static_assert(DEFAULT_HEIGHT >= MIN_HEIGHT);

constexpr static const char *DISCORD_APPID = "1277871547935100928";
constexpr static const char *DISCORD_VCLIENT_ICON = "vclient";
constexpr static const char *DISCORD_MP_ICON = "multiplayer";
constexpr static const char *DISCORD_SP_ICON = "singleplayer";
