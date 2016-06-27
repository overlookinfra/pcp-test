#include <catch.hpp>

#include <pcp-test/connection_stats.hpp>

#include <pcp-test/root_path.h>

#include <boost/nowide/fstream.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <iostream>

namespace pcp_test {

namespace fs = boost::filesystem;

static const auto TEST_PATH = fs::path(PCP_TEST_ROOT_PATH) / "test-resources";

SCENARIO("stats ctor", "[stats]") {
    SECTION("can initialize with a valid accumulator") {
        stats::my_accumulator_t acc {};
        acc(10);
        REQUIRE_NOTHROW(stats(acc));
    }
}

SCENARIO("connection_stats operator<<", "[stats]") {
    connection_timings_accumulator t_acc {};
    t_acc.accumulate_session_duration_ms(100);
    auto c_stats = t_acc.get_connection_stats();

    SECTION("can write to stdout") {
        REQUIRE_NOTHROW(std::cerr << c_stats);
    }

    SECTION("can write to file") {
        auto results_file = (TEST_PATH / "results.out").string();

        if (fs::exists(results_file))
            fs::remove(results_file);

        boost::nowide::ofstream results_s {results_file};

        REQUIRE_NOTHROW(results_s << c_stats);
        REQUIRE(fs::exists(results_file));

        fs::remove(results_file);
    }
}

SCENARIO("connection_timings_accumulator accumulate method", "[stats]") {
    SECTION("correctly accumulates") {
        connection_timings_accumulator t_acc {};
        t_acc.accumulate_session_duration_ms(1000);
        t_acc.accumulate_session_duration_ms(3000);
        auto c_stats = t_acc.get_connection_stats();

        REQUIRE(c_stats.session_duration_ms.mean == 2000);
    }
}

}  // namespace pcp_test
