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
#include "ghLib/AnalogInput.h"
#include "ghLib/Util.h"

#include "networktables/NetworkTable.h"

namespace ghLib {

class Axis {
	public:
		Axis(int axisChannel);
		Axis(int axisChannel, ghLib::Joystick* stick);
		Axis(std::string axisConfig);
		~Axis();
		struct Range { float min, max; };
		void SetDeadband(float newDeadband);
		float GetDeadband() const;
		void SetInvert(bool newInvert);
		bool GetInvert() const;
		float Get() const;
		float GetRaw() const;
		operator float() const;
	private:
		enum ChannelType {
			kAxis, kAnalog, kVirtual, kInvalid
		};
		ChannelType type = kAxis;
		int axisChannel;
		bool invert = false;
		float deadband = 0.0f;
		ghLib::Joystick* stick;
		ghLib::AnalogInput* analog;
		std::string config;
		Axis* otherAxis;
		Range input = {-1.0f, 1.0f};
		Range output = {-1.0f, 1.0f};
		bool scale = false;  // Use output range
		bool average = false; // Use average value for analog
		static Axis* FindAxis(std::string key);
		static std::vector<Axis*> axes;
		static std::mutex axesMutex;
};

}

#endif /* SRC_GHLIB_AXIS_H_ */
