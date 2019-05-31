/*===============================================================
*   Copyright (C) 2019 All rights reserved.
*   
*   Filename    : rw_lock.cpp
*   Author      : Patrickwu(wuchunhuan@gmail.com)
*   Date        : 2019/05/24
*   Description : 
*
*   Update log  : 
*
================================================================*/
#include "rw_lock.h"
common::fair_rw_lock::fair_rw_lock() : stat_(0) {

}

void common::fair_rw_lock::lock_read() {
    std::unique_lock<std::mutex> lock(mtx_);
//    while (stat_ < 0) {
//        cond_.wait(lock);
//    }
    cond_.wait(lock, std::bind(&common::fair_rw_lock::can_lock_read, this));
    ++stat_;
}

bool common::fair_rw_lock::can_lock_read() {
    return stat_ < 0;
}

void common::fair_rw_lock::unlock_read() {
    std::unique_lock<std::mutex> lock(mtx_);
    if (--stat_ == 0) {
        cond_.notify_one();
    }
}

void common::fair_rw_lock::lock_write() {
    std::unique_lock<std::mutex> lock(mtx_);
    cond_.wait(lock, std::bind(&common::fair_rw_lock::can_lock_write, this));
    stat_ = -1;
}

bool common::fair_rw_lock::can_lock_write() {
    return stat_ == 0;
}

void common::fair_rw_lock::unlock_write() {
    std::unique_lock<std::mutex> lock(mtx_);
    cond_.notify_all();
}

common::wf_rw_lock::wf_rw_lock() : r_cnt_(0), w_cnt_(0), in_write_(false){

}

void common::wf_rw_lock::lock_read() {
    std::unique_lock<std::mutex> lock(mtx_);
    r_cond_.wait(lock, std::bind(&common::wf_rw_lock::can_lock_read, this));
    ++r_cnt_;
}

bool common::wf_rw_lock::can_lock_read() {
    return (w_cnt_ == 0 && !in_write_);
}

void common::wf_rw_lock::unlock_read() {
    std::unique_lock<std::mutex> lock(mtx_);
    if (--r_cnt_ == 0 && w_cnt_ > 0) {
        w_cond_.notify_one();
    }
}

void common::wf_rw_lock::lock_write() {
    std::unique_lock<std::mutex> lock(mtx_);
    ++w_cnt_;
    w_cond_.wait(lock, std::bind(&common::wf_rw_lock::can_lock_write, this));
}

bool common::wf_rw_lock::can_lock_write() {
    return (r_cnt_ == 0 && !in_write_);
}

void common::wf_rw_lock::unlock_write() {
    std::unique_lock<std::mutex> lock(mtx_);
    --w_cnt_;
    in_write_ = false;
    if (w_cnt_ == 0) {
        r_cond_.notify_all();
    } else {
        w_cond_.notify_one();
    }
}
