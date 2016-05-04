#include <pcp-test/test_trivial.hpp>
#include <pcp-test/client_configuration.hpp>
#include <pcp-test/client.hpp>
#include <pcp-test/errors.hpp>

#include <pcp-test/root_path.h>

#include <leatherman/logging/logging.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace pcp_test {

namespace fs = boost::filesystem;

void run_trivial_test()
{
    auto root_path  = fs::path(PCP_TEST_ROOT_PATH);
    auto certs_path = root_path / "dev-resources/ssl";

    LOG_INFO("Hello! About to start a trivial test!");

    auto get_path =
        [&certs_path] (std::string&& s) -> std::string
        {
          return (certs_path / s).string();
        };

    pcp_test::client_configuration c {"wss://broker.example.com:8142/pcp/",
                                      get_path("ca/ca_crt.pem"),
                                      get_path("certs/client04.example.com.pem"),
                                      get_path("private_keys/client04.example.com.pem"),
                                      "test_client",
                                      100};

    pcp_test::client trivial_client {c};
    trivial_client.connector.connect(3);
    LOG_INFO("We should be associated with %1% at this point!", c.broker_ws_uri);
    if (!trivial_client.connector.isAssociated())
        throw fatal_error {"failed to associate"};
}

}  // namespace pcp_test
