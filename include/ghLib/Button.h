/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
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
		static void AddButton(Button* buttonPtr);
		static void DelButton(Button* buttonPtr);
		static Button* FindButton(std::string key);
	private:
		ButtonRunner();
		static ButtonRunner* instance;
		static std::vector<Button*> buttons;
		static std::atomic_flag taskRunning;
		static std::mutex buttonsMutex;
};

class Button {
	public:
		std::string config;
		enum Mode {
			kRaw, kPress, kRelease, kToggle
		};
		Button(int buttonChannel, Mode mode = kRaw);
		Button(int buttonChannel, ghLib::Joystick* stick, Mode mode = kRaw);
		Button(std::string buttonConfig);
		~Button();
		void Update();
		void SetMode(Mode newMode);
		bool IsPressed();
		bool Get(bool reset = true);
	private:
		enum ChannelType {
			kButton, kPOV, kAxis, kVirtual, kDigital, kAnalog, kInvalid
		};
		bool value = false;
		bool lastValue = false;
		bool invert = false;
		ChannelType type = kButton;
		float threshold;
		int povIndex;
		int buttonChannel;
		ghLib::Joystick* stick;
		Mode mode = kRaw;
		Button* otherButton = nullptr;
		Axis* otherAxis = nullptr;
		AnalogInput* analog = nullptr;
		bool average = false;
};

}

#endif /* SRC_UTIL_BUTTON_H_ */
