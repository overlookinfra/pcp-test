/**
 * @file
 * Stores functions for configuring the application.
 */

#pragma once

#include <pcp-test/application_options.hpp>

#include <string>

namespace pcp_test {
namespace configuration {

void setup_logging(const std::string& logfile, bool debug);

application_options get_application_options(int argc, char** argv);

void validate_test_type(application_options& a_o);

void parse_configfile_and_process_options(application_options& a_o);

void validate_application_options(application_options& a_o);

}  // namespace configuration
}  // namespace pcp_test
