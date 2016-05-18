/**
 * @file
 * Plain data structure for encapsulating the client configuration.
 */

#pragma once

#include <string>
#include <vector>

namespace pcp_test {

extern const long DEFAULT_CONNECTION_TIMEOUT_MS;
extern const unsigned int DEFAULT_MESSAGE_TIMEOUT_S;

struct client_configuration
{
    std::string common_name;
    const std::string& client_type;
    const std::vector<std::string>& broker_ws_uris;
    const std::string& certificates_dir;
    long connection_timeout_ms;
    unsigned int message_timeout_s;
    std::string ca;
    std::string crt;
    std::string key;

    void update_cert_paths();

    client_configuration(
            std::string common_name,
            const std::string& client_type,
            const std::vector<std::string>& broker_ws_uris,
            const std::string& certificate_dir,
            long connection_tmeout_ms = DEFAULT_CONNECTION_TIMEOUT_MS,
            unsigned int message_timeout_s = DEFAULT_MESSAGE_TIMEOUT_S);
};

}  // namespace pcp_test
