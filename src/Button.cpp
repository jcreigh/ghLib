
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
Button* Button::FromConfig(std::string key) {
	for (auto button : buttons) {
		if (button->config == key) {
			return button;
		}
	}
	Button* button = new Button(key);
	if (button->src == kInvalid) {
		delete button;
		return nullptr;
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
 * @brief Constructor for Button to load from a configuration structure
 * @details
 * Example:<br/>
 * `ghLib::Button::Config c;` <br/>
 * `c.config = "Button"; c.channel = 5; c.js = 2; c.deadband = 0.05f; c.invert = true;`<br/>
 * `auto button = ghLib::Button(c);`<br/>
 */
Button::Button(Button::Config buttonConfig) {
	LoadFromConfig(buttonConfig);
}

/**
 * @brief Constructor for Button to load from a configuration lambda
 * @details
 * Example:<br/>
 * `auto button = ghLib::Button([](auto& c) { c.config = "Button"; c.channel = 5; c.js = 2; c.invert = true;});`
 */
Button::Button(std::function<void(Button::Config&)> configLambda) {
	Button::Config c;
	configLambda(c);
	LoadFromConfig(c);
}

/**
 * @brief Constructor for a Button to load from Preferences
 * @details
 *  Mode     | Description
 * ----------|-------------
 * `press`   | Triggers on rising edge
 * `release` | Triggers on trailing edge
 * `toggle`  | Triggers on rising and trailing edge
 * `raw`     | Passes through raw state
 *
 *  Source   | Description
 * ----------|-------------
 * `button`  | Joystick button
 * `pov`     | Joystick POV
 * `axis`    | Joystick axis
 * `virtual` | Another Button or Axis
 * `analog`  | Analog port
 * `digital` | Digital port
 *
 *  Preference  | Type   | Default  | Description
 * -------------|--------|----------|-------------
 * `type`       | string |          | Must be 'button'
 * `channel`    | int    | `1`      | Channel. Either button number, axis number, pov direction, or digital/analog channel
 * `js`         | int    | `0`      | Joystick number
 * `mode`       | string | `raw`    | raw, press, release, toggle
 * `src`        | string | `button` | button, pov, axis, virtual, analog, digital
 * `threshold`  | float  | `0.95`   | Threshold. If negative, value must be less than it (axis and analog only)
 * `average`    | bool   | `false`  | Use average analog input
 * `pov`        | int    | `0`      | Index of the POV (POV only)
 * `virtual`    | string |          | Name of other button or axis
 *
 *
 * @param buttonConfig The configuration key for the button
 */
Button::Button(std::string buttonConfig) {
	LoadFromConfig(buttonConfig);
}

/**
 * @brief Load Button configuration from Config struct
 */
void Button::LoadFromConfig(Button::Config buttonConfig) {
	auto pref = NetworkTable::GetTable("Preferences");

	config = buttonConfig.config;
	auto table = pref->GetSubTable(config);

	table->SetDefaultString("type", "button");
	table->SetDefaultNumber("channel", buttonConfig.channel);
	table->SetDefaultNumber("js", buttonConfig.js);
	table->SetDefaultString("mode", buttonConfig.mode);
	table->SetDefaultString("src", buttonConfig.src);
	table->SetDefaultNumber("threshold", buttonConfig.threshold);
	table->SetDefaultBoolean("average", buttonConfig.average);
	table->SetDefaultNumber("pov", buttonConfig.pov);
	table->SetDefaultString("virtual", buttonConfig.virtualSrc);

	LoadFromConfig(config);
}


/**
 * @brief Load Button configuration from Preferences
 */
void Button::LoadFromConfig(std::string buttonConfig) {
	auto pref = NetworkTable::GetTable("Preferences");
	auto logger = ghLib::Logger::getLogger("Button");
	config = buttonConfig;

	if (!pref->ContainsSubTable(buttonConfig)) {
		logger->error(ghLib::Format("Attempting to load config '" + buttonConfig + "' and could not find it"));
		src = kInvalid;
		return;
	} else {
		pref = NetworkTable::GetTable("Preferences/" + buttonConfig);
		auto type = pref->GetString("type", "");
		if (type != "button") {
			logger->error(ghLib::Format("Attempting to load config '" + buttonConfig + "' and found unexpected type '" + type + "'"));
			src = kInvalid;
			return;
		}
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

		std::string srcStr = pref->GetString("src", "button");
		if (srcStr == "button") {
			src = kButton;
		} else if (srcStr == "pov") {
			src = kPOV;
			povIndex = (int)pref->GetNumber("pov", 0);
		} else if (srcStr == "axis") {
			src = kAxis;
		} else if (srcStr == "virtual") {
			src = kVirtual;
			auto virtualStr = pref->GetString("virtual", "");
			auto virtualType = NetworkTable::GetTable("Preferences")->GetString(virtualStr + "/type", "");
			if (virtualType == "axis") {
				otherAxis = Axis::FromConfig(virtualStr);
			} else if (virtualType == "button") {
				otherButton = Button::FromConfig(virtualStr);
			} else {
				logger->error(ghLib::Format("Attempting to load config '" + buttonConfig + "' and could not load virtual '" + virtualStr + "'"));
				src = kInvalid;
				return;
			}
		} else if (srcStr == "analog") {
			src = kAnalog;
			analog = new ghLib::AnalogInput(buttonChannel);
			average = pref->GetBoolean("analogAverage", false);
		} else if (srcStr == "digital") {
			src = kDigital;
			digital = new ghLib::DigitalInput(buttonChannel);
		} else {
			logger->error(ghLib::Format("Attempting to load config '" + buttonConfig + "' and found an unknown src '" + srcStr + "'. Defaulting to 'button'"));
			src = kButton;
		}
		std::string loadedBuf = ghLib::Format("[%p] Loaded config '%s', stick '%d', src '%s', ", this, buttonConfig.c_str(), stickNum, srcStr.c_str());

		if (src == kAxis || src == kAnalog || otherAxis != nullptr){
			threshold = (float)pref->GetNumber("threshold", 0.95f);
			loadedBuf += ghLib::Format("threshold '%.2f', ", threshold);
		}
		if (src == kPOV) {
			loadedBuf += ghLib::Format("povIndex '%d', ", povIndex);
		} else if (src == kVirtual) {
			loadedBuf += ghLib::Format("virtual '%s' '%p', ", pref->GetString("virtual", "").c_str(),
			             otherButton != nullptr ? (void*)otherButton : (void*)otherAxis);
		} else if (src == kAnalog) {
			loadedBuf += ghLib::Format("average '%s', ", average ? "yes" : "no");
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
	if (src == kButton) {
		if (buttonChannel > 0 && buttonChannel <= stick->GetButtonCount()) {
			newValue = stick->GetRawButton(buttonChannel);
		} else {
			newValue = false;
		}
	} else if (src == kPOV) {
		// 8 direction POV, each position is 45° from eachother
		if (povIndex >= 0 && buttonChannel < stick->GetButtonCount()) {
			newValue = (45 * buttonChannel) == stick->GetPOV(povIndex);
		}
	} else if (src == kAxis) {
		float axisValue = 0;
		if (buttonChannel >= 0 && buttonChannel < stick->GetAxisCount()) {
			axisValue = stick->GetRawAxis(buttonChannel);
		}
		newValue = AboveThreshold(axisValue, threshold);
		logger->trace(ghLib::Format("[:%d] [%p] axis = %0.2f, threshold = %0.2f, value = %s",
		                __LINE__, this, axisValue, threshold, newValue ? "true" : "false"));
	} else if (src == kVirtual) {
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
	} else if (src == kAnalog) {
			auto axisValue = ghLib::Interpolate(average ? analog->GetAverageValue() : analog->GetValue(), 0, 4095, -1.0f, 1.0f);
			newValue = AboveThreshold(axisValue, threshold);
			logger->trace(ghLib::Format("[:%d] [%p] analog (%p) = %0.2f, threshold = %0.2f, value = %s",
			              __LINE__, this, analog, axisValue, threshold, newValue ? "true" : "false"));
	} else if (src == kDigital) {
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
