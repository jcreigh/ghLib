#include "ghLib/WPI.h"
#include "ghLib/Axis.h"

namespace ghLib {

std::vector<Axis*> Axis::axes;
std::mutex Axis::axesMutex;

/**
 * Simple constructor for a Axis
 * @param axisChannel The number for the axis
 */
Axis::Axis(int axisChannel) : axisChannel(axisChannel) {
	stick = ghLib::Joystick::GetStickForPort(0);
}

/**
 * Simple constructor for a Axis
 * @param axisChannel The number for the axis
 * @param stick The Joystick the Axis is on
 */
Axis::Axis(int axisChannel, ghLib::Joystick* stick) : axisChannel(axisChannel), stick(stick) {
}

/**
 * @brief Constructor for a Axis to load from Preferences
 * @details
 *  Source   | Description
 * ----------|-------------
 * `axis`    | Joystick axis
 * `virtual` | Another Button or Axis
 * `analog`  | Analog port
 *
 *  Preference  | Type   | Default  | Description
 * -------------|--------|----------|-------------
 * `type`       | string |          | Must be 'axis'
 * `channel`    | int    | `0`      | Channel. Either axis number, or analog channel
 * `js`         | int    | `0`      | Joystick number
 * `src`        | string | `button` | axis, virtual, analog
 * `threshold`  | float  | `0.95`   | Threshold. If negative, value must be less than it
 * `average`    | bool   | `false`  | Use average analog input
 * `virtual`    | string |          | Name of other axis
 * `input.min`  | float  | `-1.0`   | Range to coerce into
 * `input.max`  | float  | ` 1.0`   | &nbsp;
 * `output.min` | float  | `-1.0`   | Range to scale into
 * `output.max` | float  | ` 1.0`   | (if `scale` is set)
 * `scale`      | bool   | `false`  | Scale output
 * `invert`     | bool   | `false`  | Invert output
 * `deadband`   | float  | `0.0`    | Apply deadband
 * @param axisConfig The configuration key for the axis
 */
Axis::Axis(std::string axisConfig) {
	auto pref = NetworkTable::GetTable("Preferences");
	auto logger = ghLib::Logger::getLogger("Axis");
	config = axisConfig;

	if (!pref->ContainsSubTable(axisConfig)) {
		logger->error("Attempting to load config '" + axisConfig + "' and could not find it");
		src = kInvalid;
		return;
	} else {
		pref = NetworkTable::GetTable("Preferences/" + axisConfig);
		auto type = pref->GetString("type", "");
		if (type != "axis") {
			logger->error(ghLib::Format("Attempting to load config '" + axisConfig + "' and found unexpected type '" + type + "'"));
			src = kInvalid;
			return;
		}
		axisChannel = (int)pref->GetNumber("channel", 0);
		std::string srcStr = pref->GetString("src", "axis");
		auto stickNum = (int)pref->GetNumber("js", 0); // Default to joystick 0
		if (srcStr == "axis") {
			stick = ghLib::Joystick::GetStickForPort(stickNum);
			src = kAxis;
		} else if (srcStr == "analog") {
			analog = new ghLib::AnalogInput(axisChannel);
			average = pref->GetBoolean("analogAverage", false);
			src = kAnalog;
		} else if (srcStr == "virtual") {
			auto otherAxisStr = pref->GetString("virtual", "");
			if (NetworkTable::GetTable("Preferences")->GetString(otherAxisStr + "/type", "") == "axis") {
				otherAxis = FromConfig(otherAxisStr);
				src = kVirtual;
			} else {
				src = kInvalid;
			}
		}

		input.min = (float)pref->GetNumber("input/min", -1.0f);
		input.max = (float)pref->GetNumber("input/max", 1.0f);
		output.min = (float)pref->GetNumber("output/min", -1.0f);
		output.max = (float)pref->GetNumber("output/max", 1.0f);
		scale = pref->GetBoolean("scale", false);
		invert = pref->GetBoolean("invert", false);
		deadband = (float)pref->GetNumber("deadband", 0.0f);

		std::string loadedBuf = ghLib::Format("[%p] Loaded config '%s', src '%s', ", this, axisConfig.c_str(), srcStr.c_str());

		if (src == kVirtual) {
			loadedBuf += ghLib::Format("virtual '%s' '%p', ", pref->GetString("virtual", "").c_str(), otherAxis);
		} else if (src == kAnalog) {
			loadedBuf += ghLib::Format("average '%s', ", average ? "yes" : "no");
		} else if (src == kAxis) {
			loadedBuf += ghLib::Format("stick '%d', ", stickNum);
		}

		if (src == kAxis || src == kAnalog) {
			loadedBuf += ghLib::Format("channel '%d', ", axisChannel);
		}

		loadedBuf += ghLib::Format("input (%02f, %02f), output (%02f, %02f), scale '%s', invert '%s'",
		             input.min, input.max, output.min, output.max, scale ? "true" : "false", invert ? "true" : "false");
		logger->info(loadedBuf);

	}

	axesMutex.lock();
	axes.push_back(this); // Add new axis
	axesMutex.unlock();
}

/**
 * Deconstructor for a Axis
 */
Axis::~Axis() {
	axesMutex.lock();
	axes.erase(std::remove(axes.begin(), axes.end(), this), axes.end());
	axesMutex.unlock();
}

/**
 * Search for an Axis by key
 * @param key Configuration key to search by
 */
Axis* Axis::FromConfig(std::string key) {
	for (auto axis : axes) {
		if (axis->config == key) {
			return axis;
		}
	}
	Axis* axis = new Axis(key);
	if (axis->src == kInvalid) {
		delete axis;
		return nullptr;
	}
	return axis;
}

void Axis::SetDeadband(float newDeadband) {
	deadband = newDeadband;
}

float Axis::GetDeadband() const {
	return deadband;
}

void Axis::SetInvert(bool newInvert) {
	invert = newInvert;
}

bool Axis::GetInvert() const {
	return invert;
}

/**
* Get the raw value of the axis, without applying anything
* @return Value of the Axis
*/
float Axis::GetRaw() const {
	if (src == ChannelType::kAxis) {
		if (stick != nullptr && stick->GetAxisCount() > axisChannel) {
			return stick->GetRawAxis(axisChannel);
		}
	} else if (src == kAnalog && analog != nullptr) {
		// Convert analog input to a value -1 to 1
		auto a = ghLib::Interpolate(average ? analog->GetAverageValue() : analog->GetValue(), 0, 4095, -1.0f, 1.0f);
		return a;
	} else if (src == kVirtual && otherAxis != nullptr) {
		return otherAxis->Get();
	}
	return 0.0f;
}

/**
 * Get the value of the axis, applying coerce, invert, deadband, and interpolation
 * @return Value of the Axis
 */
float Axis::Get() const {
	float in = ghLib::Coerce(GetRaw(), input.min, input.max);
	float out = (invert ? -1 : 1) * ghLib::Deadband(in, deadband);
	if (scale) {
		out = ghLib::Interpolate(out, input.min, input.max, output.min, output.max);
	}
	return out;
}

/**
 * Enables casting straight to float
 * @return Value of the Axis
 */
Axis::operator float() const {
	return Get();
}

}
