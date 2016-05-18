/**
 * @file
 * Utility functions.
 */

#pragma once

#include <string>

namespace pcp_test {
namespace util {

// Return the current datetime (no seconds) as a string.
std::string get_short_datetime();

// Wrap message with the POSIX cyan display code
std::string cyan(std::string const& message);

// Wrap message with the POSIX green display code
std::string green(std::string const& message);

// Wrap message with the POSIX yellow display code
std::string yellow(std::string const& message);

// Wrap message with the POSIX red display code
std::string red(std::string const& message);

// Return a new UUID
std::string get_UUID();

}  // namespace util
}  // namespace pcp-test
