/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include "networktables2/server/NetworkTableServer.h"
#include "ghLib/DriverStation.h"
#include "ghLib/Preferences.h"

namespace ghLib {

#ifndef GHLIB_PREF_FILENAME
#define GHLIB_PREF_FILENAME "/home/lvuser/preferences.ini"
#endif

static const char *kTableName = "Preferences";
static const char *kSaveField = "~S A V E~";
static const char *kFileName = GHLIB_PREF_FILENAME;

Preferences *Preferences::instance = nullptr;

Preferences *Preferences::GetInstance() {
	if (instance == nullptr) {
		instance = new Preferences();
	}
	return instance;
}

bool Preferences::IsKeyAcceptable(const std::string& value) {
	std::string forbidden = "=\n\r \t[]\"";
	for (char c : forbidden) {
		if (value.find(c) != std::string::npos) {
			return false;
		}
	}
	return true;
}

void Preferences::ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew) {
	if (key == kSaveField && prefTable->GetBoolean(kSaveField)) {
		Save();
	} else if (IsKeyAcceptable(key)) {
		std::string value = prefTable->GetString(key);
		ini.setString(key, value);
	}
}

Preferences::Preferences() {
	prefTable = NetworkTable::GetTable(kTableName);
	prefTable->AddTableListener(this, true);
	prefTable->PutBoolean(kSaveField, false);
	ini = INI::File();
	Load();
}

void Preferences::LoadTask()  {
	fileMutex.lock();
	std::ifstream t(kFileName);
	std::stringstream buffer;
	buffer << t.rdbuf();
	std::string data = buffer.str();
	t.close();
	fileMutex.unlock();
	ini.load(data.c_str(), data.size());
}

void Preferences::SaveTask(std::string data) {
	fileMutex.lock();
	//printf("Saving... Data\n---\n%s\n---\n", data.c_str());
	std::ofstream t(kFileName);
	t.write(data.c_str(), data.size());
	t.close();
	fileMutex.unlock();
}

void Preferences::Load() {
	std::async(std::launch::async, &Preferences::LoadTask, this);
}

void Preferences::Save() {
	std::string data = ini.save(); // ini isn't very thread safe, so don't call it from there
	std::async(std::launch::async, &Preferences::SaveTask, this, data);
}

void Preferences::UpdateNT(std::string key) {
	prefTable->PutString(key, GetString(key));
}

void Preferences::Put(std::string key, std::string value) {
	PutString(key, value);
	UpdateNT(key);
}

std::string Preferences::GetString(std::string key, std::string defaultValue /* = "" */) {
	return ini.getString(key, defaultValue);
}

void Preferences::PutString(std::string key, std::string value) {
	ini.setString(key, value);
	UpdateNT(key);
}

int Preferences::GetInt(std::string key, int defaultValue /* = 0 */) {
	return ini.getInt32(key, defaultValue);
}

void Preferences::PutInt(std::string key, int value) {
	ini.setInt32(key, value);
	UpdateNT(key);
}

float Preferences::GetFloat(std::string key, float defaultValue /* = 0.0 */) {
	return ini.getFloat(key, defaultValue);
}

void Preferences::PutFloat(std::string key, float value) {
	ini.setFloat(key, value);
	UpdateNT(key);
}

double Preferences::GetDouble(std::string key, double defaultValue /* = 0.0 */) {
	return ini.getDouble(key, defaultValue);
}

void Preferences::PutDouble(std::string key, double value) {
	ini.setDouble(key, value);
	UpdateNT(key);
}

double Preferences::GetBoolean(std::string key, bool defaultValue /* = false */) {
	return ini.getBool(key, defaultValue);
}

void Preferences::PutBoolean(std::string key, bool value) {
	ini.setBool(key, value);
	UpdateNT(key);
}

bool Preferences::ContainsKey(std::string key) {
	return ini.has(key);
}

//void Preferences::Remove(std::string key)
	// TODO: Add Remove functionality, maybe
//}

}
