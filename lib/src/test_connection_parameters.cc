#include <pcp-test/test_connection_parameters.hpp>

namespace pcp_test{
namespace connection_test_parameters {

const std::string NUM_RUNS {"num-runs"};
const std::string INTER_RUN_PAUSE_MS {"inter-run-pause-ms"};
const std::string NUM_ENDPOINTS {"num-endpoints"};
const std::string INTER_ENDPOINT_PAUSE_MS {"inter-endpoint-pause-ms"};
const std::string CONCURRENCY {"concurrency"};
const std::string ENDPOINTS_INCREMENT {"endpoints-increment"};
const std::string CONCURRENCY_INCREMENT {"concurrency-increment"};
const std::string RANDOMIZE_INTER_ENDPOINT_PAUSE {"randomize-inter-endpoint-pause"};
const std::string INTER_ENDPOINT_PAUSE_RNG_SEED {"inter-endpoint-pause-rng-seed"};
const std::string WS_CONNECTION_TIMEOUT_MS {"ws-connection-timeout-ms"};
const std::string WS_CONNECTION_CHECK_INTERVAL_S {"ws-connection-check-interval-s"};
const std::string ASSOCIATION_TIMEOUT_S {"association-timeout-s"};
const std::string ASSOCIATION_REQUEST_TTL_S {"association-request-ttl-s"};
const std::string PERSIST_CONNECTIONS {"persist-connections"};
const std::string SHOW_STATS {"show-stats"};

}  // namespace connection_test_parameters
}  // namespace pcp_test
