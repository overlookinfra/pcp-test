/**
 * @file
 * Abstraction for a PCP Test message.
 */

#pragma once

#include <cpp-pcp-client/protocol/chunks.hpp>

#include <leatherman/json_container/json_container.hpp>

#include <stdexcept>
#include <string>
#include <map>

namespace pcp_test {

class message {
  public:
    struct error : public std::runtime_error {
        explicit error(std::string const& msg) : std::runtime_error(msg) {}
    };

    /// Instantiate a message out of a parsed message.
    /// Throws an message::error in case it fails to retrieve
    /// the data chunk from the specified ParsedChunks or in case
    /// of binary data (currently not supported).
    message(const PCPClient::ParsedChunks& parsed_chunks);

    /// Instantiate a request of given type.
    message(const std::string& message_type,
            const std::string& sender,
            const std::string& transaction,
            std::string error = "");

    // Envelope
    const std::string& id() const;
    const std::string& message_type() const;
    const std::string& sender() const;

    // Data
    const std::string& transaction() const;
    const std::string& timestamp() const;
    const std::string& error_msg() const;
    leatherman::json_container::JsonContainer get_data() const;

  private:
    // envelope
    std::string id_;
    std::string message_type_;
    std::string sender_;

    // data
    std::string transaction_;
    std::string timestamp_;
    std::string error_msg_;

    void init(const PCPClient::ParsedChunks &parsed_chunks);
    void validateFormat(const PCPClient::ParsedChunks &parsed_chunks);
};

}  // namespace pcp_test
