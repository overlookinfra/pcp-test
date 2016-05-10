#include <pcp-test/configuration.hpp>
#include <pcp-test/test_trivial.hpp>
#include <pcp-test/pcp-test.hpp>
#include <pcp-test/errors.hpp>

#include <leatherman/logging/logging.hpp>

#include <boost/nowide/args.hpp>
#include <boost/nowide/fstream.hpp>
#include <boost/nowide/iostream.hpp>

#include <cassert>
#include <stdexcept>

namespace pcp_test {

namespace lth_log = leatherman::logging;

void start(const application_options& a_o)
{
    assert(to_test_type.find(a_o.test)->second == test_type::trivial);
    run_trivial_test();
}

int main(int argc, char **argv)
{
    // Fix args on Windows to be UTF-8
    boost::nowide::args arg_utf8 {argc, argv};
    application_options a_o {};

    try {
        a_o = configuration::get_application_options(argc, argv);
        configuration::validate_test_type(a_o);
        configuration::parse_configfile_and_process_options(a_o);
        configuration::validate_application_options(a_o);
    } catch (const configuration_error& e) {
        lth_log::colorize(boost::nowide::cerr, lth_log::log_level::error);
        boost::nowide::cerr << "Configuration error: "
                            << e.what() << "\n" << std::endl;
        lth_log::colorize(boost::nowide::cerr);
        exit(EXIT_FAILURE);
    }
    configuration::setup_logging(a_o.logfile, a_o.loglevel);

    try {
        start(a_o);
    } catch (const fatal_error& e) {
        LOG_ERROR("Fatal error: %1%", e.what());
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        LOG_ERROR("Unexpected failure: %1%", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

}  // namespace pcp_test

int main(int argc, char** argv)
{
    return pcp_test::main(argc, argv);
}
