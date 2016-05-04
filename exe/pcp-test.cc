#include <pcp-test/pcp-test.hpp>
#include <pcp-test/errors.hpp>
#include <pcp-test/test_trivial.hpp>

#include <cpp-pcp-client/util/logging.hpp>

#include <leatherman/logging/logging.hpp>
#include <leatherman/file_util/file.hpp>

#include <boost/nowide/fstream.hpp>
#include <boost/nowide/iostream.hpp>
#include <boost/nowide/args.hpp>

// boost includes are not always warning-clean. Disable warnings that
// cause problems before including the headers, then re-enable the warnings.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#include <boost/program_options.hpp>
#pragma GCC diagnostic pop

namespace pcp_test {

namespace lth_log  = leatherman::logging;
namespace lth_file = leatherman::file_util;
namespace lth_loc  = leatherman::locale;
namespace po       = boost::program_options;

const std::string DEFAULT_CONFIGURATION_PATH {""};

void help(po::options_description &desc)
{
    boost::nowide::cout <<
    "pcp-test\n"
            "========\n"
            "\n"
            "A framework for testing the pcp-broker.\n"
            "\n"
            "Usage\n"
            "=====\n"
            "\n"
            "  pcp-test <test-type> [options]\n"
            "\n"
            "Test types\n"
            "==========\n\n"
            "  'trivial': still spiking!\n\n"
            "Options\n"
            "=======\n\n" << desc <<
            "\nDescription\n"
            "===========\n"
            "\n"
            "Displays its own version string." << std::endl;
}

static boost::nowide::ofstream f_stream {};

void setup_logging(const std::string& logfile_, bool debug_)
{
    lth_log::log_level lvl = lth_log::log_level::info;
    if (debug_)
        lvl = lth_log::log_level::trace;

    std::ostream* o_stream {nullptr};

    if (!logfile_.empty()) {
        auto logfile = lth_file::tilde_expand(logfile_);
        f_stream.open(logfile.c_str(), std::ios_base::app);
        o_stream = &f_stream;
    } else {
        o_stream = &boost::nowide::cout;
    }

    PCPClient::Util::setupLogging(*o_stream, true, lvl);
    lth_log::setup_logging(*o_stream);
    lth_log::set_colorization(true);
    lth_log::set_level(lvl);
    LOG_DEBUG("Logging configured");
}

int main(int argc, char **argv)
{
    bool debug {false};
    std::string logfile {};
    std::string t_test {};

    try {
        // Fix args on Windows to be UTF-8
        boost::nowide::args arg_utf8 {argc, argv};

        // Configure command line options

        po::options_description c_l_options {""};
        c_l_options.add_options()
                ("help,h", "produce help message")
                ("debug",
                 po::bool_switch(&debug)->default_value(false),
                 "Set logging level to debug")
                ("logfile",
                 po::value<std::string>()->default_value("/tmp/pcp-test.log"),
                 "log file")
                ("version,v", "print the version and exit")
                ("test",
                 po::value<std::string>()->default_value("none"),
                 "test type to be executed");

        po::positional_options_description c_l_positional_options {};
        c_l_positional_options.add("test", 1);

        po::variables_map vm{};

        try {
            po::store(
                po::command_line_parser(argc, argv).
                    options(c_l_options).
                    positional(c_l_positional_options).
                    run(),
                vm);

            if (vm.count("help")) {
                help(c_l_options);
                return EXIT_SUCCESS;
            }

            po::notify(vm);
        } catch (const std::exception &ex) {
            lth_log::colorize(boost::nowide::cerr, lth_log::log_level::error);
            boost::nowide::cerr << "error: " << ex.what() << "\n" << std::endl;
            lth_log::colorize(boost::nowide::cerr);
            help(c_l_options);
            return EXIT_FAILURE;
        }

        if (vm.count("version")) {
            boost::nowide::cout << pcp_test::version() << std::endl;
            return EXIT_SUCCESS;
        }

        debug   = vm["debug"].as<bool>();
        logfile = vm["logfile"].as<std::string>();
        t_test  = vm["test"].as<std::string>();
        auto t_type_itr = to_test_type.find(t_test);
        std::string err_msg {};

        if (t_type_itr ==  to_test_type.end()) {
            err_msg = "Unknown test type: ";
            err_msg += t_test;
        } else if (t_type_itr->second == test_type::none) {
            err_msg =  "The test type argument must be specified";
        }

        if (!err_msg.empty()) {
            lth_log::colorize(boost::nowide::cerr, lth_log::log_level::error);
            boost::nowide::cerr << err_msg << "\n" << std::endl;
            lth_log::colorize(boost::nowide::cerr);
            lth_log::colorize(boost::nowide::cerr);
            return EXIT_FAILURE;
        }

        setup_logging(logfile, debug);
    } catch (const std::exception &ex) {
        colorize(boost::nowide::cerr, lth_log::log_level::fatal);
        boost::nowide::cerr << "unhandled exception: " << ex.what()
                            << "\n" << std::endl;
        lth_log::colorize(boost::nowide::cerr);
        return EXIT_FAILURE;
    }

    try {
        run_trivial_test();
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
