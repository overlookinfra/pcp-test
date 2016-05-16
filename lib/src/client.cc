#include <pcp-test/client.hpp>

#include <leatherman/logging/logging.hpp>

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
}

}  // namespace pcp_test
