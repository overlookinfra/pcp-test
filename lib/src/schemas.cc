#include <pcp-test/schemas.hpp>
#include <pcp-test/test_connection_parameters.hpp>
#include <pcp-test/configuration_parameters.hpp>

namespace pcp_test {
namespace schemas {

using C_Type       = PCPClient::ContentType;
using T_Constraint = PCPClient::TypeConstraint;
namespace conn_par = pcp_test::connection_test_parameters;

const std::string REQUEST_TYPE {"pcp-test-request"};
const std::string RESPONSE_TYPE {"pcp-test-response"};
const std::string ERROR_TYPE {"pcp-test-error"};

PCPClient::Schema request()
{
    PCPClient::Schema schema {REQUEST_TYPE, C_Type::Json};

    // Request-response transaction UUID
    schema.addConstraint("transaction", T_Constraint::String, true);

    // Indicates the time instant of message creation
    schema.addConstraint("timestamp", T_Constraint::String, true);
    return schema;
}

PCPClient::Schema response()
{
    PCPClient::Schema schema {RESPONSE_TYPE, C_Type::Json};

    // Request-response transaction UUID
    schema.addConstraint("transaction", T_Constraint::String, true);

    // Indicates the time instant of message creation
    schema.addConstraint("timestamp", T_Constraint::String, true);
    return schema;
}

PCPClient::Schema error()
{
    PCPClient::Schema schema {ERROR_TYPE, C_Type::Json};

    // The UUID of the request-response transaction that
    // generated the error
    schema.addConstraint("transaction", T_Constraint::String, true);

    // Indicates the time instant of message creation
    schema.addConstraint("timestamp", T_Constraint::String, true);

    // Error message
    schema.addConstraint("error_msg", T_Constraint::String, true);

    return schema;
}

PCPClient::Schema connection_test_parameters()
{
    PCPClient::Schema schema {configuration_parameters::CONNECTION_TEST_PARAMETERS,
                              C_Type::Json};

    schema.addConstraint(conn_par::NUM_RUNS,                       T_Constraint::Int,  true);
    schema.addConstraint(conn_par::INTER_RUN_PAUSE_MS,             T_Constraint::Int,  true);
    schema.addConstraint(conn_par::NUM_ENDPOINTS,                  T_Constraint::Int,  true);
    schema.addConstraint(conn_par::INTER_ENDPOINT_PAUSE_MS,        T_Constraint::Int,  true);
    schema.addConstraint(conn_par::CONCURRENCY,                    T_Constraint::Int,  true);
    schema.addConstraint(conn_par::ENDPOINTS_INCREMENT,            T_Constraint::Int,  true);
    schema.addConstraint(conn_par::CONCURRENCY_INCREMENT,          T_Constraint::Int,  true);
    schema.addConstraint(conn_par::RANDOMIZE_INTER_ENDPOINT_PAUSE, T_Constraint::Bool, false);
    schema.addConstraint(conn_par::INTER_ENDPOINT_PAUSE_RNG_SEED,  T_Constraint::Int,  false);
    schema.addConstraint(conn_par::WS_CONNECTION_TIMEOUT_MS,       T_Constraint::Int,  false);
    schema.addConstraint(conn_par::WS_CONNECTION_CHECK_INTERVAL_S, T_Constraint::Int,  false);
    schema.addConstraint(conn_par::ASSOCIATION_TIMEOUT_S,          T_Constraint::Int,  false);
    schema.addConstraint(conn_par::ASSOCIATION_REQUEST_TTL_S,      T_Constraint::Int,  false);
    schema.addConstraint(conn_par::PERSIST_CONNECTIONS,            T_Constraint::Bool, false);
    schema.addConstraint(conn_par::SHOW_STATS,                     T_Constraint::Bool, false);

    return schema;
}

}  // namespace schemas
}  // namespace pcp-test
