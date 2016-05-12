#include <pcp-test/pcp_connector.hpp>

#include <leatherman/logging/logging.hpp>

namespace pcp_test {

pcp_connector::pcp_connector(const client_configuration& cfg)
    : PCPClient::Connector {cfg.broker_ws_uris[0],
                            cfg.client_type,
                            cfg.ca,
                            cfg.crt,
                            cfg.key,
                            cfg.connection_timeout_ms}
{
    LOG_TRACE("Instantiating client!");
}

}  // namespace pcp_test
