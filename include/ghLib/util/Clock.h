/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#ifndef GHLIB_UTIL_CLOCK_H_
#define GHLIB_UTIL_CLOCK_H_

#include <stdint.h>
#include <chrono>

namespace ghLib {
	struct Clock {
		typedef std::chrono::milliseconds duration;
		typedef duration::rep rep;
		typedef duration::period period;
		typedef std::chrono::time_point<Clock> time_point;
		static const bool is_steady = false;
		static std::chrono::steady_clock::time_point started;
		static std::chrono::steady_clock::time_point epoch;

		static time_point now() noexcept;
		static void setEpoch(uint32_t seconds);
	};

}

#endif /* GHLIB_UTIL_CLOCK_H_ */
