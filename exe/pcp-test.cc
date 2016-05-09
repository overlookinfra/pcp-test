#include <pcp-test/pcp-test.hpp>
#include <pcp-test/errors.hpp>
#include <pcp-test/test_trivial.hpp>
#include <pcp-test/application_options.hpp>

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

application_options get_application_options(int argc, char** argv)
{
    application_options a_o {};
    po::options_description c_l_options {""};

    try {
        c_l_options.add_options()
            ("help,h", "produce help message")
            ("debug",
             po::bool_switch(&a_o.debug)->default_value(false),
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
                po::command_line_parser(argc, argv)
                    .options(c_l_options)
                    .positional(c_l_positional_options)
                    .run(),
                vm);

            if (vm.count("help")) {
                help(c_l_options);
                exit(EXIT_SUCCESS);
            }

            po::notify(vm);
        } catch (const std::exception &ex) {
            lth_log::colorize(boost::nowide::cerr, lth_log::log_level::error);
            boost::nowide::cerr << "error: " << ex.what() << "\n" << std::endl;
            lth_log::colorize(boost::nowide::cerr);
            help(c_l_options);
            exit(EXIT_FAILURE);
        }

        if (vm.count("version")) {
            boost::nowide::cout << pcp_test::version() << std::endl;
            exit(EXIT_SUCCESS);
        }

        a_o.logfile = vm["logfile"].as<std::string>();
        a_o.test    = vm["test"].as<std::string>();

    } catch (const std::exception &ex) {
        colorize(boost::nowide::cerr, lth_log::log_level::fatal);
        boost::nowide::cerr << "unhandled exception: " << ex.what()
        << "\n" << std::endl;
        lth_log::colorize(boost::nowide::cerr);
        exit(EXIT_FAILURE);
    }

    return a_o;
}

void process_and_validate_application_options(application_options& a_o)
{
    auto t_type_itr = to_test_type.find(a_o.test);
    std::string err_msg {};

    if (t_type_itr ==  to_test_type.end()) {
        err_msg = "Unknown test type: ";
        err_msg += a_o.test;
    } else if (t_type_itr->second == test_type::none) {
        err_msg =  "The test type argument must be specified";
    }

    if (!err_msg.empty()) {
        lth_log::colorize(boost::nowide::cerr, lth_log::log_level::error);
        boost::nowide::cerr << err_msg << "\n" << std::endl;
        lth_log::colorize(boost::nowide::cerr);
        lth_log::colorize(boost::nowide::cerr);
        exit(EXIT_FAILURE);
    }
}

void start(const application_options& a_o)
{
    assert(to_test_type.find(a_o.test)->second == test_type::trivial);
    run_trivial_test();
}

int main(int argc, char **argv)
{
    // Fix args on Windows to be UTF-8
    boost::nowide::args arg_utf8 {argc, argv};
    auto a_o = get_application_options(argc, argv);
    process_and_validate_application_options(a_o);
    setup_logging(a_o.logfile, a_o.debug);

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
