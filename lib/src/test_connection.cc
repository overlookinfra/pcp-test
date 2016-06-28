#include <pcp-test/test_connection.hpp>
#include <pcp-test/test_connection_parameters.hpp>
#include <pcp-test/util.hpp>
#include <pcp-test/errors.hpp>
#include <pcp-test/client.hpp>
#include <pcp-test/client_configuration.hpp>

#include <cpp-pcp-client/connector/errors.hpp>

#include <leatherman/logging/logging.hpp>

#include <leatherman/json_container/json_container.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <boost/format.hpp>

#include <boost/nowide/iostream.hpp>

#include <memory>
#include <vector>
#include <algorithm>
#include <math.h>

// NOTE(ale): boost::future/async have different semantics than std::
#include <thread>
#include <future>

namespace pcp_test {


namespace lth_jc   = leatherman::json_container;
namespace conn_par = pcp_test::connection_test_parameters;
namespace fs       = boost::filesystem;

void run_connection_test(const application_options& a_o)
{
    connection_test test {a_o};
    test.start();
}

//
// connection_test_run
//

connection_test_run::connection_test_run(const application_options& a_o)
    : endpoints_increment_ {
        a_o.connection_test_parameters.get<int>(conn_par::ENDPOINTS_INCREMENT)},
      concurrency_increment_ {
        a_o.connection_test_parameters.get<int>(conn_par::CONCURRENCY_INCREMENT)},
      endpoint_timeout_s_ {
        static_cast<int>(
            std::max(1.0,
                     std::ceil(a_o.connection_test_parameters.get<int>(
                        conn_par::INTER_ENDPOINT_PAUSE_MS) / 1000)))
        + a_o.connection_test_parameters.get<int>(conn_par::ASSOCIATION_TIMEOUT_S)},
      idx {1},
      num_endpoints {a_o.connection_test_parameters.get<int>(conn_par::NUM_ENDPOINTS)},
      concurrency {a_o.connection_test_parameters.get<int>(conn_par::CONCURRENCY)},
      timeout_s {num_endpoints * endpoint_timeout_s_}
{
}

connection_test_run& connection_test_run::operator++()
{
    idx++;
    num_endpoints += endpoints_increment_;
    concurrency   += concurrency_increment_;

    if (endpoints_increment_)
        timeout_s += endpoints_increment_ * endpoint_timeout_s_;

    return *this;
}

std::string connection_test_run::to_string() const
{
    return (boost::format("run %1%: %2% concurrent sets of %3% endpoints; "
                          "timeout for each set %4% s")
            % idx % concurrency % num_endpoints % timeout_s.count()).str();
}

//
// connection_test_result
//

connection_test_result::connection_test_result(const connection_test_run& run)
    : num_endpoints {run.num_endpoints},
      concurrency {run.concurrency},
      num_failures {0},
      conn_stats {}
{
}

std::ostream & operator<< (std::ostream& out, const connection_test_result& r)
{
    auto tot_connections = r.num_endpoints * r.concurrency;
    if (r.num_failures) {
        out << util::red("  [FAILURE]  ") << r.num_failures
            << " connection failures out of "
            << tot_connections << " connection attempts";
    } else {
        out << util::green("  [SUCCESS]  ") << tot_connections
            << " successful connections";
    }

    return out;
}

std::ofstream & operator<< (boost::nowide::ofstream& out,
                            const connection_test_result& r)
{
    out << r.num_endpoints << ","
        << r.concurrency << ","
        << r.num_failures;

    return out;
}

//
// connection_test
//

static constexpr uint32_t DEFAULT_WS_CONNECTION_TIMEOUT_MS {1500};
static constexpr uint32_t DEFAULT_WS_CONNECTION_CHECK_INTERVAL_S {15};

connection_test::connection_test(const application_options& a_o)
    : app_opt_(a_o),
      num_runs_ {app_opt_.connection_test_parameters.get<int>(conn_par::NUM_RUNS)},
      inter_run_pause_ms_ {static_cast<unsigned int>(
          app_opt_.connection_test_parameters.get<int>(conn_par::INTER_RUN_PAUSE_MS))},
      inter_endpoint_pause_ms_ {static_cast<unsigned int>(
          app_opt_.connection_test_parameters.get<int>(conn_par::INTER_ENDPOINT_PAUSE_MS))},
      ws_connection_timeout_ms_ {
            app_opt_.connection_test_parameters.includes(conn_par::WS_CONNECTION_TIMEOUT_MS)
            ? static_cast<unsigned int>(
                app_opt_.connection_test_parameters.get<int>(conn_par::WS_CONNECTION_TIMEOUT_MS))
            : DEFAULT_WS_CONNECTION_TIMEOUT_MS},
      ws_connection_check_interval_s_ {
            app_opt_.connection_test_parameters.includes(conn_par::WS_CONNECTION_CHECK_INTERVAL_S)
            ? static_cast<unsigned int>(
                app_opt_.connection_test_parameters.get<int>(conn_par::WS_CONNECTION_CHECK_INTERVAL_S))
            : DEFAULT_WS_CONNECTION_CHECK_INTERVAL_S},
      association_timeout_s_ {
            app_opt_.connection_test_parameters.includes(conn_par::ASSOCIATION_TIMEOUT_S)
            ? static_cast<unsigned int>(
                app_opt_.connection_test_parameters.get<int>(conn_par::ASSOCIATION_TIMEOUT_S))
            : DEFAULT_ASSOCIATION_TIMEOUT_S},
      association_request_ttl_s_ {
            app_opt_.connection_test_parameters.includes(conn_par::ASSOCIATION_REQUEST_TTL_S)
            ? static_cast<unsigned int>(
                app_opt_.connection_test_parameters.get<int>(conn_par::ASSOCIATION_REQUEST_TTL_S))
            : DEFAULT_ASSOCIATION_REQUEST_TTL_S},
      persist_connections_ {
            app_opt_.connection_test_parameters.includes(conn_par::PERSIST_CONNECTIONS)
            ? app_opt_.connection_test_parameters.get<bool>(conn_par::PERSIST_CONNECTIONS)
            : false},
      show_stats_ {
            app_opt_.connection_test_parameters.includes(conn_par::SHOW_STATS)
            ? app_opt_.connection_test_parameters.get<bool>(conn_par::SHOW_STATS)
            : false},
      current_run_ {app_opt_},
      results_file_name_ {(boost::format("connection_test_%1%.csv")
                           % util::get_short_datetime()).str()},
      results_file_stream_ {(fs::path(app_opt_.results_dir)
                             / results_file_name_).string()}
{
    if (!results_file_stream_.is_open())
        throw fatal_error {((boost::format("failed to open %1%")
                             % results_file_name_).str())};
}

void connection_test::start()
{
    auto start_time = std::chrono::system_clock::now();
    LOG_INFO("Requested %1% runs", num_runs_);
    boost::format run_msg_fmt {"Starting %1%\n"};
    display_setup();

    do {
        boost::nowide::cout << (run_msg_fmt % current_run_.to_string()).str();
        auto results = perform_current_run();
        results_file_stream_ << results;
        boost::nowide::cout << results;

        if (show_stats_) {
            results_file_stream_ << results.conn_stats;
            boost::nowide::cout << results.conn_stats;
        }

        results_file_stream_ << '\n';
        boost::nowide::cout << '\n';
        ++current_run_;

        if (current_run_.idx <= num_runs_) {
            // Be nice with the broker and pause (fixed + 50 ms per endpoint)
            std::this_thread::sleep_for(std::chrono::milliseconds(
                inter_run_pause_ms_
                + current_run_.num_endpoints * current_run_.concurrency * 50));
        }
    } while (current_run_.idx <= num_runs_);

    display_execution_time(start_time);
}

void connection_test::display_setup()
{
    const auto& p = app_opt_.connection_test_parameters;
    boost::nowide::cout
        << "\nConnection test setup:\n"
        << "  " << p.get<int>(conn_par::CONCURRENCY) << " concurrent sets (+"
        << p.get<int>(conn_par::CONCURRENCY_INCREMENT) << " per run) of "
        << p.get<int>(conn_par::NUM_ENDPOINTS) << " endpoints (+"
        << p.get<int>(conn_par::ENDPOINTS_INCREMENT) << " per run)\n"
        << "  " << num_runs_ << " runs, ("
        << inter_run_pause_ms_ << " + 50 * num_endpoints) ms pause between each run\n"
        << "  " << inter_endpoint_pause_ms_
        << " ms pause between each endpoint connection\n"
        << "  WebSocket connection timeout " << ws_connection_timeout_ms_ << " ms\n"
        << "  Association timeout " << association_timeout_s_
        << " s; Association Request TTL " << association_request_ttl_s_ << " s\n"
        << "  keep WebSocket connections alive: ";

    if (persist_connections_) {
        boost::nowide::cout << "yes, by pinging every "
                            << ws_connection_check_interval_s_ << " s\n\n";
    } else {
        boost::nowide::cout << "no\n\n";
    }
}

void connection_test::display_execution_time(
        std::chrono::system_clock::time_point start_time)
{
    auto end_time = std::chrono::system_clock::now();
    auto duration_m = std::chrono::duration_cast<std::chrono::minutes>(
            end_time - start_time).count();
    auto duration_s = std::chrono::duration_cast<std::chrono::seconds>(
            end_time - start_time).count() - (duration_m * 60);

    boost::nowide::cout
        << "\nConnection test: finished in " << duration_m << " m "
        << duration_s << " s";

    if (current_run_.idx <= num_runs_) {
        auto executed_runs = current_run_.idx - 1;
        boost::nowide::cout
            << "; only the first "
            << (executed_runs > 1
                    ? ((boost::format("%1% runs were") % executed_runs)).str()
                    : "run was")
            << " executed\n" << std::endl;
    } else {
        boost::nowide::cout  << "\n" << std::endl;
    }
}

// Connection task

int connect_clients_serially(std::vector<std::unique_ptr<client>> client_ptrs,
                             const unsigned int inter_endpoint_pause_ms,
                             const bool persist_connections,
                             const uint32_t connection_check_interval_s,
                             std::shared_ptr<connection_timings_accumulator> timings_acc_ptr,
                             const unsigned int task_id)
{
    int num_failures {0};
    static std::chrono::milliseconds pause {inter_endpoint_pause_ms};

    for (auto &e_p : client_ptrs) {
        try {
            bool associated = true;
            e_p->connect(1);
            associated &= e_p->isAssociated();

            if (timings_acc_ptr) {
                auto ws_timings = e_p->getConnectionTimings();
                timings_acc_ptr->accumulate_tcp_us(
                        ws_timings.getTCPInterval().count());
                timings_acc_ptr->accumulate_ws_open_handshake_us(
                        ws_timings.getOpeningHandshakeInterval().count());

                if (associated) {
                    auto ass_timings = e_p->getAssociationTimings();
                    timings_acc_ptr->accumulate_association_ms(
                            ass_timings.getAssociationInterval().count());
                }
            }

            std::this_thread::sleep_for(pause);
            associated &= e_p->isAssociated();

            if (!associated) {
                LOG_WARNING("Connection Task %1%: client %2% is not associated after %3% ms",
                            task_id, e_p->configuration.common_name, pause.count());
                num_failures++;
            } else if (persist_connections) {
                e_p->connector.startMonitoring(1, connection_check_interval_s);
            }
        } catch (const PCPClient::connection_error& e) {
            LOG_WARNING("Connection Task %1%: client %2% failed to connect (%3%) "
                        "- wil wait %4% ms", task_id,
                        e_p->configuration.common_name, e.what(), pause.count());
            num_failures++;
            std::this_thread::sleep_for(pause);
        }
    }

    for (auto &e_p : client_ptrs) {
        try {
            if (persist_connections)
                e_p->connector.stopMonitoring();

            if (timings_acc_ptr && e_p->connector.isAssociated()) {
                auto ass_timings = e_p->connector.getAssociationTimings();
                timings_acc_ptr->accumulate_session_duration_ms(
                        ass_timings.getOverallSessionInterval_ms().count());
            }
        } catch (PCPClient::connection_not_init_error) {
            // Skip, already threw an exception above.
        } catch (const PCPClient::connection_error& e) {
            LOG_WARNING("Connection Task %1%: client %2% failure (%3%)",
                        task_id, e_p->configuration.common_name, e.what());
            num_failures++;
        }
    }

    LOG_WARNING("Connection Task %1%: completed", task_id);
    return num_failures;
}

static const std::string CONNECTION_TEST_CLIENT_TYPE {"CONNECTION_TEST_CLIENT"};

connection_test_result connection_test::perform_current_run()
{
    connection_test_result results {current_run_};
    std::shared_ptr<connection_timings_accumulator> timings_acc_ptr {nullptr};

    if (show_stats_)
        timings_acc_ptr.reset(new connection_timings_accumulator());

    std::vector<std::future<int>> task_futures {};
    client_configuration c_cfg {"0000agent",
                                CONNECTION_TEST_CLIENT_TYPE,
                                app_opt_.broker_ws_uris,
                                app_opt_.certificates_dir,
                                ws_connection_timeout_ms_,
                                association_timeout_s_,
                                association_request_ttl_s_};

    // Spawn concurrent tasks

    auto add_client =
        [&c_cfg] (std::string name,
                  std::vector<std::unique_ptr<client>>& task_client_ptrs) -> void
        {
            c_cfg.common_name = name;
            c_cfg.update_cert_paths();
            auto c_ptr = std::unique_ptr<client>(new client(c_cfg));
            task_client_ptrs.push_back(std::move(c_ptr));
        };

    auto agents_it       = app_opt_.agents.begin();
    auto agents_end      = app_opt_.agents.end();
    auto controllers_it  = app_opt_.controllers.begin();
    auto controllers_end = app_opt_.controllers.end();

    auto get_name =
        [&agents_it, &agents_end, &controllers_it, &controllers_end] () -> std::string
        {
            std::string name {};
            if (agents_it != agents_end) {
                name = *agents_it;
                agents_it++;
            } else if (controllers_it != controllers_end) {
                name = *controllers_it;
                controllers_it++;
            } else {
                assert(false);
            }

            return name;
        };

    for (auto task_idx = 0; task_idx < current_run_.concurrency; task_idx++) {
        std::vector<std::unique_ptr<client>> task_client_ptrs {};

        for (auto idx = 0; idx < current_run_.num_endpoints; idx++)
            add_client(get_name(), task_client_ptrs);

        try {
            task_futures.push_back(
                std::async(std::launch::async,
                           &connect_clients_serially,
                           std::move(task_client_ptrs),
                           inter_endpoint_pause_ms_,
                           persist_connections_,
                           ws_connection_check_interval_s_,
                           timings_acc_ptr,
                           task_idx));
            LOG_DEBUG("Run %1% - started Connection Task n.%2%",
                      current_run_.idx, task_idx + 1);
        } catch (std::exception& e) {
            boost::nowide::cout
                << "\n" << util::red("   [ERROR]   ")
                << "failed to start Connection Task - thread error: " << e.what() << "\n";
            throw fatal_error { "failed to start Connection Task threads" };
        }
    }

    // Wait for threads to complete and get the number of failures (as futures)
    auto start = std::chrono::system_clock::now();
    for (std::size_t thread_idx = 0; thread_idx < task_futures.size(); thread_idx++) {
        auto elapsed = std::chrono::system_clock::now() - start;
        auto timeout = current_run_.timeout_s > elapsed
                       ? current_run_.timeout_s - elapsed
                       : std::chrono::seconds::zero();

        if (task_futures[thread_idx].wait_for(timeout) != std::future_status::ready) {
            LOG_WARNING("Run #%1% - Connection Task %2% timed out",
                        current_run_.idx, thread_idx);
            results.num_failures += current_run_.num_endpoints;
        } else {
            try {
                results.num_failures += task_futures[thread_idx].get();
            } catch (std::exception& e) {
                LOG_WARNING("Run #%1% - Connection Task %2% failure: %3%",
                            current_run_.idx, thread_idx, e.what());
                results.num_failures += current_run_.num_endpoints;
            }
        }
    }

    // Get timing stats
    results.conn_stats = timings_acc_ptr->get_connection_stats();

    return results;
}

}  // namespace pcp_test
