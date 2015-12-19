#include "ghLib/Axis.h"

namespace ghLib {

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

	if (!pref->ContainsKey(axisConfig.c_str())) {
		logger->error("Attempting to load config '" + axisConfig + "' and could not find it");
		return;
	} else {
		axisChannel = (int)pref->GetNumber(axisConfig.c_str(), 0);
		auto stickNum = (int)pref->GetNumber((axisConfig + ".js").c_str(), 0); // Default to joystick 0
		stick = ghLib::Joystick::GetStickForPort(stickNum);

		invert = pref->GetBoolean((axisConfig + ".invert").c_str(), false);
		deadband = (float)pref->GetNumber((axisConfig + ".deadband"), 0.0f);

	}
}

/**
 * Deconstructor for a Axis
 */
Axis::~Axis() {
}

void Axis::SetDeadband(float newDeadband) {
	deadband = newDeadband;
}

float Axis::GetDeadband() {
	return deadband;
}

void Axis::SetInvert(bool newInvert) {
	invert = newInvert;
}

bool Axis::GetInvert() {
	return invert;
}

/**
 * Get the value of the axis, applying invert and deadband
 * @return Value of the Axis
 */
float Axis::Get() {
	//auto logger = ghLib::Logger::getLogger("Axis");
	//logger->trace(ghLib::Format("Axis %d: Stick: %p, Invert %s, Deadband %f, Raw %f", axisChannel, stick, invert ? "True" : "False", deadband, stick->GetRawAxis(axisChannel)));
	return (invert ? -1 : 1) * ghLib::Deadband(stick->GetRawAxis(axisChannel), deadband);
}

}
