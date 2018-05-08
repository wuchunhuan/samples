#include <unistd.h>
#include <mutex>
#include <thread>
#include "cache.h"

std::mutex mtx;
Cache<std::string, int> cache;

void insert() {
    for (int i = 0; i < 20; ++i) {
        std::unique_lock<std::mutex> lock(mtx);
        cache.Insert(std::to_string(i), i, 10);
        std::cout << "Key: " << std::to_string(i) << ", value: " << i << " is inserted!" << std::endl;
    }
}

void get() {
    int val = 0;
    for (int i = 0; i < 20; ++i) {
        std::unique_lock<std::mutex> lock(mtx);
        if (cache.Find(std::to_string(i), val) == 0) {
            std::cout << "Key: " << std::to_string(i) << ", value: " << val << " is founded!" << std::endl;
        } else {
            std::cout << "Key: " << std::to_string(i) << " is not founded!" << std::endl;
        }
    }
}

int main() {
//    cache.Init(10, LRU, 10);
    cache.Init(10, LRU);
    std::thread th1(insert);
    sleep(3);
    std::thread th2(get);
    sleep(8);
    std::thread th3(get);

    th1.join();
    th2.join();
    th3.join();
    return 0;
}