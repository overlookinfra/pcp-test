/**
 * @file
 * Declares a utility for retrieving the library version.
 */

#pragma once

#include <pcp-test/export.h>

#include <string>
#include <unordered_map>

namespace pcp_test {

enum class test_type {
    none,
    connection,
    trivial
};

extern const std::unordered_map<std::string, test_type> to_test_type;

/**
 * Query the library version.
 * @return A version string with \<major>.\<minor>.\<patch>
 */
std::string LIBPCP_TEST_EXPORT version();

}  // namespace pcp_test
