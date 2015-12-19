/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#ifndef GHLIB_LOGGER_H_
#define GHLIB_LOGGER_H_

#ifdef ROBOT
#include "WPILib.h"

// Because wpilib pollutes the global namespace
#ifdef ERROR
#undef ERROR
#endif

#endif

#include "narf/Logger.h"

namespace ghLib {
	typedef ::narf::Logger Logger;
}

#endif /* GHLIB_LOGGER_H_ */
