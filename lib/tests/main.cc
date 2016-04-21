#define CATCH_CONFIG_RUNNER

#include <catch.hpp>

// To enable log messages:
// #define ENABLE_LOGGING

#ifdef ENABLE_LOGGING
#include <boost/nowide/iostream.hpp>
#define LEATHERMAN_LOGGING_NAMESPACE "puppetlabs.pcp-test.test"
#include <leatherman/logging/logging.hpp>
#endif

int main(int argc, char** argv) {
#ifdef ENABLE_LOGGING
    leatherman::logging::setup_logging(boost::nowide::cout);
    leatherman::logging::set_level(leatherman::logging::log_level::debug);
#endif
    Catch::Session test_session {};
    test_session.configData().showDurations = Catch::ShowDurations::Always;
    return test_session.run(argc, argv);
}
