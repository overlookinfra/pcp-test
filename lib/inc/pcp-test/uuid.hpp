/**
 * @file
 * Utility function for generating UUIDs.
 */

#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <string>

namespace pcp_test {

static std::string get_UUID() __attribute__((unused));

/// Return a new UUID instance
static std::string get_UUID() {
    static boost::uuids::random_generator gen;
    boost::uuids::uuid uuid = gen();
    return boost::uuids::to_string(uuid);
}

}  // namespace pcp_test
