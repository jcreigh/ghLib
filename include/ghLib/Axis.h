/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#ifndef SRC_GHLIB_AXIS_H_
#define SRC_GHLIB_AXIS_H_

#include <vector>
#include "ghLib/Joystick.h"
#include "ghLib/Logger.h"
#include "ghLib/Util.h"

#include "networktables/NetworkTable.h"

namespace ghLib {

class Axis {
	public:
		Axis(int axisChannel);
		Axis(int axisChannel, ghLib::Joystick* stick);
		Axis(std::string axisConfig);
		~Axis();
		void SetDeadband(float newDeadband);
		float GetDeadband();
		void SetInvert(bool newInvert);
		bool GetInvert();
		float Get();
	private:
		int axisChannel;
		bool invert = false;
		float deadband;
		ghLib::Joystick* stick;
		std::string config;

};

}

#endif /* SRC_GHLIB_AXIS_H_ */
