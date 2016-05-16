#include <pcp-test/schemas.hpp>

namespace pcp_test {
namespace schemas {

using C_Type = PCPClient::ContentType;
using T_Constraint = PCPClient::TypeConstraint;

const std::string REQUEST_TYPE {"pcp-test-request"};
const std::string RESPONSE_TYPE {"pcp-test-response"};
const std::string ERROR_TYPE {"pcp-test-error"};

PCPClient::Schema request() {
    PCPClient::Schema schema {REQUEST_TYPE, C_Type::Json};

    // Request-response transaction UUID
    schema.addConstraint("transaction", T_Constraint::String, true);

    // Indicates the time instant of message creation
    schema.addConstraint("timestamp", T_Constraint::String, true);
    return schema;
}

PCPClient::Schema response() {
    PCPClient::Schema schema {RESPONSE_TYPE, C_Type::Json};

    // Request-response transaction UUID
    schema.addConstraint("transaction", T_Constraint::String, true);

    // Indicates the time instant of message creation
    schema.addConstraint("timestamp", T_Constraint::String, true);
    return schema;
}

PCPClient::Schema error() {
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

}  // namespace schemas
}  // namespace pcp-test
