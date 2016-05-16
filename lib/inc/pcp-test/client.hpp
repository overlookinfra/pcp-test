/**
 * @file
 * Models a PCP client.
 */

#pragma once

#include <pcp-test/client_configuration.hpp>
#include <pcp-test/message.hpp>

#include <cpp-pcp-client/protocol/chunks.hpp>
#include <cpp-pcp-client/connector/connector.hpp>

#include <string>
#include <vector>

namespace pcp_test {

struct client
{
    client_configuration configuration;
    PCPClient::Connector connector;

    client() = delete;
    explicit client(client_configuration config);

    // Send the request message to the specified endpoints.
    void send_request(const message& request,
                      const std::vector<std::string>& endpoints);

    // Replies to the specified request message with a
    // response message.
    void reply(const message& request);


    // Replies to the specified request message with an
    // error message.
    void reply_with_error(const message& request,
                          const std::string& err_msg);
};

}  // namespace pcp_test
