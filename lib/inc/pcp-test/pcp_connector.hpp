/**
 * @file
 * Provides the PCP interface.
 */

#pragma once

#include <pcp-test/client_configuration.hpp>
#include <pcp-test/request.hpp>
#include <pcp-test/response.hpp>

#include <cpp-pcp-client/connector/connector.hpp>

#include <memory>
#include <string>

namespace pcp_test {

class pcp_connector : public PCPClient::Connector {
  public:
    pcp_connector() = delete;

    explicit pcp_connector(const client_configuration& cfg);

    void send_request(const request& req);

    void send_response(const response& resp);
};

}  // namespace pcp_test
