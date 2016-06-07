/**
 * @file
 * Stores functions for configuring the application.
 */

#pragma once

#include <pcp-test/application_options.hpp>

#include <string>

namespace pcp_test {
namespace configuration {

// Throw a std::out_of_range in case of invalid log level.
void setup_logging(const std::string& logfile,
                   const std::string& loglevel,
                   const std::string& client_loglevel);

// Parse the command line arguments and return an
// application_options instance.
// In case --help or --version were specified, display
// the requested message and exit with EXIT_SUCCESS.
// Throw a configuration_error in case of failure.
application_options get_application_options(int argc, char** argv);

// Validate the test_type argument.
void validate_test_type(application_options& a_o);

// Parse the configuration file.
// In case a given option was not specified by command line,
// update the application_options instance with the value
// stored in the configuration file or with the default
// hardcoded value.
void parse_configfile_and_process_options(application_options& a_o);

// Throw a configuration_error in case of invalid options.
void validate_application_options(application_options& a_o);

}  // namespace configuration
}  // namespace pcp_test
