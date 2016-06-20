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
        REQUIRE_NOTHROW(configuration::setup_logging(log_file, "warning", "info"));
    }

    SECTION("correctly sets the log level"){
        configuration::setup_logging(log_file, "fatal", "debug");
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
        std::vector<std::string> expected_broker_uris {"broker_01.example.com",
                                                       "broker_02.example.com"};
        REQUIRE_NOTHROW(configuration::parse_configfile_and_process_options(ao));
        REQUIRE(ao.logfile == "/tmp/pcp-test.log");
        REQUIRE(ao.loglevel == "warning");
        REQUIRE(ao.broker_ws_uris == expected_broker_uris);
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
    application_options ao {};
    ao.test = "trivial";
    ao.loglevel = "info";
    ao.client_loglevel = "debug";
    ao.configfile = (CONFIG_PATH / "good.json").string();
    ao.logfile = (CONFIG_PATH / "good.log").string();
    ao.broker_ws_uris = {"wss://localhost:8142/pcp/vNext",
                         "wss://broker.example.com:8142/pcp/"};
    ao.certificates_dir = (TEST_PATH / "ssl").string();
    ao.results_dir = TEST_PATH.string();

    SECTION("throws a configuration_error if the parent log dir does not exist") {
        ao.logfile = (CONFIG_PATH / "does" / "not" / "exist").string();
        REQUIRE_THROWS_AS(configuration::validate_application_options(ao),
                          configuration_error);
    }

    SECTION("throws a configuration_error in case of invalid log level") {
        ao.loglevel = "super_logging";
        REQUIRE_THROWS_AS(configuration::validate_application_options(ao),
                          configuration_error);
    }

    SECTION("throws a configuration_error in case of invalid client log level") {
        ao.client_loglevel = "super_logging";
        REQUIRE_THROWS_AS(configuration::validate_application_options(ao),
                          configuration_error);
    }

    SECTION("throws a configuration_error in case of bad WS URI") {
        SECTION("bad protocol") {
            ao.broker_ws_uris = {"https://localhost:8080/godscop/"};
        }

        SECTION("not secure WebSocket") {
            ao.broker_ws_uris = {"ws://localhost:8142/pcp/"};
        }

        SECTION("no port") {
            ao.broker_ws_uris = {"wss://localhost:8142/pcp/vNext",
                                 "wss://broker.example.com"};
        }

        REQUIRE_THROWS_AS(configuration::validate_application_options(ao),
                          configuration_error);
    }

    SECTION("throws a configuration_error in case of bad Certificates Directory") {
        SECTION("directory does not exist") {
            ao.certificates_dir = (CONFIG_PATH / "does" / "not" / "exist").string();
        }

        SECTION("directory is a regular file") {
            ao.certificates_dir = (CONFIG_PATH / "bad.json").string();
        }

        SECTION("no CA cert") {
            ao.certificates_dir = (TEST_PATH / "ssl_no_ca").string();
        }

        SECTION("no 'test' subdirectory") {
            ao.certificates_dir = (TEST_PATH / "ssl_no_test").string();
        }


        REQUIRE_THROWS_AS(configuration::validate_application_options(ao),
                          configuration_error);
    }

    SECTION("throws a configuration_error in case of bad Results Directory") {
        SECTION("directory does not exist") {
            ao.results_dir = (CONFIG_PATH / "does" / "not" / "exist").string();
        }

        SECTION("is a regular file") {
            ao.results_dir = (CONFIG_PATH / "bad.json").string();
        }

        REQUIRE_THROWS_AS(configuration::validate_application_options(ao),
                          configuration_error);
    }

    SECTION("validates succesfully good options") {
        REQUIRE_NOTHROW(configuration::validate_application_options(ao));
    }
}

}  // namespace pcp_test
