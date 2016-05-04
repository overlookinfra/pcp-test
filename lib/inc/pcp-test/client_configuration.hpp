/**
 * @file
 * Plain data structure for encapsulating the client configuration.
 */

#pragma once

#include <string>

namespace pcp_test {

struct client_configuration
{
    std::string broker_ws_uri;
    std::string ca;
    std::string crt;
    std::string key;
    std::string client_type;
    long connection_timeout_ms;
};

}  // namespace pcp_test
