/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2017. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include "ghLib/Runnable.h"

namespace ghLib {

bool Runnable::Start() {
	task = std::async(std::launch::async, &Runnable::Task, this);
	return true;
}

bool Runnable::Running() {
	if (!task.valid()) {
		return false; // Thread has never started
	}
	auto status = task.wait_for(std::chrono::milliseconds(0));
	if (status == std::future_status::ready) {
		return false; // Thread has finished
	}
	return true;
}

}
