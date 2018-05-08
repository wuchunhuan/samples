#include <cstdint>
#include <boost/date_time/posix_time/posix_time.hpp>
//精确到毫秒
int64_t get_time_stamp_ms() {
    boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::gregorian::Jan, 1));
    boost::posix_time::time_duration time_from_epoch =
        boost::posix_time::microsec_clock::universal_time() - epoch;
    return time_from_epoch.total_milliseconds();
}
//精确到微秒
int64_t get_time_stamp_us() {
    boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::gregorian::Jan, 1));
    boost::posix_time::time_duration time_from_epoch =
        boost::posix_time::microsec_clock::universal_time() - epoch;
    return time_from_epoch.total_microseconds();
}

//精确到毫秒
int64_t get_time_stamp_ms1() {
    struct timeval now;
    gettimeofday(&now, NULL);
    return now.tv_sec * 1000 + now.tv_usec / 1000;
}
//精确到微秒
int64_t get_time_stamp_us1() {
    struct timeval now;
    gettimeofday(&now, NULL);
    return now.tv_sec * 1000000 + now.tv_usec;
}
