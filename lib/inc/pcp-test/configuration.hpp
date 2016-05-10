/**
 * @file
 * Stores functions for configuring the application.
 */

#pragma once

#include <pcp-test/application_options.hpp>

#include <string>

namespace pcp_test {
namespace configuration {

// Throw a std::out_of_range in case of invalid loglevel.
void setup_logging(const std::string& logfile, const std::string& loglevel);

application_options get_application_options(int argc, char** argv);

void validate_test_type(application_options& a_o);

// Parse the configuration file.
// In case a given option was not specified by command line,
// update the application_options instance with the value
// stored in the configuration file or with the default
// hardcoded value.
void parse_configfile_and_process_options(application_options& a_o);

void validate_application_options(application_options& a_o);

}  // namespace configuration
}  // namespace pcp_test
