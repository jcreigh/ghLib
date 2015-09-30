/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include "ghLib/util/Clock.h"
#include "ghLib/Logger.h"

namespace ghLib {

std::chrono::steady_clock::time_point Clock::started = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point Clock::epoch;

Clock::time_point Clock::now() noexcept {
	std::chrono::steady_clock::time_point t = std::chrono::steady_clock::now() - started.time_since_epoch();
	auto after_epoch = epoch.time_since_epoch() + t;
	duration dur = std::chrono::duration_cast<duration>(after_epoch.time_since_epoch());
	return Clock::time_point(dur);
}

void Clock::setEpoch(uint64_t milliseconds) {
	auto log = ghLib::Logger::GetLogger("Debug.Clock");
	log->Trace(ghLib::Format("Setting epoch to %d", milliseconds));
	epoch = std::chrono::steady_clock::time_point(std::chrono::milliseconds(milliseconds));
	//log->Trace(ghLib::Format("Epoch is %d", epoch.time_since_epoch().count()));
}

uint64_t Clock::getEpoch() {
	return (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(epoch.time_since_epoch()).count();
}


}
