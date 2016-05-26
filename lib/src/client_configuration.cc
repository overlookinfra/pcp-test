#include <pcp-test/client_configuration.hpp>

#include <pcp-test/root_path.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <boost/format.hpp>

#include <utility>

namespace pcp_test {

namespace fs = boost::filesystem;

const long DEFAULT_CONNECTION_TIMEOUT_MS {100};
const unsigned int DEFAULT_MESSAGE_TIMEOUT_S {5};

static boost::format CERT_FILE_FORMAT {"%1%.example.com_%2%.pem"};

client_configuration::client_configuration(
        std::string common_name_,
        const std::string& client_type_,
        const std::vector <std::string>& broker_ws_uris_,
        const std::string& certificates_dir_,
        long connection_tmeout_ms_,
        unsigned int message_timeout_s_)
    : common_name {std::move(common_name_)},
      client_type (client_type_),
      broker_ws_uris (broker_ws_uris_),
      certificates_dir (certificates_dir_),
      connection_timeout_ms {std::move(connection_tmeout_ms_)},
      message_timeout_s {std::move(message_timeout_s_)}
{
    update_cert_paths();
}

void client_configuration::update_cert_paths()
{
    fs::path c_p {certificates_dir};
    ca  = (c_p / "ca_crt.pem").string();
    crt = (c_p / "test" / (CERT_FILE_FORMAT % common_name % "crt").str()).string();
    key = (c_p / "test" / (CERT_FILE_FORMAT % common_name % "key").str()).string();
}

}  // namespace pcp_test
