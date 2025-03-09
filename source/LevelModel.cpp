#include "LevelModel.h"

template <typename T>
shared_ptr<JsonValue> LevelModel::createJsonObjectList(string name, vector<shared_ptr<T>>& objects) {
	shared_ptr<JsonValue> json = JsonValue::allocObject();
	shared_ptr<JsonValue> innerArray = JsonValue::allocArray();
	map<string, any> objData;
	if (objects.size () == 0) {
		json->appendValue("name", std::string("emptyObjectList"));
	}
	else {
		json->appendValue("name", name);
	}
	for (auto it = objects.begin(); it != objects.end(); ++it) {
		objData = (*it)->getMap();
		innerArray->appendChild(createJsonObject(objData));
	}
	json->appendChild("objects", innerArray);
	return json;

}

shared_ptr<JsonValue> LevelModel::createJsonObject(map<std::string, std::any>& dict) {
	shared_ptr<JsonValue> json = JsonValue::allocObject();
	for (auto it = dict.begin(); it != dict.end(); ++it) {
		if ((it->second).type() == typeid(bool)) {
			json->appendValue(it->first, std::any_cast<bool>(it->second));
		}
		else if ((it->second).type() == typeid(std::string)) {
			json->appendValue(it->first, std::any_cast<std::string>(it->second));
		}
		else if ((it->second).type() == typeid(double)) {
			json->appendValue(it->first, std::any_cast<double>(it->second));
		}
		else if ((it->second).type() == typeid(long)) {
			json->appendValue(it->first, std::any_cast<long>(it->second));
		}
		// Add other types here if necessary
		
	}
	return json;
}

/** 
* This method is the primary method to call in order to generate a level.
* By passing it the level objects, it creates and saves a JSON level.
*/

void LevelModel::createJsonFromLevel(Size levelSize, vector<shared_ptr<Platform>>& platforms, vector<shared_ptr<Spike>>& spikes,
	vector<shared_ptr<Treasure>>& treasures, vector<shared_ptr<WindObstacle>>& windObstacles) {
	shared_ptr<JsonValue> json = JsonValue::allocObject();
	shared_ptr<JsonValue> innerArray = JsonValue::allocArray();
	json->appendValue("width", double(levelSize.getIWidth()));
	json->appendValue("height", double(levelSize.getIHeight()));
	//json->appendArray("objects");
	innerArray->appendChild(createJsonObjectList("platforms", platforms));
	innerArray->appendChild(createJsonObjectList("spikes", spikes));
	json->appendChild("objectTypes", innerArray);

	shared_ptr<JsonWriter> jsonWriter = JsonWriter::alloc("leveltest.json");
	jsonWriter->writeJson(json);
}

void LevelModel::createJsonFromLevel(Size levelSize, vector<shared_ptr<Object>>& objects) {
	vector<shared_ptr<Platform>> platforms;
	vector<shared_ptr<Spike>> spikes;
	vector<shared_ptr<WindObstacle>> windObstacles;
	vector<shared_ptr<Treasure>> treasures;
	string key;
	for (auto it = objects.begin(); it != objects.end(); ++it) {
		key = (*it)->getJsonKey();
		// Apparently you can't use a switch statement with strings in C++...
		if (key == "platforms") {
			platforms.push_back(dynamic_pointer_cast<Platform>(*it));
		}
		else if (key == "spikes") {
			spikes.push_back(dynamic_pointer_cast<Spike>(*it));
		}
		else if (key == "treasures") {
			treasures.push_back(dynamic_pointer_cast<Treasure>(*it));
		}
		else if (key == "windObstacles") {
			windObstacles.push_back(dynamic_pointer_cast<WindObstacle>(*it));
		}
	}
	createJsonFromLevel(levelSize, platforms, spikes, treasures, windObstacles);
}