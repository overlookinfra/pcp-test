#include <catch.hpp>

#include <pcp-test/pcp-test.hpp>
#include <pcp-test/client.hpp>
#include <pcp-test/client_configuration.hpp>

#include <pcp-test/root_path.h>
#include <pcp-test/version.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <utility>
#include <string>
#include <vector>

namespace fs = boost::filesystem;

SCENARIO("version() returns the version", "[configuration]")
{
    REQUIRE(pcp_test::version() == PCP_TEST_VERSION_WITH_COMMIT);
}

SCENARIO("client ctor", "[configuration]")
{
    auto certs_path =
        (fs::path {PCP_TEST_ROOT_PATH} / "test-resources" / "ssl").string();
    std::string client_type {"test_client"};
    std::vector<std::string> uris {"wss://localhost:8142"};
    pcp_test::client_configuration cc {std::string("0000agent"),
                                       client_type,
                                       uris,
                                       certs_path};
    
    REQUIRE_NOTHROW(pcp_test::client {std::move(cc)});
}
