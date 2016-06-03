/**
 * @file
 * Connection test parameters.
 */

#pragma once

#include <string>

namespace pcp_test{
namespace connection_test_parameters {

extern const std::string NUM_RUNS;
extern const std::string INTER_RUN_PAUSE_MS;
extern const std::string NUM_ENDPOINTS;
extern const std::string INTER_ENDPOINT_PAUSE_MS;
extern const std::string CONCURRENCY;
extern const std::string ENDPOINTS_INCREMENT;
extern const std::string CONCURRENCY_INCREMENT;
extern const std::string WS_CONNECTION_TIMEOUT_MS;
extern const std::string ASSOCIATION_TTL_S;
extern const std::string PERSIST_CONNECTIONS;

}  // namespace connection_test_parameters
}  // namespace pcp_test
