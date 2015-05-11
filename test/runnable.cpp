#include <stdio.h>
#include <atomic>
#include "Runnable.h"
#include "gtest/gtest.h"

class RunnableTest : public ghLib::Runnable {
	public:
	std::atomic_bool stop;
	RunnableTest() {
		stop = false;
	}

	void Task() {
		while(!stop) {
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
	}

	~RunnableTest() {};
};

void RunTest() {
	RunnableTest foo;
	foo.Start();
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	EXPECT_TRUE(foo.Running());
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	foo.stop = true;
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	EXPECT_FALSE(foo.Running());
}

TEST(Runnable, Test1) {
	RunTest();
}

