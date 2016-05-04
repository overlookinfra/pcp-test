/**
 * @file
 * Models a PCP client.
 */

#pragma once

#include <pcp-test/client_configuration.hpp>
#include <pcp-test/pcp_connector.hpp>

namespace pcp_test {

struct client
{
    client_configuration configuration;
    pcp_connector connector;

    client() = delete;
    explicit client(client_configuration config);
};

}  // namespace pcp_test
