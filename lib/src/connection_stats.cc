#include <pcp-test/connection_stats.hpp>

#include <cmath>
#include <iostream>

namespace pcp_test {

//
// stats
//

stats::stats() {
}

stats::stats(const timing_accumulator_t &a)
        : mean   {boost::accumulators::mean(a)},
          stddev {std::sqrt(boost::accumulators::variance(a))},
          max    {boost::accumulators::max(a)},
          count  {static_cast<uint32_t>(boost::accumulators::count(a)) }
{
}

//
// connection_stats
//

std::ostream &operator<<(std::ostream& out, const connection_stats& c_s)
{
    out << "; timing stats:\n"
        << "  TCP Connection: ..... mean "
        << c_s.tcp_us.mean / 1000 << " ms, std dev "
        << c_s.tcp_us.stddev / 1000 << " ms, max "
        << static_cast<float>(c_s.tcp_us.max) / 1000 << " ms\n"
        << "  WS Open Handshake: .. mean "
        << c_s.ws_open_handshake_us.mean / 1000 << " ms, std dev "
        << c_s.ws_open_handshake_us.stddev / 1000 << " ms, max "
        << static_cast<float>(c_s.ws_open_handshake_us.max) / 1000 << " ms\n"
        << "  PCP Association: .... mean "
        << c_s.association_ms.mean << " ms, std dev "
        << c_s.association_ms.stddev << " ms, max "
        << c_s.association_ms.max << " ms\n"
        << "  PCP Session: ........ mean "
        << c_s.session_duration_ms.mean / 1000 << " s, std dev "
        << c_s.session_duration_ms.stddev / 1000 << " s, max "
        << static_cast<float>(c_s.session_duration_ms.max) / 1000 << " s ("
        << c_s.session_duration_ms.count << " sessions)\n";

    return out;
}

std::ofstream& operator<<(boost::nowide::ofstream& out, const connection_stats& c_s)
{
    out << c_s.tcp_us.mean / 1000 << ","
        << c_s.tcp_us.stddev / 1000 << ","
        << static_cast<float>(c_s.tcp_us.max) / 1000 << ","
        << c_s.ws_open_handshake_us.mean / 1000 << ","
        << c_s.ws_open_handshake_us.stddev / 1000 << ","
        << static_cast<float>(c_s.ws_open_handshake_us.max) / 1000 << ","
        << c_s.association_ms.mean << ","
        << c_s.association_ms.stddev << ","
        << c_s.association_ms.max << ","
        << c_s.session_duration_ms.mean / 1000 << ","
        << c_s.session_duration_ms.stddev / 1000 << ","
        << static_cast<float>(c_s.session_duration_ms.max) / 1000;

    return out;
}

//
// connection_timings_accumulator
//

void connection_timings_accumulator::accumulate_tcp_us(uint32_t interval)
{
    std::lock_guard<std::mutex> the_lock {the_mutex_};
    tcp_us_acc_(interval);
}

void connection_timings_accumulator::accumulate_ws_open_handshake_us(uint32_t interval)
{
    std::lock_guard<std::mutex> the_lock {the_mutex_};
    ws_open_handshake_us_acc_(interval);
}

void connection_timings_accumulator::accumulate_ws_close_handshake_us(uint32_t interval)
{
    std::lock_guard<std::mutex> the_lock {the_mutex_};
    ws_close_handshake_us_acc_(interval);
}

void connection_timings_accumulator::accumulate_association_ms(uint32_t interval)
{
    std::lock_guard<std::mutex> the_lock {the_mutex_};
    association_ms_acc_(interval);
}

void connection_timings_accumulator::accumulate_session_duration_ms(uint32_t interval)
{
    std::lock_guard<std::mutex> the_lock {the_mutex_};
    session_duration_ms_acc_(interval);
}

connection_stats connection_timings_accumulator::get_connection_stats() const
{
    std::lock_guard<std::mutex> the_lock {the_mutex_};
    return connection_stats {stats(tcp_us_acc_),
                             stats(ws_open_handshake_us_acc_),
                             stats(ws_close_handshake_us_acc_),
                             stats(association_ms_acc_),
                             stats(session_duration_ms_acc_)};
}

}  // namespace pcp_test
