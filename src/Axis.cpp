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
 * Constructor for a Axis to load from Preferences
 * <p><b>Axis Preferences</b><br>
 * foo = channel. Either axis number, axis number, or pov direction (int)<br>
 * foo.js = joystick (int)<br>
 * </p>
 * @param axisConfig The configuration key for the axis
 */
Axis::Axis(std::string axisConfig) {
	auto pref = NetworkTable::GetTable("Preferences");
	auto logger = ghLib::Logger::getLogger("Axis");
	config = axisConfig;

	if (!pref->ContainsKey(axisConfig)) {
		logger->error("Attempting to load config '" + axisConfig + "' and could not find it");
		return;
	} else {
		axisChannel = (int)pref->GetNumber(axisConfig, 0);
		std::string typeStr = pref->GetString((axisConfig + ".type"), "axis");
		if (typeStr == "axis") {
			auto stickNum = (int)pref->GetNumber(axisConfig + ".js", 0); // Default to joystick 0
			stick = ghLib::Joystick::GetStickForPort(stickNum);
			type = kAxis;
		} else if (typeStr == "analog") {
			analog = new ghLib::AnalogInput(axisChannel);
			average = pref->GetBoolean(axisConfig + ".average", false);
			type = kAnalog;
		} else if (typeStr == "virtual") {
			otherAxis = FindAxis(pref->GetString(axisConfig + ".virtual", ""));
			type = kVirtual;
		}

		input.min = (float)pref->GetNumber(axisConfig + ".input.min", -1.0f);
		input.max = (float)pref->GetNumber(axisConfig + ".input.max", 1.0f);
		output.min = (float)pref->GetNumber(axisConfig + ".output.min", -1.0f);
		output.max = (float)pref->GetNumber(axisConfig + ".output.max", 1.0f);
		scale = pref->GetBoolean(axisConfig + ".scale", false);
		invert = pref->GetBoolean(axisConfig + ".invert", false);
		deadband = (float)pref->GetNumber(axisConfig + ".deadband", 0.0f);

	}

	axesMutex.lock();
	axes.push_back(this); // Add new axis
	axesMutex.unlock();
}

/**
 * Deconstructor for a Axis
 */
Axis::~Axis() {
}

/**
 * Search for an Axis by key
 * @param key Configuration key to search by
 */
Axis* Axis::FindAxis(std::string key) {
	for (auto axis : axes) {
		if (axis->config == key) {
			return axis;
		}
	}
	return nullptr;
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

float Axis::GetRaw() const {
	if (type == ChannelType::kAxis) {
		if (stick != nullptr && stick->GetAxisCount() > axisChannel) {
			return stick->GetRawAxis(axisChannel);
		}
	} else if (type == kAnalog && analog != nullptr) {
		// Convert analog input to a value -1 to 1
		auto a = ghLib::Interpolate(average ? analog->GetAverageValue() : analog->GetValue(), 0, 4095, -1.0f, 1.0f);
		return a;
	} else if (type == kVirtual && otherAxis != nullptr) {
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
