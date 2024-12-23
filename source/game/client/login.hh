// SPDX-License-Identifier: BSD-2-Clause
#pragma once

namespace login
{
extern std::uint16_t mode;
extern std::uint64_t identity;
extern std::string username;
} // namespace login

namespace login
{
void init(void);
void init_late(void);
} // namespace login
