#include <pcp-test/test_trivial.hpp>
#include <pcp-test/client_configuration.hpp>
#include <pcp-test/client.hpp>
#include <pcp-test/errors.hpp>
#include <pcp-test/schemas.hpp>
#include <pcp-test/message.hpp>

#include <cpp-pcp-client/connector/errors.hpp>
#include <cpp-pcp-client/protocol/chunks.hpp>
#include <cpp-pcp-client/util/thread.hpp>
#include <cpp-pcp-client/util/chrono.hpp>

#include <leatherman/logging/logging.hpp>

#include <leatherman/util/timer.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <boost/format.hpp>

#include <vector>
#include <atomic>
#include <algorithm>

namespace pcp_test {

namespace fs       = boost::filesystem;
namespace lth_util = leatherman::util;

static const std::string TEST_AGENT {"test_agent"};
static const std::string TEST_CONTROLLER {"test_controller"};

class trivial_agent : public client
{
  public:
    explicit trivial_agent(client_configuration config)
        : client(std::move(config))
    {
    }

  private:
    virtual void process_request(const PCPClient::ParsedChunks& parsed_chunks)
    {
        message req {parsed_chunks};
        LOG_INFO("%1%: sending response for transaction '%2%' to %3%",
                 configuration.common_name, req.transaction(),
                 req.sender());
        reply(req);
    }
};

void run_trivial_test(const application_options& a_o)
{
    LOG_INFO("Hello! About to start a trivial test!");

    trivial_agent agent_01 {client_configuration("0001agent",
                                                 TEST_AGENT,
                                                 a_o.broker_ws_uris,
                                                 a_o.certificates_dir)};

    trivial_agent agent_02 {client_configuration("0002agent",
                                                 TEST_AGENT,
                                                 a_o.broker_ws_uris,
                                                 a_o.certificates_dir)};

    client controller {client_configuration("0001controller",
                                            TEST_CONTROLLER,
                                            a_o.broker_ws_uris,
                                            a_o.certificates_dir)};

    std::atomic<int> num_responses {0};
    std::string expected_transaction {"42"};
    controller.response_callback =
        [&num_responses, &expected_transaction] (
                const PCPClient::ParsedChunks& parsed_chunks,
                client* client_ptr)
        {
            message resp {parsed_chunks};
            if (resp.transaction() == expected_transaction) {
                LOG_INFO("%1%: received response for transaction '%2%' from %3%",
                         client_ptr->configuration.common_name,
                         resp.transaction(), resp.sender());
                num_responses++;
            } else {
                LOG_WARNING("%1%: unexpected response for transaction '%2%' from %3%",
                            client_ptr->configuration.common_name,
                            resp.transaction(), resp.sender());
            }
        };

    std::vector<std::reference_wrapper<client>> clients {agent_01, agent_02,
                                                         controller};

    std::vector<PCPClient::Util::thread> tasks {};
    for (client& c : clients)
        tasks.push_back(PCPClient::Util::thread(
                [&]() -> void
                {
                    try {
                        c.connector.connect(3);
                    } catch (const PCPClient::connection_error& e) {
                        LOG_DEBUG("Failed to connect: %1%", e.what());
                    }
                }));

    auto all_associated =
            [&clients]() -> bool
            {
                return std::all_of(clients.begin(), clients.end(),
                                   [](client& c){return c.connector.isAssociated();});
            };

    lth_util::Timer timer {};
    while (!all_associated() && timer.elapsed_seconds() < 5)
        PCPClient::Util::this_thread::sleep_for(
                PCPClient::Util::chrono::milliseconds(100));

    for (auto& task : tasks)
        if (task.joinable())
            task.join();

    if (!all_associated())
        throw fatal_error {"failed to associate"};

    LOG_INFO("All clients are associated; sending request");
    message req {schemas::REQUEST_TYPE,
                      controller.configuration.common_name,
                      expected_transaction};
    std::vector<std::string> endpoints {
            (boost::format("pcp://*/%1%") % TEST_AGENT).str()};
    controller.send_request(req, endpoints);

    timer.reset();
    while (num_responses.load() < 2 && timer.elapsed_seconds() < 5)
        PCPClient::Util::this_thread::sleep_for(
                PCPClient::Util::chrono::milliseconds(100));

    if (num_responses.load() == 2) {
        LOG_INFO("%1% received 2 responses, as expected",
                 controller.configuration.common_name);
    } else {
        LOG_WARNING("%1% received %2% responses!",
                    controller.configuration.common_name, num_responses.load());
    }
}

}  // namespace pcp_test
