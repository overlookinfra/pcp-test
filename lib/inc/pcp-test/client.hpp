/**
 * @file
 * Models a PCP client.
 */

#pragma once

#include <pcp-test/client_configuration.hpp>

#include <cpp-pcp-client/connector/connector.hpp>

namespace pcp_test {

struct client
{
    client_configuration configuration;
    PCPClient::Connector connector;

    client() = delete;
    explicit client(client_configuration config);
};

}  // namespace pcp_test
