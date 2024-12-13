// SPDX-License-Identifier: BSD-2-Clause
#pragma once

namespace session
{
void init(void);
void deinit(void);
void invalidate(void);
} // namespace session

namespace session::mp
{
void connect(const std::string &hostname, std::uint16_t port);
void disconnect(const std::string &reason);
void send_login_request(void);
} // namespace session::mp

namespace session::sp
{
void update(void);
void update_late(void);
void load_world(const std::string &world_dir);
void unload_world(void);
} // namespace session::sp
