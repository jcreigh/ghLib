/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2017. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#ifndef GHLIB_RUNNABLE_H_
#define GHLIB_RUNNABLE_H_

#include <chrono>
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

#endif /* GHLIB_RUNNABLE_H_ */
