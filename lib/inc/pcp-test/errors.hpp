/**
 * @file
 * Error classes.
 */

#pragma once

#include <stdexcept>
#include <string>

namespace pcp_test {

/// Fatal error
struct fatal_error : public std::runtime_error {
    explicit fatal_error(std::string const& msg)
            : std::runtime_error(msg) {}
};

/// Configuration error
struct configuration_error : public std::runtime_error {
    explicit configuration_error(std::string const& msg)
            : std::runtime_error(msg) {}
};

}  // namespace pcp_test
