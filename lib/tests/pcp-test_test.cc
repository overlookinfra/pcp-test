#include <catch.hpp>

#include <pcp-test/version.h>
#include <pcp-test/pcp-test.hpp>
#include <pcp-test/client.hpp>
#include <pcp-test/client_configuration.hpp>

#include <pcp-test/root_path.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <utility>
#include <string>

namespace fs = boost::filesystem;

SCENARIO("version() returns the version", "[configuration]")
{
    REQUIRE(pcp_test::version() == PCP_TEST_VERSION_WITH_COMMIT);
}

std::string get_path(std::string&& s)
{
    static auto root_path  = fs::path {PCP_TEST_ROOT_PATH};
    static auto certs_path = root_path / "test-resources/ssl";
    return (certs_path / s).string();
}

SCENARIO("client ctor", "[configuration]")
{
    pcp_test::client_configuration c {"wss://localhost:8142",
                                      get_path("ca_crt.pem"),
                                      get_path("test/0007agent.example.com_crt.pem"),
                                      get_path("test/0007agent.example.com_key.pem"),
                                      "test_client",
                                      100};
    REQUIRE_NOTHROW(pcp_test::client {std::move(c)});
}
