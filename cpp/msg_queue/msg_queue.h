/*===============================================================
*   Copyright (C) 2019 All rights reserved.
*   
*   Filename    : msg_queue.h
*   Author      : Patrickwu(wuchunhuan@gmail.com)
*   Date        : 2019/05/31
*   Description : 
*
*   Update log  : 
*
================================================================*/
#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>

template<typename T>
class msg_queue {
private:
    bool enabled;
    std::queue<T> the_queue;
    mutable std::mutex the_mutex;
    std::condition_variable the_condition_variable;

public:
    msg_queue();

    ~msg_queue();

    void destroy();

    bool send(T const &value);

    bool send(T const &&value);

    bool empty() const;

    bool try_receive(T &popped_value);

    bool receive(T &popped_value);

};
template<typename T>
msg_queue<T>::msg_queue() : enabled(true) {

}

template<typename T>
msg_queue<T>::~msg_queue() {
    destroy();
}

template<typename T>
void msg_queue<T>::destroy() {
    enabled = false;
    the_condition_variable.notify_all();
}

template<typename T>
bool msg_queue<T>::send(T const &value) {
    if (enabled) {
        std::unique_lock<std::mutex> lock(the_mutex);
        the_queue.push(value);
        lock.unlock();
        the_condition_variable.notify_one();
        return true;
    } else {
        return false;
    }
}

template<typename T>
bool msg_queue<T>::send(T const &&value) {
    if (enabled) {
        std::unique_lock<std::mutex> lock(the_mutex);
        the_queue.push(std::move(value));
        lock.unlock();
        the_condition_variable.notify_one();
        return true;
    } else {
        return false;
    }
}

template<typename T>
bool msg_queue<T>::empty() const {
    std::unique_lock<std::mutex> lock(the_mutex);
    return the_queue.empty();
}

template<typename T>
bool msg_queue<T>::try_receive(T &popped_value) {
    if (enabled) {
        std::unique_lock<std::mutex> lock(the_mutex);
        if (the_queue.empty()) {
            return false;
        }

        popped_value = the_queue.front();
        the_queue.pop();
        return true;
    } else {
        return false;
    }
}

template<typename T>
bool msg_queue<T>::receive(T &popped_value) {

    std::unique_lock<std::mutex> lock(the_mutex);
    while (the_queue.empty()) {
        if (enabled) {
            the_condition_variable.wait(lock);
        } else {
            return false;
        }
    }

    popped_value = the_queue.front();
    the_queue.pop();
    return true;
}
