/**
 * @file
 * JSON schemas to validate JSON things.
 */

#pragma once

#include <cpp-pcp-client/validator/schema.hpp>

#include <string>

namespace pcp_test {
namespace schemas {

extern const std::string REQUEST_TYPE;
extern const std::string RESPONSE_TYPE;
extern const std::string ERROR_TYPE;

PCPClient::Schema request();
PCPClient::Schema response();
PCPClient::Schema error();

extern const std::string CONNECTION_TEST_PARAMETERS;

PCPClient::Schema connection_test_parameters();

}  // namespace schemas
}  // namespace pcp-test
