/*===============================================================
*   Copyright (C) 2019 All rights reserved.
*   
*   Filename    : main.cpp
*   Author      : Patrickwu(wuchunhuan@gmail.com)
*   Date        : 2019/05/24
*   Description : 
*
*   Update log  : 
*
================================================================*/
//g++ main.cpp rw_lock.cpp -o lock_test -std=c++11 -pthread

#include "rw_lock.h"
#include <iostream>
#include <thread> 
#include <unistd.h>

using namespace common;
typedef common::shared_lock r_lock;
typedef common::unique_lock w_lock;
wf_rw_lock lock_mutex_;

void fr1() {
	int a = 5;
	while(a > 0) {
		r_lock lock(&lock_mutex_);
		std::cout << "read in fr1" << std::endl;
		a--;
	}
}

void fr2() {
	int a = 5;
	while(a > 0) {
		r_lock lock(&lock_mutex_);
		std::cout << "read in fr2" << std::endl;
		a--;
	}
}

void fw() {
	int a = 5;
	while(a > 0) {
		w_lock lock(&lock_mutex_);
		std::cout << "write in fw" << std::endl;
		sleep(5);
		a--;
	}
}

int main () {
	std::thread t2(fr1);
	//std::thread t1(fw);
	std::thread t3(fr2);
	//t1.join();
    t2.join();
    t3.join();

	return 0;
}
