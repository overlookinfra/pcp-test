#include <catch.hpp>

#include <pcp-test/version.h>
#include <pcp-test/pcp-test.hpp>
#include <pcp-test/client.hpp>
#include <pcp-test/client_configuration.hpp>

#include <utility>

SCENARIO("version() returns the version", "[main]")
{
    REQUIRE(pcp_test::version() == PCP_TEST_VERSION_WITH_COMMIT);
}

SCENARIO("client ctor", "[main]")
{
    pcp_test::client_configuration c {"wss://broker.example.com:8142",
                                      "/tmp/ca.pem",
                                      "/tmp/crt.pem",
                                      "/tmp/key.pem",
                                      "test_client",
                                      100};
    REQUIRE_NOTHROW(pcp_test::client {std::move(c)});
}
