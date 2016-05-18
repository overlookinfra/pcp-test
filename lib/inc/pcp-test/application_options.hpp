/**
 * @file
 * Models pcp-test options.
 */

#pragma once

#include <string>
#include <vector>
#include <set>

namespace pcp_test {

struct application_options
{
    std::string test;           // the test that will be executed
    std::string logfile;        // path to the log file
    std::string loglevel;       // log level
    std::string configfile;     // path to the configuration file

    std::vector<std::string> broker_ws_uris;    // WS URIs of PCP brokers
    std::string certificates_dir;               // SSL certs dir
    std::string results_dir;                    // results dir

    static bool exists(const std::string& option_name)
    {
        static std::set<std::string> option_names {"logfile",
                                                   "loglevel",
                                                   "configfile",
                                                   "broker-ws-uris",
                                                   "certificates-dir",
                                                   "results-dir"};
        return (option_names.find(option_name) != option_names.end());
    }
};

}  // namespace pcp_test
