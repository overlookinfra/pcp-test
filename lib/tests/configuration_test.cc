#include <catch.hpp>

#include <pcp-test/configuration.hpp>
#include <pcp-test/errors.hpp>
#include <pcp-test/application_options.hpp>

#include <pcp-test/root_path.h>

#include <leatherman/logging/logging.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace pcp_test {

namespace fs       = boost::filesystem;
namespace lth_log  = leatherman::logging;

static const auto TEST_PATH = fs::path(PCP_TEST_ROOT_PATH) / "test-resources";

SCENARIO("configuration::setup_logging", "[configuration]") {
    auto log_file = (TEST_PATH / "test.log").string();

    SECTION("can setup leatherman's logging") {
        REQUIRE_NOTHROW(configuration::setup_logging(log_file, "warning"));
    }

    SECTION("correctly sets the log level"){
        configuration::setup_logging(log_file, "fatal");
        REQUIRE(lth_log::get_level() == lth_log::log_level::fatal);
    }

    if (fs::exists(log_file))
        fs::remove(log_file);
}

SCENARIO("configuration::get_application_options", "[configuration]") {
    SECTION("throws a configuration_error in case of invalid option") {
        const char* args[] = {"bin_name", "trivial", "--hermeneutic", "required"};
        REQUIRE_THROWS_AS(
            configuration::get_application_options(4, const_cast<char**>(args)),
            configuration_error);
    }

    SECTION("sets the test type to 'none' if not provided") {
        const char* args[] = {"bin_name", "--loglevel", "debug"};
        auto ao = configuration::get_application_options(3, const_cast<char**>(args));
        REQUIRE(ao.test == "none");
    }
}

SCENARIO("configuration::validate_test_type", "[configuration]") {
    SECTION("throws a configuration_error if the test type is 'none'") {
        application_options ao {};
        ao.test = "none";
        REQUIRE_THROWS_AS(configuration::validate_test_type(ao),
                          configuration_error);
    }

    SECTION("throws a configuration_error in case of invalid test_type") {
        application_options ao {};
        ao.test = "ontological";
        REQUIRE_THROWS_AS(configuration::validate_test_type(ao),
                          configuration_error);
    }
}

static const auto CONFIG_PATH = TEST_PATH / "configuration";

SCENARIO("configuration::parse_configfile_and_process_options", "[configuration]") {
    SECTION("throws a configuration_error in case of bad JSON") {
        application_options ao {};
        ao.configfile = (CONFIG_PATH / "bad.json").string();
        REQUIRE_THROWS_AS(configuration::parse_configfile_and_process_options(ao),
                          configuration_error);
    }

    SECTION("throws a configuration_error in case of an empty file") {
        application_options ao {};
        ao.configfile = (CONFIG_PATH / "empty.json").string();
        REQUIRE_THROWS_AS(configuration::parse_configfile_and_process_options(ao),
                          configuration_error);
    }

    SECTION("is happy with an empty JSON objec") {
        application_options ao {};
        ao.configfile = (CONFIG_PATH / "empty_object.json").string();
        REQUIRE_NOTHROW(configuration::parse_configfile_and_process_options(ao));
    }

    SECTION("successfully parses a valid config file") {
        application_options ao {};
        ao.configfile = (CONFIG_PATH / "good.json").string();
        REQUIRE_NOTHROW(configuration::parse_configfile_and_process_options(ao));
        REQUIRE(ao.logfile == "/tmp/pcp-test.log");
        REQUIRE(ao.loglevel == "warning");
    }

    SECTION("does not supersede command line settings") {
        application_options ao {};
        ao.configfile = (CONFIG_PATH / "good.json").string();
        ao.loglevel = "trace";
        configuration::parse_configfile_and_process_options(ao);
        REQUIRE(ao.loglevel == "trace");

    }

    SECTION("set default values when the option is not specified on file nor CL") {
        application_options ao {};
        // No loglevel is specified - default should be 'info'
        ao.configfile = (CONFIG_PATH / "other_good.json").string();
        configuration::parse_configfile_and_process_options(ao);
        REQUIRE(ao.loglevel == "info");
    }
}

SCENARIO("configuration::validate_application_options", "[configuration]") {
    SECTION("throws a configuration_error if the parent log dir does not exist") {
        application_options bad_log_dir_ao {};
        bad_log_dir_ao.loglevel = "info";
        bad_log_dir_ao.logfile = (CONFIG_PATH / "does" / "not" / "exist").string();
        REQUIRE_THROWS_AS(configuration::validate_application_options(bad_log_dir_ao),
                          configuration_error);
    }

    SECTION("throws a configuration_error in case of invalid log level") {
        application_options bad_log_level_ao {};
        bad_log_level_ao.loglevel = "super_logging";
        bad_log_level_ao.logfile = (CONFIG_PATH / "good.log").string();
        REQUIRE_THROWS_AS(configuration::validate_application_options(bad_log_level_ao),
                          configuration_error);
    }

    SECTION("validates succesfully good options") {
        application_options good_ao {};
        good_ao.loglevel = "info";
        good_ao.logfile = (CONFIG_PATH / "good.log").string();
        REQUIRE_NOTHROW(configuration::validate_application_options(good_ao));
    }
}

}  // namespace pcp_test
