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

// NOTE(ale): boost::future/packaged_task have different semantics than std::
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
    : idx {1},
      num_endpoints {a_o.connection_test_parameters.get<int>(conn_par::NUM_ENDPOINTS)},
      concurrency {a_o.connection_test_parameters.get<int>(conn_par::CONCURRENCY)},
      endpoints_increment_ {a_o.connection_test_parameters.get<int>(conn_par::ENDPOINTS_INCREMENT)},
      concurrency_increment_ {a_o.connection_test_parameters.get<int>(conn_par::CONCURRENCY_INCREMENT)}
{
}

connection_test_run& connection_test_run::operator++()
{
    idx++;
    num_endpoints += endpoints_increment_;
    concurrency   += concurrency_increment_;
    return *this;
}

std::string connection_test_run::to_string() const
{
    return (boost::format("run %1%: %2% concurrent sets of %3% endpoints")
            % idx % concurrency % num_endpoints).str();
}

//
// connection_test_result
//

connection_test_result::connection_test_result(const connection_test_run& run)
    : num_endpoints {run.num_endpoints},
      concurrency {run.concurrency},
      num_failures {0}
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

connection_test::connection_test(const application_options& a_o)
    : app_opt_ {a_o},
      num_runs_ {app_opt_.connection_test_parameters.get<int>(conn_par::NUM_RUNS)},
      inter_run_pause_ms_ {static_cast<unsigned int>(
          app_opt_.connection_test_parameters.get<int>(conn_par::INTER_RUN_PAUSE_MS))},
      inter_endpoint_pause_ms_ {static_cast<unsigned int>(
          app_opt_.connection_test_parameters.get<int>(conn_par::INTER_ENDPOINT_PAUSE_MS))},
      ws_connection_timeout_ms_ {static_cast<unsigned int>(
          app_opt_.connection_test_parameters.get<int>(conn_par::WS_CONNECTION_TIMEOUT_MS))},
      association_ttl_s_ {static_cast<unsigned int>(
          app_opt_.connection_test_parameters.get<int>(conn_par::ASSOCIATION_TTL_S))},
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
        results_file_stream_ << results << '\n';
        boost::nowide::cout << results << '\n';
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
        << "  " << num_runs_ << " runs, "
        << inter_run_pause_ms_ << " ms pause between each run\n"
        << "  " << inter_endpoint_pause_ms_
        << " ms pause between each endpoint connection\n"
        << "  WebSocket connection timeout " << ws_connection_timeout_ms_
        << " ms; Association Request TTL " << association_ttl_s_ << " s\n\n";
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
        << "\n  Connection test: finished in " << duration_m << " m "
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

int connect_clients_serially(std::vector<std::shared_ptr<client>> client_ptrs,
                             const unsigned int inter_endpoint_pause_ms)
{
    int num_failures {0};
    static std::chrono::milliseconds pause {inter_endpoint_pause_ms};
    bool associated {};

    for (auto e_p : client_ptrs) {
        try {
            associated = true;
            e_p->connector.connect(1);
            associated &= e_p->connector.isAssociated();
            std::this_thread::sleep_for(pause);
            associated &= e_p->connector.isAssociated();
            if (!associated)
                num_failures++;
        } catch (PCPClient::connection_error) {
            num_failures++;
        }
    }

    return num_failures;
}

connection_test_result connection_test::perform_current_run()
{
    connection_test_result results {current_run_};
    using task_type = int(std::vector<std::shared_ptr<client>>,
                          const unsigned int);
    std::vector<std::future<int>> task_futures {};
    std::vector<std::shared_ptr<client>> all_client_ptrs {};
    int endpoint_idx {};
    std::string c_type {"CONNECTION_TEST_CLIENT"};
    client_configuration c_cfg {"0000agent",
                                c_type,
                                app_opt_.broker_ws_uris,
                                app_opt_.certificates_dir,
                                ws_connection_timeout_ms_,
                                association_ttl_s_};

    // Spawn concurrent tasks

    for (auto task_idx = 0; task_idx < current_run_.concurrency; task_idx++) {
        std::vector<std::shared_ptr<client>> task_client_ptrs {};
        endpoint_idx = 0;

        auto add_client =
            [&c_cfg, &all_client_ptrs, &task_client_ptrs] (const std::string& name)
            {
                c_cfg.common_name = name;
                c_cfg.update_cert_paths();
                auto c_ptr = std::make_shared<client>(c_cfg);
                all_client_ptrs.push_back(c_ptr);
                task_client_ptrs.push_back(std::move(c_ptr));
            };

        for (const auto& name : app_opt_.agents) {
            if (++endpoint_idx > current_run_.num_endpoints)
                break;
            add_client(name);
        }

        if (endpoint_idx <= current_run_.num_endpoints) {
            for (const auto& name : app_opt_.controllers) {
                if (++endpoint_idx > current_run_.num_endpoints)
                    break;
                add_client(name);
            }
        }

        std::packaged_task<task_type> connection_task {&connect_clients_serially};
        task_futures.push_back(connection_task.get_future());

        try {
            std::thread(std::move(connection_task),
                        std::move(task_client_ptrs),
                        inter_endpoint_pause_ms_).detach();
            LOG_DEBUG("Run %1% - started connection task n.%2%",
                      current_run_.idx, task_idx + 1);
        } catch (std::exception& e) {
            boost::nowide::cout
                << "\n" << util::red("   [ERROR]   ")
                << "failed to start connection threads: " << e.what() << "\n";
            throw fatal_error { "failed to start connection threads" };
        }
    }

    // Wait for threads to complete and get the number of failures (as futures)

    std::chrono::seconds timeout_s {
        5 + static_cast<int>(current_run_.num_endpoints * ws_connection_timeout_ms_ / 1000)};

    for (std::size_t thread_idx = 0; thread_idx < task_futures.size(); thread_idx++) {
        if (task_futures[thread_idx].wait_for(timeout_s) != std::future_status::ready) {
            LOG_DEBUG("run #%1% - thread  %2% timed out", current_run_.idx, thread_idx);
            results.num_failures += current_run_.num_endpoints;
        } else {
            try {
                results.num_failures += task_futures[thread_idx].get();
            } catch (std::exception& e) {
                LOG_DEBUG("run #%1% - thread failure: %2%", current_run_.idx, e.what());
                results.num_failures += current_run_.num_endpoints;
            }
        }
    }

    return results;
}

}  // namespace pcp_test
