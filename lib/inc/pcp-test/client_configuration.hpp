/**
 * @file
 * Plain data structure for encapsulating the client configuration.
 */

#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace pcp_test {

extern const long DEFAULT_CONNECTION_TIMEOUT_MS;
extern const uint32_t DEFAULT_ASSOCIATION_TIMEOUT_S;
extern const uint32_t DEFAULT_ASSOCIATION_REQUEST_TTL_S;
extern const uint32_t DEFAULT_MESSAGE_TTL_S;

struct client_configuration
{
    std::string common_name;
    const std::string& client_type;
    const std::vector<std::string>& broker_ws_uris;
    const std::string& certificates_dir;
    long connection_timeout_ms;
    uint32_t association_timeout_s;
    uint32_t association_request_ttl_s;
    uint32_t message_ttl_s;
    std::string ca;
    std::string crt;
    std::string key;

    void update_cert_paths();

    client_configuration(
            std::string common_name,
            const std::string& client_type,
            const std::vector<std::string>& broker_ws_uris,
            const std::string& certificate_dir,
            long connection_timeout_ms         = DEFAULT_CONNECTION_TIMEOUT_MS,
            uint32_t association_timeout_s     = DEFAULT_ASSOCIATION_TIMEOUT_S,
            uint32_t association_request_ttl_s = DEFAULT_ASSOCIATION_REQUEST_TTL_S,
            uint32_t message_ttl_s             = DEFAULT_MESSAGE_TTL_S);
};

}  // namespace pcp_test
