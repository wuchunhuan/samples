/*===============================================================
*   Copyright (C) 2019 All rights reserved.
*   
*   Filename    : rw_lock.h
*   Author      : Patrickwu(wuchunhuan@gmail.com)
*   Date        : 2019/05/24
*   Description : Implementation of fair read/write lock and
*                 write first read/write lock.
*   Update log  : 
*
================================================================*/
#pragma once

#include <mutex>
#include <condition_variable>

namespace common {

    class rw_lock {
    public:
        virtual void lock_read() = 0;
        virtual void unlock_read() = 0;
        virtual void lock_write() = 0;
        virtual void unlock_write() = 0;
    };

    class fair_rw_lock : public rw_lock{
    public:
        fair_rw_lock();

        void lock_read();
        void unlock_read();
        void lock_write();
        void unlock_write();

    private:
        bool can_lock_read();
        bool can_lock_write();

    private:
        std::mutex mtx_;
        std::condition_variable cond_;
        // -1    : one writer
        // 0     : no reader and no writer
        // n > 0 : n reader
        int64_t stat_;
    };

    //write first read/write lock
    class wf_rw_lock  : public rw_lock{
    public:
        wf_rw_lock();

        void lock_read();
        void unlock_read();
        void lock_write();
        void unlock_write();

    private:
        bool can_lock_read();
        bool can_lock_write();

    private:
        std::mutex mtx_;
        std::condition_variable r_cond_;
        std::condition_variable w_cond_;
        //how many locks on read
        size_t r_cnt_;
        //how many attempts to lock write
        size_t w_cnt_;
        bool in_write_;
    };

    class shared_lock {
    public:
        explicit shared_lock(rw_lock* rw_lockable) : rw_lockable_(rw_lockable){
            rw_lockable_->lock_read();
        }

        ~shared_lock() {
            rw_lockable_->unlock_read();
        }

        shared_lock() = delete;
        shared_lock(const shared_lock&) = delete;
        shared_lock&operator=(const shared_lock&) = delete;
    private:
        rw_lock* rw_lockable_;
    };

    class unique_lock {
    public:
        explicit unique_lock(rw_lock* rw_lockable) : rw_lockable_(rw_lockable){
            rw_lockable_->lock_write();
        }

        ~unique_lock() {
            rw_lockable_->unlock_write();
        }

        unique_lock() = delete;
        unique_lock(const unique_lock&) = delete;
        unique_lock&operator=(const unique_lock&) = delete;
    private:
        rw_lock* rw_lockable_;
    };
}
