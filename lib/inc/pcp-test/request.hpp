/**
 * @file
 * Abstraction for a PCP Test request message.
 */

#pragma once

#include <cpp-pcp-client/protocol/chunks.hpp>      // ParsedChunk

#include <leatherman/json_container/json_container.hpp>

#include <stdexcept>
#include <string>
#include <map>

namespace pcp_test {

class request {
  public:
    struct error : public std::runtime_error {
        explicit error(std::string const& msg) : std::runtime_error(msg) {}
    };

    /// Throws an request::error in case it fails to retrieve
    /// the data chunk from the specified ParsedChunks or in case
    /// of binary data (currently not supported).
    request(const PCPClient::ParsedChunks& parsed_chunks_);
    request(PCPClient::ParsedChunks&& parsed_chunks_);

    const std::string& id() const;
    const std::string& message_type() const;
    const std::string& sender() const;
    const std::string& transaction() const;
    const std::string& timestamp();
    const PCPClient::ParsedChunks& parsedChunks() const;

  private:
    // envelope
    std::string id_;
    std::string message_type_;
    std::string sender_;

    // data
    std::string transaction_;
    std::string timestamp_;

    // parsed chunks
    PCPClient::ParsedChunks parsed_chunks_;

    void init();
    void validateFormat();
};

}  // namespace pcp_test
