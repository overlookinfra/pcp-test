/**
 * @file
 * Models pcp-test options.
 */

#pragma once

#include <string>
#include <vector>

namespace pcp_test {

struct application_options
{
    bool debug;             // log debug information
    std::string test;       // the test that will be executed
    std::string logfile;    // path to the log file
};

}  // namespace pcp_test
