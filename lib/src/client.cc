#include <pcp-test/client.hpp>

#include <leatherman/logging/logging.hpp>

#include <utility>  // std::move

namespace pcp_test {

client::client(client_configuration cfg)
    : configuration {std::move(cfg)},
      connector {configuration}
{
    LOG_INFO("Instantiating client!");
}

}  // namespace pcp_test
