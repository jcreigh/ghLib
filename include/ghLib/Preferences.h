/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include <string>
#include <future>
#include <fstream>
#include <sstream>
#include "networktables/NetworkTable.h"
#include "ghLib/ini.h"

namespace ghLib {


class Preferences : public ITableListener {
public:
	static Preferences *GetInstance();
	static bool IsKeyAcceptable(const std::string& value);
	void ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew);

	void Load();
	void Save();
	void UpdateNT(std::string key);

	void Put(std::string key, std::string value);
	std::string GetString(std::string key, std::string defaultValue = "");
	void PutString(std::string key, std::string value);
	int GetInt(std::string key, int defaultValue = 0);
	void PutInt(std::string key, int value);
	float GetFloat(std::string key, float defaultValue = 0.0);
	void PutFloat(std::string key, float value);
	double GetDouble(std::string key, double defaultValue = 0.0);
	void PutDouble(std::string key, double value);
	double GetBoolean(std::string key, bool defaultValue = false);
	void PutBoolean(std::string key, bool value);
	bool ContainsKey(std::string key);
	//void Remove(std::string key);

private:
	Preferences();
	void LoadTask();
	void SaveTask(std::string data);
	INI::File ini;
	ITable* prefTable;
	std::mutex fileMutex;
	static Preferences* instance;

};

}

