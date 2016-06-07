/**
 * @file
 * Connection test - determines how many concurrent connections can be created
 *                   by a given PCP broker.
 */

#pragma once

#include <pcp-test/application_options.hpp>

#include <boost/nowide/fstream.hpp>

#include <ostream>
#include <chrono>

namespace pcp_test {

void run_connection_test(const application_options& a_o);

struct connection_test_run
{
    int idx;
    int num_endpoints;
    int concurrency;

    explicit connection_test_run(const application_options& a_o);

    connection_test_run& operator++();

    std::string to_string() const;

  private:
    int endpoints_increment_;
    int concurrency_increment_;
};

struct connection_test_result
{
    int num_endpoints;
    int concurrency;
    int num_failures;

    explicit connection_test_result(const connection_test_run& run);

    // To stdout (human readable)
    friend std::ostream& operator<< (std::ostream& out_stream,
                                     const connection_test_result& results);

    // To file (csv)
    friend std::ofstream & operator<< (boost::nowide::ofstream& out_stream,
                                       const connection_test_result& results);
};

class connection_test
{
  public:
    explicit connection_test(const application_options& a_o);

    void start();

  private:
    const application_options& app_opt_;
    int num_runs_;
    unsigned int inter_run_pause_ms_;
    unsigned int inter_endpoint_pause_ms_;
    unsigned int ws_connection_timeout_ms_;
    unsigned int association_timeout_s_;
    unsigned int association_request_ttl_s_;
    connection_test_run current_run_;
    std::string results_file_name_;
    boost::nowide::ofstream results_file_stream_;

    void display_setup();
    void display_execution_time(std::chrono::system_clock::time_point start_time);
    connection_test_result perform_current_run();
};

}  // namespace pcp_test
