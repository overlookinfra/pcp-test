#include <catch.hpp>
#include <pcp-test/version.h>
#include <pcp-test/pcp-test.hpp>

SCENARIO("version() returns the version") {
    REQUIRE(pcp_test::version() == PCP_TEST_VERSION_WITH_COMMIT);
}
