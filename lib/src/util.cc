#include <pcp-test/util.hpp>

#include <boost/format.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <ctime>
#include <time.h>

namespace pcp_test {
namespace util {

static const std::string SHORT_DATETIME_FORMAT = "%C%m%d_%H%M";

static std::string get_expiry_datetime(int expiry_minutes, const std::string& format) {
    struct tm expiry_time_info;
    char expiry_time_buffer[80];

    // Get current time and add the specified minutes
    time_t expiry_time {time(nullptr)};
    expiry_time += 60 * expiry_minutes;

    // Get local time structure
    localtime_r(&expiry_time, &expiry_time_info);

    // Return the formatted string
    if (strftime(expiry_time_buffer, 80, format.c_str(), &expiry_time_info) == 0) {
        // invalid buffer
        return "";
    }

    return std::string(expiry_time_buffer);
}

std::string get_short_datetime()
{
    return get_expiry_datetime(0, SHORT_DATETIME_FORMAT);
}

std::string cyan(std::string const& message)
{
    static boost::format f {"\33[0;36m%1%\33[0m"};
    return  (f % message).str();
}

std::string green(std::string const& message)
{
    static boost::format f {"\33[0;32m%1%\33[0m"};
    return (f % message).str();
}

std::string yellow(std::string const& message)
{
    static boost::format f {"\33[0;33m%1%\33[0m"};
    return (f % message).str();
}

std::string red(std::string const& message)
{
    static boost::format f {"\33[0;31m%1%\33[0m"};
    return (f % message).str();
}

std::string get_UUID()
{
    static boost::uuids::random_generator gen;
    boost::uuids::uuid uuid = gen();
    return boost::uuids::to_string(uuid);
}

}  // namespace util
}  // namespace pcp-test
