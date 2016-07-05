#include <catch.hpp>

#include <pcp-test/random.hpp>

namespace pcp_test {

SCENARIO("exponential_integers ctor", "[random]") {
    SECTION("can instantiate") {
        REQUIRE_NOTHROW(exponential_integers(2));
    }

    SECTION("can instantiate with a given RNG seed") {
        REQUIRE_NOTHROW(exponential_integers(2, 42));
    }
}

SCENARIO("exponential_integers call operator", "[random]") {
    SECTION("does generate integers") {
        exponential_integers e {2};
        REQUIRE_NOTHROW(e());
    }

    SECTION("specifying seed guarantees repeatability") {
        exponential_integers e_1 {2, 42};
        exponential_integers e_2 {2, 42};
        exponential_integers e_3 {2, 69};
        auto v_1 = e_1();
        auto v_2 = e_2();
        auto v_3 = e_3();

        REQUIRE(v_1 == v_2);
        REQUIRE(v_1 != v_3);
    }
}

}  // namespace pcp_test
