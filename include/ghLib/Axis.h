/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#ifndef SRC_GHLIB_AXIS_H_
#define SRC_GHLIB_AXIS_H_

#include <vector>
#include <functional>
#include "ghLib/WPI.h"
#include "ghLib/Logger.h"
#include "ghLib/Util.h"

#include "networktables/NetworkTable.h"

namespace ghLib {

class Axis {
	public:
		struct Range { float min, max; };
		struct Config {
			std::string config;
			int channel = 0;
			int js = 0;
			std::string src = "axis";
			float threshold = 0.95f;
			std::string virtualSrc = "";
			Range input = {0.0f, 0.0f};
			Range output = {0.0f, 0.0f};
			bool scale = false;
			bool invert = false;
			float deadband = 0.0f;
		};

		Axis(int axisChannel);
		Axis(int axisChannel, ghLib::Joystick* stick);
		Axis(Config axisConfig);
		Axis(std::function<void(Axis::Config&)> configLambda);
		Axis(std::string axisConfig);
		~Axis();
		void LoadFromConfig(Axis::Config axisConfig);
		void LoadFromConfig(std::string axisConfig);
		void SetDeadband(float newDeadband);
		float GetDeadband() const;
		void SetInvert(bool newInvert);
		bool GetInvert() const;
		float Get() const;
		float GetRaw() const;
		operator float() const;
		static Axis* FromConfig(std::string key);
	private:
		enum ChannelType {
			kAxis, kAnalog, kVirtual, kInvalid
		};
		ChannelType src = kAxis;
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
		static std::vector<Axis*> axes;
		static std::mutex axesMutex;
};

}

#endif /* SRC_GHLIB_AXIS_H_ */
