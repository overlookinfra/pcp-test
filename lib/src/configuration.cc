#include <pcp-test/configuration.hpp>
#include <pcp-test/pcp-test.hpp>

#include <cpp-pcp-client/util/logging.hpp>

#include <leatherman/logging/logging.hpp>
#include <leatherman/file_util/file.hpp>
#include <leatherman/json_container/json_container.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <boost/nowide/fstream.hpp>
#include <boost/nowide/iostream.hpp>

#include <boost/format.hpp>

// boost includes are not always warning-clean. Disable warnings that
// cause problems before including the headers, then re-enable the warnings.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#include <boost/program_options.hpp>
#pragma GCC diagnostic pop

#include <map>

namespace pcp_test {
namespace configuration {

namespace lth_log  = leatherman::logging;
namespace lth_file = leatherman::file_util;
namespace lth_loc  = leatherman::locale;
namespace lth_jc   = leatherman::json_container;
namespace po       = boost::program_options;
namespace fs       = boost::filesystem;

const std::string DEFAULT_CONFIGFILE {"/etc/puppetlabs/pcp-test/pcp-test.conf"};
const std::string DEFAULT_LOGFILE {"/var/log/puppetlabs/pcp-test/pcp-test.log"};
const std::string DEFAULT_LOGLEVEL_TEXT {"info"};

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
            "Displays its own version string.\n" << std::endl;
}

static boost::nowide::ofstream f_stream {};

static const std::map<std::string, lth_log::log_level> option_to_log_level {
        { "none",    lth_log::log_level::none },
        { "trace",   lth_log::log_level::trace },
        { "debug",   lth_log::log_level::debug },
        { "info",    lth_log::log_level::info },
        { "warning", lth_log::log_level::warning },
        { "error",   lth_log::log_level::error },
        { "fatal",   lth_log::log_level::fatal }};

void setup_logging(const std::string& logfile_, const std::string& loglevel_)
{
    assert(!logfile_.empty());
    auto lvl = option_to_log_level.at(loglevel_);
    std::ostream* o_stream {nullptr};
    f_stream.open(logfile_.c_str(), std::ios_base::app);
    o_stream = &f_stream;
    PCPClient::Util::setupLogging(*o_stream, true, lvl);
    lth_log::setup_logging(*o_stream);
    lth_log::set_colorization(true);
    lth_log::set_level(lvl);
    LOG_DEBUG("Logging configured");
}

void print_error(const std::string& err_msg)
{
    lth_log::colorize(boost::nowide::cerr, lth_log::log_level::error);
    boost::nowide::cerr << "Error: " << err_msg << "\n" << std::endl;
    lth_log::colorize(boost::nowide::cerr);
}

void print_error_and_exit(const std::string& err_msg)
{
    print_error(err_msg);
    exit(EXIT_FAILURE);
}

application_options get_application_options(int argc, char** argv)
{
    application_options a_o {};
    po::options_description c_l_options {""};

    try {
        c_l_options.add_options()
                ("help,h", "produce help message")
                ("loglevel",
                 po::value<std::string>()->default_value(""),
                 "log level (none, trace, debug, info, warning, error, fatal)")
                ("logfile",
                 po::value<std::string>()->default_value(""),
                 "log file")
                ("config-file",
                 po::value<std::string>()->default_value(""),
                 "configuration file")
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
            print_error(ex.what());
            help(c_l_options);
            exit(EXIT_FAILURE);
        }

        if (vm.count("version")) {
            boost::nowide::cout << pcp_test::version() << std::endl;
            exit(EXIT_SUCCESS);
        }

        a_o.logfile    = vm["logfile"].as<std::string>();
        a_o.loglevel   = vm["loglevel"].as<std::string>();
        a_o.test       = vm["test"].as<std::string>();
        a_o.configfile = vm["config-file"].as<std::string>();
    } catch (const std::exception &ex) {
        print_error_and_exit(ex.what());
    }

    return a_o;
}

void validate_test_type(application_options& a_o)
{
    auto t_type_itr = to_test_type.find(a_o.test);
    if (t_type_itr ==  to_test_type.end()) {
        print_error_and_exit((boost::format("unknown test type (%1%)")
                              % a_o.test).str());
    } else if (t_type_itr->second == test_type::none) {
        print_error_and_exit("the test type argument must be specified");
    }
}

void parse_configfile_and_process_options(application_options& a_o) {
    if (a_o.configfile.empty())
        a_o.configfile = DEFAULT_CONFIGFILE;

    a_o.configfile = lth_file::tilde_expand(a_o.configfile);

    if (!fs::exists(a_o.configfile) || !fs::is_regular_file(a_o.configfile))
        print_error_and_exit("configuration file does not exist");

    if (!lth_file::file_readable(a_o.configfile))
        print_error_and_exit("cannot read the configuration file");

    lth_jc::JsonContainer config_json;

    try {
        config_json = lth_jc::JsonContainer(lth_file::read(a_o.configfile));
    } catch (lth_jc::data_parse_error &e) {
        print_error_and_exit("failed to parse the configuration file; "
                             "invalid JSON");
    }

    if (config_json.type() != lth_jc::DataType::Object)
        print_error_and_exit("invalid configuration file; root element is not "
                             "a JSON object");

    for (const auto &key: config_json.keys())
        if (!application_options::exists(key))
            print_error_and_exit((boost::format("unknown option (%1%)")
                                  % key).str());

    if (a_o.logfile.empty()) {
        if (config_json.includes("logfile")) {
            a_o.logfile = config_json.get<std::string>("logfile");
        } else {
            a_o.logfile = DEFAULT_LOGFILE;
        }
    }

    a_o.logfile = lth_file::tilde_expand(a_o.logfile);

    if (a_o.loglevel.empty()) {
        if (config_json.includes("loglevel")) {
            a_o.loglevel = config_json.get<std::string>("loglevel");
        } else {
            a_o.loglevel = DEFAULT_LOGLEVEL_TEXT;
        }
    }
}

void validate_application_options(application_options& a_o)
{
    auto log_file_parent_dir = (fs::path {a_o.logfile}).parent_path();
    if (!fs::exists(log_file_parent_dir))
        print_error_and_exit("log directory does not exist");

    if (option_to_log_level.find(a_o.loglevel) == option_to_log_level.end())
        print_error_and_exit((boost::format("invalid log level (%1%)")
                              % a_o.loglevel).str());
}

}  // namespace configuration
}  // namespace pcp_test
