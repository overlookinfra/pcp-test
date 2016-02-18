#include <pcp-test/version.h>
#include <pcp-test/pcp-test.hpp>

#include <leatherman/logging/logging.hpp>

namespace pcp_test {

    using namespace std;

    string version()
    {
        LOG_DEBUG("pcp-test version is %1%", PCP_TEST_VERSION_WITH_COMMIT);
        return PCP_TEST_VERSION_WITH_COMMIT;
    }

}  // pcp_test
