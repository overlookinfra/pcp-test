/**
 * @file
 * RNG utility classes.
 */

#pragma once

#include <random>
#include <stdint.h>

namespace pcp_test {

class exponential_integers {
  public:
    using engine_type = std::default_random_engine;

    exponential_integers(double lambda, int seed_val = 0)
            : engine_ {},
              distribution_ {lambda}
    {
        if (seed_val)
            engine_.seed(static_cast<engine_type::result_type>(seed_val));
    }

    // RNG - return a random integer value, obtained by multiplying
    //       the value provided by the exp generator by 10^3, in
    //       order to model interarrivals in [ms], as we define the
    //       engine by specifying lambda which, in turn, is a
    //       frequency [Hz].
    uint32_t operator()()
    {
        return static_cast<uint32_t >(1000 * distribution_(engine_));
    }

  private:
    engine_type engine_;
    std::exponential_distribution<double> distribution_;
};

}  // namespace pcp_test
