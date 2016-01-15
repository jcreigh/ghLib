
#include "ghLib/Button.h"

namespace ghLib {

ButtonRunner *ButtonRunner::instance = nullptr;
std::atomic_flag ButtonRunner::taskRunning = ATOMIC_FLAG_INIT;
std::vector<Button*> Button::buttons;
std::mutex Button::buttonsMutex;

ButtonRunner::ButtonRunner() {
}

/**
 * Task to periodically check button input
 */
void ButtonRunner::Task() {
	while (taskRunning.test_and_set()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		Button::buttonsMutex.lock();
		for (auto button : Button::buttons) {
			button->Update();
		}
		Button::buttonsMutex.unlock();
	}
}

ButtonRunner *ButtonRunner::GetInstance() {
	if (instance == nullptr) {
		instance = new ButtonRunner();
	}
	return instance;
}

/**
 *  Enable or disable the ButtonChecker task
 */
void ButtonRunner::SetEnabled(bool enabled) { // Enable or disable the ButtonChecker task
	if (instance == nullptr) {
		instance = new ButtonRunner();
	}
	if (enabled) {
		if (!taskRunning.test_and_set()) { // Test if started, and if not, start it
			instance->Start();
		}
	} else {
		taskRunning.clear();
		while (instance->Running()) { // Wait for task to end
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
	}
}

/**
 * Search for a Button by key
 * @param key Configuration key to search by
 */
Button* Button::FromConfig(std::string key, bool createNotFound /* = true*/) {
	for (auto button : buttons) {
		if (button->config == key) {
			return button;
		}
	}
	Button* button = nullptr;
	if (createNotFound) {
		button = new Button(key);
		if (button->type == kInvalid) {
			delete button;
			return nullptr;
		}
	}
	return button;
}

/**
 * Add a Button to be checked by the ButtonChecker Task
 * @param buttonPtr Pointer to a Button
 */
void Button::AddButton(Button* buttonPtr) {
	buttonsMutex.lock();
	buttons.push_back(buttonPtr); // Add new button
	buttonsMutex.unlock();
}

/**
 * Simple constructor for a Button
 * @param buttonChannel The number for the button
 * @param mode The mode for the button (Default: kRaw)
 */
Button::Button(int buttonChannel, Mode mode /* = Button::kRaw */) : buttonChannel(buttonChannel), mode(mode) {
	stick = ghLib::Joystick::GetStickForPort(0);
	AddButton(this);
}

/**
 * Simple constructor for a Button
 * @param buttonChannel The number for the button
 * @param stick The Joystick the Button is on
 * @param mode The mode for the button (Default: kRaw)
 */
Button::Button(int buttonChannel, ghLib::Joystick* stick, Mode mode /* = Button::kRaw */) : buttonChannel(buttonChannel), stick(stick), mode(mode) {
	AddButton(this);
}

/**
 * Constructor for a Button to load from Preferences
 * <p><b>Button Preferences</b><br>
 * foo = channel. Either button number, axis number, or pov direction (int)<br>
 * foo.js = joystick (int)<br>
 * foo.mode = raw, press, release, toggle (string)<br>
 * foo.type = button, pov, axis (string)<br>
 * foo.threshold = axis threshold. If negative, value must be less than it. (valid only for axis) (float)<br>
 * foo.index = index of the pov (valid only for pov) (int)
 * </p>
 * @param buttonConfig The configuration key for the button
 */
Button::Button(std::string buttonConfig) {
	auto pref = NetworkTable::GetTable("Preferences");
	auto logger = ghLib::Logger::getLogger("Button");
	config = buttonConfig;

	if (!pref->ContainsSubTable(buttonConfig)) {
		logger->error(ghLib::Format("Attempting to load config '" + buttonConfig + "' and could not find it"));
		type = kInvalid;
		return;
	} else {
		pref = NetworkTable::GetTable("Preferences/" + buttonConfig);
		buttonChannel = (int)pref->GetNumber("channel", 1); // Specify a default even though this won't be called if there's no value
		auto stickNum = (int)pref->GetNumber("js", 0); // Default to joystick 0
		stick = ghLib::Joystick::GetStickForPort(stickNum);
		std::string modeStr = pref->GetString("mode", "raw");
		if (modeStr == "press") {
			SetMode(kPress);
		} else if (modeStr == "release") {
			SetMode(kRelease);
		} else if (modeStr == "toggle") {
			SetMode(kToggle);
		} else if (modeStr == "raw") {
			SetMode(kRaw);
		} else {
			logger->error(ghLib::Format("Attempting to load config '" + buttonConfig + "' and found an unknown mode '" + modeStr + "'. Defaulting to 'raw'"));
			SetMode(kRaw);
		}

		std::string typeStr = pref->GetString("type", "button");
		if (typeStr == "button") {
			type = kButton;
		} else if (typeStr == "pov") {
			type = kPOV;
			povIndex = (int)pref->GetNumber("pov", 0);
		} else if (typeStr == "axis") {
			type = kAxis;
		} else if (typeStr == "virtual") {
			type = kVirtual;
			auto virtualStr = pref->GetString("virtual", "");
			// Search for a Button first, then an Axis if not found.
			otherButton = Button::FromConfig(virtualStr, false);
			if (otherButton == nullptr) {
				otherAxis = Axis::FromConfig(virtualStr);
			}
			if (otherButton == nullptr && otherAxis == nullptr) {
				type = kInvalid;
				logger->error(ghLib::Format("Attempting to load config '" + buttonConfig + "' and could note find virtual '" + virtualStr + "'"));
				return;
			}
		} else if (typeStr == "analog") {
			type = kAnalog;
			analog = new ghLib::AnalogInput(buttonChannel);
			average = pref->GetBoolean("analogAverage", false);
		} else if (typeStr == "digital") {
			type = kDigital;
			digital = new ghLib::DigitalInput(buttonChannel);
		} else {
			logger->error(ghLib::Format("Attempting to load config '" + buttonConfig + "' and found an unknown type '" + typeStr + "'. Defaulting to 'button'"));
			type = kButton;
		}
		std::string loadedBuf = ghLib::Format("[%p] Loaded config '%s', stick '%d', type '%s', ", this, buttonConfig.c_str(), stickNum, typeStr.c_str());

		if (type == kPOV) {
			loadedBuf += ghLib::Format("povIndex '%d', ", povIndex);
		} else if (type == kAxis || type == kAnalog || otherAxis != nullptr){
			threshold = (float)pref->GetNumber("threshold", 0.95f);
			loadedBuf += ghLib::Format("threshold '%.2f', ", threshold);
		} else if (type == kVirtual) {
			if (otherButton) {
				loadedBuf += ghLib::Format("otherButton '%s' '%p', ", pref->GetString("virtual", "").c_str(), otherButton);
			} else if (otherAxis) {
				loadedBuf += ghLib::Format("otherAxis '%s' '%p', ", pref->GetString("virtual", "").c_str(), otherButton);
			}
		}
		invert = pref->GetBoolean("invert", false);

		loadedBuf += ghLib::Format("mode '%s', channel '%d', invert '%s'", modeStr.c_str(), buttonChannel, invert ? "true" : "false");
		logger->info(loadedBuf);

	}
	AddButton(this);
}

/**
 * Deconstructor for a Button
 */
Button::~Button() {
	buttonsMutex.lock();
	buttons.erase(std::remove(buttons.begin(), buttons.end(), this), buttons.end());
	buttonsMutex.unlock();
}

/**
 * Read value of the Button and update according to mode
 */
void Button::Update() {
	bool newValue;
	auto logger = ghLib::Logger::getLogger("Button");
	if (type == kButton) {
		if (buttonChannel > 0 && buttonChannel <= stick->GetButtonCount()) {
			newValue = stick->GetRawButton(buttonChannel);
		} else {
			newValue = false;
		}
	} else if (type == kPOV) {
		// 8 direction POV, each position is 45° from eachother
		if (povIndex >= 0 && buttonChannel < stick->GetButtonCount()) {
			newValue = (45 * buttonChannel) == stick->GetPOV(povIndex);
		}
	} else if (type == kAxis) {
		float axisValue = 0;
		if (buttonChannel >= 0 && buttonChannel < stick->GetAxisCount()) {
			axisValue = stick->GetRawAxis(buttonChannel);
		}
		newValue = AboveThreshold(axisValue, threshold);
		logger->trace(ghLib::Format("[:%d] [%p] axis = %0.2f, threshold = %0.2f, value = %s",
		                __LINE__, this, axisValue, threshold, newValue ? "true" : "false"));
	} else if (type == kVirtual) {
		if (otherButton != nullptr) {
			newValue = otherButton->Get();
			logger->trace(ghLib::Format("[:%d] [%p] otherButton (%p) = %0.2f", __LINE__, this, otherButton, newValue));
		} else if (otherAxis != nullptr){
			newValue = AboveThreshold(otherAxis->Get(), threshold);
			logger->trace(ghLib::Format("[:%d] [%p] otherAxis (%p) = %0.2f, threshold = %0.2f, value = %s",
			              __LINE__, this, otherAxis, (float)*otherAxis, threshold, newValue ? "true" : "false"));
		} else {
			newValue = false;
		}
	} else if (type == kAnalog) {
			auto axisValue = ghLib::Interpolate(average ? analog->GetAverageValue() : analog->GetValue(), 0, 4095, -1.0f, 1.0f);
			newValue = AboveThreshold(axisValue, threshold);
			logger->trace(ghLib::Format("[:%d] [%p] analog (%p) = %0.2f, threshold = %0.2f, value = %s",
			              __LINE__, this, analog, axisValue, threshold, newValue ? "true" : "false"));
	} else if (type == kDigital) {
		newValue = digital->Get();
	} else {
		newValue = false;
	}

	if (mode == Button::kRaw) {
		value = newValue;
	} else if (mode == Button::kPress) {
		if (!lastValue && newValue) { // If wasn't pressed but now is
			value = true; // Set to true (leading edge)
		}
	} else if (mode == Button::kRelease) {
		if (lastValue && !newValue) { // If was pressed but now isn't
			value = true; // Set to true (trailing edge)
		}
	} else if (mode == Button::kToggle) {
		if (!lastValue && newValue) { // If wasn't pressed  but now is
			value = !value; // Invert
		}
	}
	lastValue = newValue;
}

/**
 * Set Button's mode
 * @param newMode The button mode to set
 */
void Button::SetMode(Button::Mode newMode) {
	mode = newMode;
}

/**
 * Get if the button is currently pressed on not
 * @return Raw value of the Button
 */
bool Button::IsPressed() {
	return lastValue;
}

/**
 * Get the value of the button and reset if needed
 * @param reset Resets value and last value to false (Default: true)
 * @return Value of the Button
 */
bool Button::Get(bool reset /* = true */) {
	bool returnValue = value;
	if ((mode == Button::kPress || mode == Button::kRelease) && reset) {
		value = false; // Reset to false to show that we've read it
		//lastValue = false;
	}
  // If in Toggle or Raw mode, just return the value

	return returnValue ^ invert;
}

}
