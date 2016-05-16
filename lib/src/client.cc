#include <pcp-test/client.hpp>

#include <leatherman/logging/logging.hpp>

#include <cpp-pcp-client/connector/errors.hpp>

#include <utility>  // std::move

namespace pcp_test {

client::client(client_configuration cfg)
    : configuration {std::move(cfg)},
      connector {configuration.broker_ws_uris[0],
                 configuration.client_type,
                 configuration.ca,
                 configuration.crt,
                 configuration.key,
                 configuration.connection_timeout_ms}
{
    LOG_INFO("Instantiating client!");

void client::send_request(const message& request,
                  const std::vector<std::string>& endpoints)
{
    try {
        connector.send(endpoints,
                       schemas::REQUEST_TYPE,
                       configuration.message_timeout_s,
                       request.get_data());
        LOG_DEBUG("Sent request %1%", request.transaction());
    } catch (PCPClient::connection_error& e) {
        LOG_ERROR("Failed to send request request %1%: %2%",
                  request.transaction(), e.what());
    }
}

void client::reply(const message& request)
{
    try {
        connector.send({request.sender()},
                       schemas::RESPONSE_TYPE,
                       configuration.message_timeout_s,
                       request.get_data());
        LOG_DEBUG("Replied to request %1%", request.transaction());
    } catch (PCPClient::connection_error& e) {
        LOG_ERROR("Failed to reply to request %1%: %2%",
                  request.transaction(), e.what());
    }
}

void client::reply_with_error(const message& request,
                              const std::string& err_msg)
{
    auto data = request.get_data();
    data.set<std::string>("err_msg", err_msg);

    try {
        connector.send({request.sender()},
                       schemas::ERROR_TYPE,
                       configuration.message_timeout_s,
                       std::move(data));
        LOG_DEBUG("Replied to request %1% with an error", request.transaction());
    } catch (PCPClient::connection_error& e) {
        LOG_ERROR("Failed to reply to request %1% with an error: %2%",
                  request.transaction(), e.what());
    }
}
}

}  // namespace pcp_test
