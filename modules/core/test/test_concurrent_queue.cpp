/*L*************************************************************************************************
*
*  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
*
*  By downloading, copying, installing or using the software you agree to this license. If you do
*  not agree to this license, do not download, install, copy or use the software.
*
*                            Software License Agreement (BSD License)
*                               For Smart Surveillance Framework
*                                 http://ssig.dcc.ufmg.br/ssf/
*
*  Copyright (c) 2013, Smart Surveillance Interest Group, all rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted
*  provided that the following conditions are met:
*
*    1. Redistributions of source code must retain the above copyright notice, this list of
*       conditions and the following disclaimer.
*
*    2. Redistributions in binary form must reproduce the above copyright notice, this list of
*       conditions and the following disclaimer in the documentation and/or other materials
*       provided with the distribution.
*
*    3. Neither the name of the copyright holder nor the names of its contributors may be used to
*       endorse or promote products derived from this software without specific prior written
*       permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
*  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
*  AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
*  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
*  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
*  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*************************************************************************************************L*/

#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include <core/concurrent_queue.hpp>
#include <core/blocking_concurrent_queue.hpp>

TEST(ConcurrentQueue, general) {
	ssf::ConcurrentQueue<int> q;
	int dequeued[100] = { 0 };
	std::thread threads[20];

	// Producers
	for (int i = 0; i != 10; ++i) {
		threads[i] = std::thread([&](int i) {
			for (int j = 0; j != 10; ++j) {
				q.push(i * 10 + j);
			}
		}, i);
	}

	// Consumers
	for (int i = 10; i != 20; ++i) {
		threads[i] = std::thread([&]() {
			int item;
			for (int j = 0; j != 20; ++j) {
				if (q.pop(item)) {
					++dequeued[item];
				}
			}
		});
	}

	// Wait for all threads
	for (int i = 0; i != 20; ++i) {
		threads[i].join();
	}

	// Collect any leftovers (could be some if e.g. consumers finish before producers)
	int item;
	while (q.pop(item)) {
		++dequeued[item];
	}

	// Make sure everything went in and came back out!
	for (int i = 0; i != 100; ++i) {
		EXPECT_EQ(1, dequeued[i]);
	}



}


TEST(BlockingConcurrentQueue, general) {
	ssf::BlockingConcurrentQueue<int> q;
	int dequeued[100] = { 0 };
	std::thread threads[20];

	// Producers
	for (int i = 0; i != 10; ++i) {
		std::this_thread::sleep_for(std::chrono::milliseconds(30));
		threads[i] = std::thread([&](int i) {
			for (int j = 0; j != 10; ++j) {
				q.push(i * 10 + j);
			}
		}, i);
	}

	// Consumers
	for (int i = 10; i != 20; ++i) {
		threads[i] = std::thread([&]() {
			int item;
			for (int j = 0; j != 20; ++j) {
				if (q.pop(item)) {
					++dequeued[item];
				}
			}
		});
	}

	// Wait for all threads
	for (int i = 0; i != 20; ++i) {
		threads[i].join();
	}

	// Collect any leftovers (could be some if e.g. consumers finish before producers)
	int item;
	while (q.pop(item)) {
		++dequeued[item];
	}

	// Make sure everything went in and came back out!
	for (int i = 0; i != 100; ++i) {
		EXPECT_EQ(1, dequeued[i]);
	}



}