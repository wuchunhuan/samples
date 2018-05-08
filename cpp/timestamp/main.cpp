#include <iostream>
#include <unistd.h>
#include "timestamp.h"
int main() {
    std::cout << "get_time_stamp : " << get_time_stamp_ms() << ", get_time_stamp1: " << get_time_stamp_ms1() << std::endl;
    int64_t ts1 = get_time_stamp_us1();
    usleep(1000);
    int64_t ts2 = get_time_stamp_us1();
    std::cout << "Elapsed time: " << ts2 - ts1 << " us";
}