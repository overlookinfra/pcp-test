#include <pcp-test/configuration.hpp>
#include <pcp-test/pcp-test.hpp>
#include <pcp-test/errors.hpp>
#include <pcp-test/schemas.hpp>
#include <pcp-test/test_connection_parameters.hpp>

#include <pcp-test/root_path.h>

#include <cpp-pcp-client/util/logging.hpp>
#include <cpp-pcp-client/validator/validator.hpp>

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
#include <vector>
#include <set>

namespace pcp_test {
namespace configuration {

namespace lth_log  = leatherman::logging;
namespace lth_file = leatherman::file_util;
namespace lth_loc  = leatherman::locale;
namespace lth_jc   = leatherman::json_container;
namespace po       = boost::program_options;
namespace fs       = boost::filesystem;
namespace conn_par = pcp_test::connection_test_parameters;

const std::string DEFAULT_CONFIGFILE  {"/etc/puppetlabs/pcp-test/pcp-test.conf"};
const std::string DEFAULT_LOGFILE     {"/var/log/puppetlabs/pcp-test/pcp-test.log"};
const std::string DEFAULT_RESULTS_DIR {"/opt/puppetlabs/pcp-test/results"};
const std::string DEFAULT_CERTIFICATES_DIR {
        (fs::path(PCP_TEST_ROOT_PATH) / "dev-resources" / "pcp-certificates").string()};

const std::string DEFAULT_LOGLEVEL_TEXT {"info"};
const std::string DEFAULT_CLIENT_LOGLEVEL_TEXT {"warning"};
const std::string DEFAULT_BROKER_WS_URI {"wss://localhost:8142/pcp/"};

const std::string URL_REGEX             {"^wss:\\/\\/\\S+:\\d+\\/\\S+"};
const std::string AGENT_CERT_REGEX      {"(\\d+)agent.example.com_.*"};
const std::string CONTROLLER_CERT_REGEX {"(\\d+)controller.example.com_.*"};

//
// utility functions
//

std::set<std::string> get_endpoint_names(const std::vector<std::string>& names,
                                         std::string role,
                                         std::string endpoint_regex,
                                         int num_endpoints) {
    boost::regex re { endpoint_regex };
    boost::cmatch matches;
    std::set<std::string> unique_names;
    auto num_names = static_cast<unsigned int>(num_endpoints);  // NOLINT(runtime/int)

    for (const auto& name : names) {
        if (boost::regex_match(name.c_str(), matches, re)) {
            unique_names.insert(
                    std::string(matches[1].first, matches[1].second) + role);

            if (unique_names.size() >= num_names)
                break;
        }
    }

    return unique_names;
}

std::set<std::string> get_agent_names(const std::vector<std::string>& names,
                                      int num_agents) {
    return get_endpoint_names(names, "agent", AGENT_CERT_REGEX, num_agents);
}

std::set<std::string> get_controller_names(const std::vector<std::string>& names,
                                           int num_controllers) {
    return get_endpoint_names(names, "controller", CONTROLLER_CERT_REGEX,
                              num_controllers);
}

bool has_cert_suffix(const std::string& s)
{
    static const std::string suffix {"_crt.pem"};
    static auto suffix_size = suffix.size();

    return (s.size() > suffix_size
            && s.compare(s.size() - suffix_size, suffix_size, suffix) == 0);
}

std::vector<std::string> get_crt_files_from_dir(fs::path dir_path) {
    std::vector<std::string> names {};
    fs::directory_iterator end_iter {};
    fs::directory_iterator dir_iter {dir_path};

    for (; dir_iter != end_iter; ++dir_iter) {
        if (fs::is_regular_file(dir_iter->status())) {
            auto name = dir_iter->path().filename().string();
            if (has_cert_suffix(name))
                names.push_back(std::move(name));
        }
    }

    return names;
}

//
// configuration functions
//

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
        "==========\n"
        "\n"
        "  trivial    - just a proof of concept\n"
        "  connection - determines how many PCP connections the broker can handle\n"
        "\n"
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

void setup_logging(const std::string& logfile_,
                   const std::string& loglevel_,
                   const std::string& client_loglevel_)
{
    assert(!logfile_.empty());
    auto lvl = option_to_log_level.at(loglevel_);
    auto client_lvl = option_to_log_level.at(client_loglevel_);
    std::ostream* o_stream {nullptr};
    f_stream.open(logfile_.c_str(), std::ios_base::app);
    o_stream = &f_stream;
    PCPClient::Util::setupLogging(*o_stream, true, client_lvl);
    lth_log::setup_logging(*o_stream);
    lth_log::set_colorization(true);
    lth_log::set_level(lvl);
    LOG_DEBUG("Logging configured");
}

application_options get_application_options(int argc, char** argv)
{
    application_options a_o {};
    po::options_description visible_options {"visible command line options"};
    visible_options.add_options()
            ("help,h", "produce help message")
            ("version,v", "print the version and exit")
            ("loglevel",
             po::value<std::string>()->default_value(""),
             "pcp-test log level (none, trace, debug, info, warning, error, fatal)")
            ("client-loglevel",
             po::value<std::string>()->default_value(""),
             "client lib log level (none, trace, debug, info, warning, error, fatal)")
            ("logfile",
             po::value<std::string>()->default_value(""),
             "log file")
            ("config-file",
             po::value<std::string>()->default_value(DEFAULT_CONFIGFILE),
             "mandatory configuration file")
            ("broker-ws-uris,u",
             po::value<std::vector<std::string>>()->multitoken(),
             "PCP broker WebSocket URIs")
            ("certificates-dir",
             po::value<std::string>()->default_value(DEFAULT_CERTIFICATES_DIR),
             "SSL certificates path (see doc for the expected directory tree "
             "structure)")
            ("results-dir",
             po::value<std::string>()->default_value(DEFAULT_RESULTS_DIR),
             "results directory");

    po::positional_options_description positional_options {};
    positional_options.add("test", 1);

    // To avoid listing the <test_type> argument as a named option
    po::options_description hidden_options {"hidden options"};
    hidden_options.add_options()
            ("test",
             po::value<std::string>()->default_value("none"),
             "test type to be executed");

    // Merge the above groups
    po::options_description c_l_options {"all command line options"};
    c_l_options.add(visible_options)
               .add(hidden_options);

    po::variables_map vm{};

    try {
        po::store(
                po::command_line_parser(argc, argv)
                        .options(c_l_options)
                        .positional(positional_options)
                        .run(),
                vm);

        if (vm.count("help")) {
            help(visible_options);
            exit(EXIT_SUCCESS);
        }

        po::notify(vm);
    } catch (const std::exception &e) {
        throw configuration_error(e.what());
    }

    if (vm.count("version")) {
        boost::nowide::cout << pcp_test::version() << std::endl;
        exit(EXIT_SUCCESS);
    }

    a_o.logfile         = vm["logfile"].as<std::string>();
    a_o.loglevel        = vm["loglevel"].as<std::string>();
    a_o.client_loglevel = vm["client-loglevel"].as<std::string>();
    a_o.test            = vm["test"].as<std::string>();
    a_o.configfile      = vm["config-file"].as<std::string>();

    if (vm.count("broker-ws-uris")) {
        a_o.broker_ws_uris = vm["broker-ws-uris"].as<std::vector<std::string>>();
    } else {
        a_o.broker_ws_uris = std::vector<std::string>();
    }

    a_o.certificates_dir = vm["certificates-dir"].as<std::string>();
    a_o.results_dir      = vm["results-dir"].as<std::string>();

    return a_o;
}

void validate_test_type(application_options& a_o)
{
    auto t_type_itr = to_test_type.find(a_o.test);
    if (t_type_itr ==  to_test_type.end()) {
        throw configuration_error((boost::format("unknown test type (%1%)")
                                   % a_o.test).str());
    } else if (t_type_itr->second == test_type::none) {
        throw configuration_error("the test type argument must be specified");
    }
}

void parse_configfile_and_process_options(application_options& a_o) {
    if (a_o.configfile.empty())
        throw configuration_error("configuration file must be specified");

    a_o.configfile = lth_file::tilde_expand(a_o.configfile);

    // NOTE(ale): configfile is mandatory!
    if (!fs::exists(a_o.configfile) || !fs::is_regular_file(a_o.configfile))
        throw configuration_error("configuration file does not exist");

    if (!lth_file::file_readable(a_o.configfile))
        throw configuration_error("cannot read the configuration file");

    lth_jc::JsonContainer config_json;

    try {
        config_json = lth_jc::JsonContainer(lth_file::read(a_o.configfile));
    } catch (lth_jc::data_parse_error &e) {
        throw configuration_error("failed to parse the configuration file; "
                                  "invalid JSON");
    }

    if (config_json.type() != lth_jc::DataType::Object)
        throw configuration_error("invalid configuration file; root element "
                                  "is not a JSON object");

    for (const auto &key : config_json.keys())
        if (!application_options::is_configuration_file_option(key))
            throw configuration_error((boost::format("unknown option (%1%)")
                                      % key).str());

    // Process options that can be specified both on CL and configfile

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

    if (a_o.client_loglevel.empty()) {
        if (config_json.includes("client-loglevel")) {
            a_o.client_loglevel = config_json.get<std::string>("client-loglevel");
        } else {
            a_o.client_loglevel = DEFAULT_CLIENT_LOGLEVEL_TEXT;
        }
    }

    if (a_o.broker_ws_uris.empty()) {
        if (config_json.includes("broker-ws-uris")) {
            a_o.broker_ws_uris =
                    config_json.get<std::vector<std::string>>("broker-ws-uris");
        } else {
            a_o.broker_ws_uris = std::vector<std::string> {DEFAULT_BROKER_WS_URI};
        }
    }

    a_o.certificates_dir = lth_file::tilde_expand(a_o.certificates_dir);
    a_o.results_dir      = lth_file::tilde_expand(a_o.results_dir);

    // Process configfile-only options

    if (config_json.includes("connection-test-parameters")) {
        try {
            a_o.connection_test_parameters =
                config_json.get<lth_jc::JsonContainer>(
                        "connection-test-parameters");
        } catch (const lth_jc::data_error& e) {
            throw configuration_error((boost::format(
                                           "invalid configuration file (%1%)")
                                       % e.what()).str());
        }
    }
}

void validate_application_options(application_options& a_o)
{
    // log file
    auto log_file_parent_dir = (fs::path {a_o.logfile}).parent_path();
    if (!fs::exists(log_file_parent_dir))
        throw configuration_error("log directory does not exist");

    // log level

    if (option_to_log_level.find(a_o.loglevel) == option_to_log_level.end())
        throw configuration_error((boost::format("invalid log level (%1%)")
                                   % a_o.loglevel).str());

    if (option_to_log_level.find(a_o.client_loglevel) == option_to_log_level.end())
        throw configuration_error((boost::format("invalid client lib log level (%1%)")
                                   % a_o.client_loglevel).str());

    // broker WebSocket URIs
    boost::regex uri_re {URL_REGEX};
    for (auto uri : a_o.broker_ws_uris)
        if (!boost::regex_match(uri, uri_re))
            throw configuration_error((boost::format("invalid WebSocket URI (%1%)")
                                       % uri).str());

    // SSL certificates directory

    fs::path cert_root_dir {a_o.certificates_dir};
    auto cert_test_dir = cert_root_dir / "test";

    if (!fs::exists(cert_root_dir) || !fs::exists(cert_test_dir))
        throw configuration_error("invalid certificate directory");

    if (!fs::exists(cert_root_dir / "ca_crt.pem"))
        throw configuration_error("CA certificate does not exist");

    // results directory
    if (!fs::exists(a_o.results_dir) || !fs::is_directory(a_o.results_dir))
        throw configuration_error((boost::format("invalid results directory '%1%'")
                                   % a_o.results_dir).str());

    // test parameters

    PCPClient::Validator parameters_validator {};

    if (!a_o.connection_test_parameters.empty()) {
        parameters_validator.registerSchema(schemas::connection_test_parameters());

        try {
            parameters_validator.validate(a_o.connection_test_parameters,
                                          schemas::CONNECTION_TEST_PARAMETERS);
        } catch (const PCPClient::validation_error& e) {
            throw configuration_error((boost::format("invalid connection test "
                                                     "parameters (%1%)")
                                       % e.what()).str());
        }
    } else if (to_test_type.at(a_o.test) == test_type::connection) {
        throw configuration_error("connection test settings are missing in "
                                  "the configuration file");
    }

    // client common names

    if (to_test_type.at(a_o.test) == test_type::connection) {
        // We need certs...
        const auto& p = a_o.connection_test_parameters;
        auto max_num_clients_per_task =
            p.get<int>(conn_par::NUM_ENDPOINTS)
            + (p.get<int>(conn_par::NUM_RUNS) * p.get<int>(conn_par::ENDPOINTS_INCREMENT));
        auto max_concurrency =
            p.get<int>(conn_par::CONCURRENCY)
            + (p.get<int>(conn_par::NUM_RUNS) * p.get<int>(conn_par::CONCURRENCY_INCREMENT));
        auto num_clients = max_num_clients_per_task * max_concurrency;

        auto file_names = get_crt_files_from_dir(cert_test_dir);
        a_o.agents = get_agent_names(file_names, num_clients);

        auto num_a_certs = static_cast<int>(a_o.agents.size());
        if (num_a_certs < num_clients) {
            a_o.controllers = get_controller_names(file_names,
                                                   num_clients - num_a_certs);

            auto num_c_certs = static_cast<int>(a_o.controllers.size());
            if (num_a_certs + num_c_certs < num_clients)
                throw configuration_error(
                    (boost::format("%1% certificates requested, but only %2% "
                                   "are available")
                     % num_clients % (num_a_certs + num_c_certs)).str());
        }
    }
}

}  // namespace configuration
}  // namespace pcp_test
