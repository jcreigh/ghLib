/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include "ghLib/wpi/ghLib_DriverStation.h"

#ifdef NOROBOT
namespace ghLib {

void DriverStation::ReportError(std::string error) {
	printf("%s", error.c_str());
}

}
#endif
