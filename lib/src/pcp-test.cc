#include <pcp-test/version.h>
#include <pcp-test/pcp-test.hpp>

#include <leatherman/logging/logging.hpp>

namespace pcp_test {

std::unordered_map<std::string, test_type> to_test_type {
        {{"trivial", test_type::trivial},
         {"none", test_type::none}}
};

std::string version()
{
    LOG_DEBUG("pcp-test version is %1%", PCP_TEST_VERSION_WITH_COMMIT);
    return PCP_TEST_VERSION_WITH_COMMIT;
}

}  // namespace pcp_test
