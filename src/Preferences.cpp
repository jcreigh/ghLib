/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include "ghLib/DriverStation.h"
#include "ghLib/Preferences.h"

#ifndef ROBOT

#include "networktables2/server/NetworkTableServer.h"

namespace ghLib {

Preferences *Preferences::instance = nullptr;
ITable* Preferences::prefTable;

Preferences::Preferences() {
	prefTable = NetworkTable::GetTable("Preferences");
	//class ServerListener : public ITableListener {
		//virtual void ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew){
			//fprintf(stdout, "Got key in Preferences: %s = %f\n", key.c_str(), value.f);
			//fflush(stdout);
		//};
	//};
	//auto listener = new ServerListener();
	//prefTable->AddTableListener(listener, true);
}

Preferences *Preferences::GetInstance() {
	if (instance == nullptr) {
		instance = new Preferences();
	}
	return instance;
}

void Preferences::Put(const char *key, std::string value) {
	prefTable->PutString(key, value);
}

std::string Preferences::GetString(const char *key, const char *defaultValue /* = "" */) {
	return prefTable->GetString(key, defaultValue);
}

void Preferences::PutString(const char *key, const char *value) {
	Put(key, value);
}

int Preferences::GetInt(const char *key, int defaultValue /* = 0 */) {
	if (!ContainsKey(key)) {
		return defaultValue; // Prevent possible lossy conversion to double
	}
	return static_cast<int>(prefTable->GetNumber(key));
}

void Preferences::PutInt(const char *key, int value) {
	prefTable->PutNumber(key, value);
}

float Preferences::GetFloat(const char *key, float defaultValue /* = 0.0 */) {
	if (!ContainsKey(key)) {
		return defaultValue; // Prevent possible lossy conversion to double
	}
	return static_cast<float>(prefTable->GetNumber(key));
}

void Preferences::PutFloat(const char *key, float value) {
	prefTable->PutNumber(key, (float)value);
}

double Preferences::GetDouble(const char *key, double defaultValue /* = 0.0 */) {
	return prefTable->GetNumber(key, defaultValue);
}

double Preferences::GetBoolean(const char *key, bool defaultValue /* = false */) {
	return prefTable->GetBoolean(key, defaultValue);
}

void Preferences::PutBoolean(const char *key, bool value) {
	prefTable->PutBoolean(key, value);
}

bool Preferences::ContainsKey(const char *key) {
	auto res = prefTable->ContainsKey(key);
	//printf("ContainsKey: %s = %s\n", key, res ? "true" : "false");
	return res;
}

}

#endif
