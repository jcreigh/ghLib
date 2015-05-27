/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include "ghLib/Util.h"

namespace ghLib {

double Deadband(double v, double deadband) {
	// If v is within `deadband` of 0, then just return 0.
	// Else, interpolate |v| from [deadband, 1] into [0, 1] and
	// make it negative if it was that before
	return (std::abs(v) < deadband) ? 0 : (std::signbit(v) ? -1 : 1) * Interpolate(std::abs(v), deadband, 1.0, 0.0, 1.0);
}

}
