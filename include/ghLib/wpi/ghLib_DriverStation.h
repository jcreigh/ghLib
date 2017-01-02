/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/
#ifndef SRC_GHLIB_DRIVERSTATION_H_
#define SRC_GHLIB_DRIVERSTATION_H_

#ifndef NOROBOT

#include "DriverStation.h"
namespace ghLib {
typedef ::frc::DriverStation DriverStation;
}

#else

#include <string>
#include <stdio.h>

namespace ghLib {

class DriverStation {
	public:
	static void ReportError(std::string error);
};

}

#endif

#endif /* SRC_GHLIB_DRIVERSTATION_H_ */
