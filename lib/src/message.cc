#include <pcp-test/message.hpp>
#include <pcp-test/schemas.hpp>
#include <pcp-test/util.hpp>

#include <leatherman/util/time.hpp>

namespace pcp_test {

namespace lth_util = leatherman::util;
namespace lth_jc   = leatherman::json_container;


message::message(const PCPClient::ParsedChunks &parsed_chunks)
        : id_ {parsed_chunks.envelope.get<std::string>("id")},
          message_type_ {parsed_chunks.envelope.get<std::string>("message_type")},
          sender_ {parsed_chunks.envelope.get<std::string>("sender")}
{
    init(parsed_chunks);
}

message::message(const std::string &message_type,
                 const std::string &sender,
                 const std::string &transaction,
                 std::string error_msg)
        : id_ {util::get_UUID()},
          message_type_ {message_type},
          sender_ {sender},
          transaction_ {transaction},
          timestamp_ {lth_util::get_date_time()},
          error_msg_ {std::move(error_msg)}
{
}

const std::string& message::id() const           { return id_; }
const std::string& message::message_type() const { return message_type_; }
const std::string& message::sender() const       { return sender_; }
const std::string& message::transaction() const  { return transaction_; }
const std::string& message::timestamp() const    { return timestamp_; }
const std::string& message::error_msg() const    { return error_msg_; }

lth_jc::JsonContainer message::get_data() const
{
    lth_jc::JsonContainer data {};
    data.set<std::string>("transaction", transaction_);
    data.set<std::string>("timestamp", timestamp_);

    if (message_type_ == schemas::ERROR_TYPE)
        data.set<std::string>("error_msg", error_msg_);

    return data;
}

void message::init(const PCPClient::ParsedChunks &parsed_chunks)
{
    validateFormat(parsed_chunks);
    transaction_ = parsed_chunks.data.get<std::string>("transaction");
    timestamp_   = parsed_chunks.data.get<std::string>("timestamp");

    if (message_type_ == schemas::ERROR_TYPE)
        error_msg_ = parsed_chunks.data.get<std::string>("error_msg");
}

void message::validateFormat(const PCPClient::ParsedChunks &parsed_chunks)
{
    if (!parsed_chunks.has_data)
        throw message::error("no data");
    if (parsed_chunks.invalid_data)
        throw message::error("invalid data");
    if (parsed_chunks.data_type != PCPClient::ContentType::Json)
        throw message::error("data is not in JSON format");
}

}  // namespace pcp_test
