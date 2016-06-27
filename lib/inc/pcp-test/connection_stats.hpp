/**
 * @file
 * Accumulates and process WebSocket and PCP Association timing stats.
 */

#pragma once

#include <pcp-test/connection_stats.hpp>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>

#include <boost/nowide/fstream.hpp>

#include <ostream>
#include <chrono>
#include <mutex>
#include <stdint.h>
#include <string>

namespace pcp_test {

using timing_accumulator_t = boost::accumulators::accumulator_set<
                                uint32_t,
                                boost::accumulators::stats<
                                    boost::accumulators::tag::max,
                                    boost::accumulators::tag::mean,
                                    boost::accumulators::tag::variance>>;
struct stats
{
    double   mean;
    double   stddev;
    uint32_t max;
    uint32_t count;

    stats();
    explicit stats(const timing_accumulator_t& a);
};

struct connection_stats
{
    stats tcp_us;
    stats ws_open_handshake_us;
    stats ws_close_handshake_us;
    stats association_ms;
    stats session_duration_ms;

    friend std::ostream& operator<< (std::ostream& out_stream,
                                     const connection_stats& c_s);

    friend std::ofstream & operator<< (boost::nowide::ofstream& out_stream,
                                       const connection_stats& c_s);
};

struct connection_timings_accumulator
{
  public:
    void accumulate_tcp_us(uint32_t interval);
    void accumulate_ws_open_handshake_us(uint32_t interval);
    void accumulate_ws_close_handshake_us(uint32_t interval);
    void accumulate_association_ms(uint32_t interval);
    void accumulate_session_duration_ms(uint32_t interval);

    connection_stats get_connection_stats() const;

  private:
    // Synchronizes accumulate() access to state
    mutable std::mutex the_mutex_;

    timing_accumulator_t tcp_us_acc_;
    timing_accumulator_t ws_open_handshake_us_acc_;
    timing_accumulator_t ws_close_handshake_us_acc_;
    timing_accumulator_t association_ms_acc_;
    timing_accumulator_t session_duration_ms_acc_;
};

}  // namespace pcp_test
