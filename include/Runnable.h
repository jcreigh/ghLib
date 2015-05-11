/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include <thread>
#include <chrono>
#include <functional>
#include <future>

namespace ghLib {

class Runnable {
	public:
		bool Start();
		bool Running();
		virtual ~Runnable() {};

	private:
		virtual void Task() = 0;
		std::future<void> task;
};

}
