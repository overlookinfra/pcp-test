/**
 * @file
 * Abstraction for a PCP Test request message.
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

}  // namespace schemas
}  // namespace pcp-test
