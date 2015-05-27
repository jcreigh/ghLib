/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/
#ifdef ROBOT

#include "WPILib.h"
namespace ghLib {
typedef ::DriverStation DriverStation;
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
