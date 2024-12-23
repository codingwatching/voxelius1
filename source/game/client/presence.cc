// SPDX-License-Identifier: BSD-2-Clause
#include "client/precompiled.hh"
#include "client/presence.hh"

#include "common/epoch.hh"

#include "client/const.hh"


static void on_discord_ready(const DiscordUser *user)
{
    spdlog::info("presence: connected with user {}", user->username);
}

static void on_discord_disconnect(int error_code, const char *message)
{
    spdlog::info("presence: disconnected with code {}: {}", error_code, message);
}

static void on_discord_error(int error_code, const char *message)
{
    spdlog::error("presence: error code {}: {}", error_code, message);
}

void presence::init(void)
{
    DiscordEventHandlers handlers;
    std::memset(&handlers, 0, sizeof(handlers));

    handlers.ready = &on_discord_ready;
    handlers.disconnected = &on_discord_disconnect;
    handlers.errored = &on_discord_error;

    Discord_Initialize(DISCORD_APPID, &handlers, true, nullptr);
}

void presence::deinit(void)
{
    Discord_Shutdown();
}

void presence::send_main_menu(void)
{
    DiscordRichPresence rich_presence;
    std::memset(&rich_presence, 0, sizeof(rich_presence));

    rich_presence.state = "Main menu";
    rich_presence.startTimestamp = epoch::seconds();
    rich_presence.largeImageKey = DISCORD_VCLIENT_ICON;

    Discord_UpdatePresence(&rich_presence);
}

void presence::send_playing_multiplayer(void)
{
    DiscordRichPresence rich_presence;
    std::memset(&rich_presence, 0, sizeof(rich_presence));

    rich_presence.state = "Playing Multiplayer";
    rich_presence.startTimestamp = epoch::seconds();
    rich_presence.largeImageKey = DISCORD_VCLIENT_ICON;
    rich_presence.smallImageKey = DISCORD_MP_ICON;

    Discord_UpdatePresence(&rich_presence);
}

void presence::send_playing_singleplayer(void)
{
    DiscordRichPresence rich_presence;
    std::memset(&rich_presence, 0, sizeof(rich_presence));

    rich_presence.state = "Playing Singleplayer";
    rich_presence.startTimestamp = epoch::seconds();
    rich_presence.largeImageKey = DISCORD_VCLIENT_ICON;
    rich_presence.smallImageKey = DISCORD_SP_ICON;

    Discord_UpdatePresence(&rich_presence);
}
