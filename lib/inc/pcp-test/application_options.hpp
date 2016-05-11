/**
 * @file
 * Models pcp-test options.
 */

#pragma once

#include <string>
#include <set>

namespace pcp_test {

struct application_options
{
    std::string test;           // the test that will be executed
    std::string logfile;        // path to the log file
    std::string loglevel;       // log level
    std::string configfile;     // path to the configuration file

    static bool exists(const std::string& option_name)
    {
        static std::set<std::string> option_names {"logfile",
                                                   "loglevel",
                                                   "configfile"};
        return (option_names.find(option_name) != option_names.end());
    }
};

}  // namespace pcp_test
