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
#include <functional>
#include <memory>
#include <vector>

namespace pcp_test {

// pcp_test::client provides two ways of implementing the dispatch
// logic for handling PCP test messages (request, response, error):
// polymorphism, by subclassing and defining process_XXX() methods,
// and callbacks

class client
{
  public:
    // NOTE(ale): PCPClient::Connector's dtor resets all WS event
    // callbacks, so it's safe to pass the instance pointer
    using msg_callback =
        std::function<void(const PCPClient::ParsedChunks& parsed_chunks,
                           client* client_ptr)>;

    client_configuration configuration;
    PCPClient::Connector connector;

    msg_callback request_callback;
    msg_callback response_callback;
    msg_callback error_callback;

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
  protected:
    virtual void process_request(const PCPClient::ParsedChunks& parsed_chunks);
    virtual void process_response(const PCPClient::ParsedChunks& parsed_chunks);
    virtual void process_error(const PCPClient::ParsedChunks& parsed_chunks);
};

}  // namespace pcp_test
