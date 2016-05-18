/**
 * @file
 * Models pcp-test options.
 */

#pragma once

#include <leatherman/json_container/json_container.hpp>

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

    // hidden settings
    std::set<std::string> agents;               // agent CNs
    std::set<std::string> controllers;          // controller CNs

    // configuration parameters for test_connection
    leatherman::json_container::JsonContainer connection_test_parameters;

    static bool is_configuration_file_option(const std::string& option_name)
    {
        static std::set<std::string> option_names {"logfile",
                                                   "loglevel",
                                                   "configfile",
                                                   "broker-ws-uris",
                                                   "certificates-dir",
                                                   "results-dir",
                                                   "connection-test-parameters"};
        return (option_names.find(option_name) != option_names.end());
    }
};

}  // namespace pcp_test
