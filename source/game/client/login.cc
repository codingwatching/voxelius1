// SPDX-License-Identifier: BSD-2-Clause
#include "client/precompiled.hh"
#include "client/login.hh"

#include "common/cmdline.hh"
#include "common/config.hh"
#include "common/crc64.hh"
#include "common/epoch.hh"

#include "shared/protocol.hh"

#include "client/gui/settings.hh"

#include "client/globals.hh"


std::uint16_t login::mode = protocol::LoginRequest::MODE_CLASSIC;
std::uint64_t login::identity = UINT64_MAX;
std::string login::username = std::string();

static std::size_t response_body_callback(const void *contents, std::size_t size, std::size_t nmemb, std::string *response_body)
{
    const std::size_t chunk_size = size * nmemb;
    response_body->append(reinterpret_cast<const char *>(contents), chunk_size);
    return chunk_size;
}

static void use_classic_credentials(void)
{
    spdlog::warn("login: using classic credentials");

    login::mode = protocol::LoginRequest::MODE_CLASSIC;
    login::identity = epoch::microseconds();
    login::username = std::string("player");

    Config::add(globals::client_config, "login.classic_identity", login::identity);
    Config::add(globals::client_config, "login.classic_username", login::username);

    settings::add_input(1, settings::GENERAL, "login.classic_username", login::username, true, false);
}

static void use_itch_io_credentials(const char *itch_key)
{
    spdlog::warn("login: using itch.io credentials");

    login::mode = protocol::LoginRequest::MODE_ITCH_IO;
    login::identity = epoch::microseconds();
    login::username = std::string("player");

    curl_global_init(CURL_GLOBAL_DEFAULT);

    auto curl = curl_easy_init();

    if(curl == nullptr) {
        spdlog::critical("login: curl_easy_init failed");
        std::terminate();
    }

    curl_easy_setopt(curl, CURLOPT_URL, "https://itch.io/api/1/jwt/me");
    curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    curl_slist *headers = nullptr;
    curl_slist_append(headers, fmt::format("Authorization: Bearer {}", itch_key).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    std::string response_body;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &response_body_callback);

    auto result = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    if(result != CURLE_OK) {
        spdlog::critical("login: curl_easy_perform failed with error code {}", static_cast<int>(result));
        std::terminate();
    }

    JSON_Value *jsonv = json_parse_string(response_body.c_str());
    const JSON_Object *json = json_value_get_object(jsonv);
    const JSON_Object *user = json_object_get_object(json, "user");
    const char *json_username = json_object_get_string(user, "username");

    if(jsonv == nullptr) {
        spdlog::critical("login: malformed JSON response body");
        std::terminate();
    }

    if((json == nullptr) || (user == nullptr)) {
        spdlog::critical("login: malformed JSON response contents");
        json_value_free(jsonv);
        std::terminate();
    }

    if(json_username == nullptr) {
        spdlog::critical("login: JSON response has no username");
        json_value_free(jsonv);
        std::terminate();
    }

    login::mode = protocol::LoginRequest::MODE_ITCH_IO;
    login::identity = static_cast<std::uint64_t>(json_object_get_number(user, "id"));
    login::username = std::string(json_username);
}

void login::init(void)
{
    if(cmdline::contains("login-classic")) {
        use_classic_credentials();
        return;
    }

    if(const char *itch_key = std::getenv("ITCHIO_API_KEY")) {
        use_itch_io_credentials(itch_key);
        return;
    }

    // As a last resort, the login system will
    // still use classic credentials; public test
    // servers shouldn't reject client connections
    // with classic credentials, at least for now
    use_classic_credentials();
}

void login::init_late(void)
{
    spdlog::info("login: identity: {}", login::identity);
    spdlog::info("login: username: {}", login::username);
}
