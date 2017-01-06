/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2017. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#ifndef SRC_UTIL_BUTTON_H_
#define SRC_UTIL_BUTTON_H_

#include <vector>
#include <atomic>
#include <mutex>
#include <algorithm>

#include "ghLib/WPI.h"
#include "ghLib/Runnable.h"
#include "ghLib/Logger.h"
#include "ghLib/Util.h"
#include "ghLib/Axis.h"

#include "networktables/NetworkTable.h"


namespace ghLib {

class Button;

class ButtonRunner : public ghLib::Runnable {
	public:
		void Task();
		static ButtonRunner* GetInstance();
		static void SetEnabled(bool enabled);
	private:
		ButtonRunner();
		static ButtonRunner* instance;
		static std::atomic_flag taskRunning;
};

class Button {
	friend class ButtonRunner;
	public:
		struct Config {
			std::string config;
			int channel = 1;
			int js = 0;
			std::string mode = "raw";
			std::string src = "button";
			int threshold = 0.95f;
			bool average = false;
			int pov = 0;
			std::string virtualSrc = "";
		};
		std::string config;
		enum Mode {
			kRaw, kPress, kRelease, kToggle
		};
		Button(int buttonChannel, Mode mode = kRaw);
		Button(int buttonChannel, ghLib::Joystick* stick, Mode mode = kRaw);
		Button(Button::Config buttonConfig);
		Button(std::function<void(Button::Config&)> configLambda);
		Button(std::string buttonConfig);
		void LoadFromConfig(Button::Config buttonConfig);
		void LoadFromConfig(std::string buttonConfig);
		~Button();
		void Update();
		void SetMode(Mode newMode);
		bool IsPressed();
		bool Get(bool reset = true);
		static Button* FromConfig(std::string key);

	private:
		enum ChannelType {
			kButton, kPOV, kAxis, kVirtual, kDigital, kAnalog, kInvalid
		};
		bool value = false;
		bool lastValue = false;
		bool invert = false;
		ChannelType src = kButton;
		float threshold;
		int povIndex;
		int buttonChannel;
		ghLib::Joystick* stick;
		Mode mode = kRaw;
		Button* otherButton = nullptr;
		Axis* otherAxis = nullptr;
		AnalogInput* analog = nullptr;
		DigitalInput* digital = nullptr;
		bool average = false;
		static std::vector<Button*> buttons;
		static std::mutex buttonsMutex;
		static void AddButton(Button* buttonPtr);
};

}

#endif /* SRC_UTIL_BUTTON_H_ */
