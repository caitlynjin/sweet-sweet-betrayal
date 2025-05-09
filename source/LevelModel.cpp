#include "LevelModel.h"
#include "ArtObject.h"

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
		if (name == "artObjects") {
			if (std::find(xOffsetArtObjects.begin(), xOffsetArtObjects.end(), (*it)->getJsonType()) != xOffsetArtObjects.end()) {
				//(*it)->setPosition(Vec2((*it)->getPosition().x - 32, (*it)->getPosition().y));

			}
			if (std::find(yOffsetArtObjects.begin(), yOffsetArtObjects.end(), (*it)->getJsonType()) != yOffsetArtObjects.end()) {
				//(*it)->setPosition(Vec2((*it)->getPosition().x, (*it)->getPosition().y - 32));
			}
		}
		innerArray->appendChild(createJsonObject(objData));
		if (name == "artObjects") {
			if (std::find(xOffsetArtObjects.begin(), xOffsetArtObjects.end(), (*it)->getJsonType()) != xOffsetArtObjects.end()) {
				//(*it)->setPosition(Vec2((*it)->getPosition().x + 32, (*it)->getPosition().y));

			}
			if (std::find(yOffsetArtObjects.begin(), yOffsetArtObjects.end(), (*it)->getJsonType()) != yOffsetArtObjects.end()) {
				//(*it)->setPosition(Vec2((*it)->getPosition().x, (*it)->getPosition().y + 32));
			}
		}
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

void LevelModel::createJsonFromLevel(string fileName, Size levelSize, vector<shared_ptr<Platform>>& platforms, vector<shared_ptr<Spike>>& spikes,
	vector<shared_ptr<Treasure>>& treasures, vector<shared_ptr<WindObstacle>>& windObstacles, vector<shared_ptr<Tile>>& tiles,
	vector<shared_ptr<ArtObject>>& artObjects) {
	shared_ptr<JsonValue> json = JsonValue::allocObject();
	shared_ptr<JsonValue> innerArray = JsonValue::allocArray();
	json->appendValue("width", double(levelSize.getIWidth()));
	json->appendValue("height", double(levelSize.getIHeight()));
	json->appendArray("objects");
	innerArray->appendChild(createJsonObjectList("platforms", platforms));
	innerArray->appendChild(createJsonObjectList("tiles", tiles));
	innerArray->appendChild(createJsonObjectList("spikes", spikes));
	innerArray->appendChild(createJsonObjectList("treasures", treasures));
	innerArray->appendChild(createJsonObjectList("windObstacles", windObstacles));
	innerArray->appendChild(createJsonObjectList("artObjects", artObjects));
	json->appendChild("objectTypes", innerArray);

	shared_ptr<JsonWriter> jsonWriter = JsonWriter::alloc(fileName);
	jsonWriter->writeJson(json);
	jsonWriter->close();
}

void LevelModel::createJsonFromLevel(string fileName, Size levelSize, vector<shared_ptr<Object>>* objects) {
	vector<shared_ptr<Platform>> platforms;
	vector<shared_ptr<Tile>> tiles;
	vector<shared_ptr<Spike>> spikes;
	vector<shared_ptr<WindObstacle>> windObstacles;
	vector<shared_ptr<Treasure>> treasures;
	vector<shared_ptr<ArtObject>> artObjects;
	string key;
	for (auto it = (*objects).begin(); it != (*objects).end(); ++it) {
		key = (*it)->getJsonKey();
		// Apparently you can't use a switch statement with strings in C++...
		if (key == "platforms") {
			platforms.push_back(dynamic_pointer_cast<Platform>(*it));
		}
		else if (key == "tiles") {
			tiles.push_back(dynamic_pointer_cast<Tile>(*it));
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
		else if (key == "artObjects") {
			auto artObj = dynamic_pointer_cast<ArtObject>(*it);
			artObjects.push_back(artObj);
		}
	}
	createJsonFromLevel(fileName, levelSize, platforms, spikes, treasures, windObstacles, tiles, artObjects);
}

/**
* Creates a level and returns the objects within it.
* 
* These objects have NOT been added to the physics world, and need to be added there after this method is called for them to show up.
* @param fileName The name of the JSON file containing the level information
*/
vector<shared_ptr<Object>> LevelModel::createLevelFromJson(string fileName, bool useAbsolutePath) {
	vector<shared_ptr<Object>> allLevelObjects;
	shared_ptr<JsonReader> jsonReader;
	if (useAbsolutePath) { // for the load button in the level editor
		jsonReader = JsonReader::alloc(fileName);
	}
	else { // this happens everywhere else
		jsonReader = JsonReader::allocWithAsset(fileName);
	}
	if (jsonReader == nullptr) {
		return allLevelObjects;
	}
	shared_ptr<JsonValue> json = jsonReader->readJson();
	vector<shared_ptr<JsonValue>> objectTypes = json->get("objectTypes")->children();
	string name;

	_levelSize = Size(
		json->get("width")->asFloat(),
		json->get("height")->asFloat()
	);

	for (auto it = objectTypes.begin(); it != objectTypes.end(); ++it) {
		vector<shared_ptr<JsonValue>> objects = (*it)->get("objects")->children();
		for (auto it2 = objects.begin(); it2 != objects.end(); ++it2) {
			if ((*it)->get("name")->_stringValue == string("platforms")) {
				Size theSize = Size((*it2)->get("width")->asFloat(), (*it2)->get("height")->asFloat());
				allLevelObjects.push_back(Platform::alloc(
					Vec2((*it2)->get("x")->asFloat(), (*it2)->get("y")->asFloat()),
					theSize,
					(*it2)->get("type")->asString()
				));
			}
			else if ((*it)->get("name")->_stringValue == string("tiles")) {
				Size theSize = Size((*it2)->get("width")->asFloat(), (*it2)->get("height")->asFloat());
				allLevelObjects.push_back(Tile::alloc(
					Vec2((*it2)->get("x")->asFloat(), (*it2)->get("y")->asFloat()),
					theSize,
					(*it2)->get("type")->asString(), _scale
				));
			}
			else if ((*it)->get("name")->_stringValue == string("spikes")) {
				allLevelObjects.push_back(Spike::alloc(
					Vec2((*it2)->get("x")->asFloat(), (*it2)->get("y")->asFloat()),
					Size((*it2)->get("width")->asFloat(), (*it2)->get("height")->asFloat()),
					(*it2)->get("scale")->asFloat(),
					(*it2)->get("angle")->asFloat(),
					(*it2)->get("type")->asString()
				));
			}
			else if ((*it)->get("name")->_stringValue == string("treasures")) {
				allLevelObjects.push_back(Treasure::alloc(
					Vec2((*it2)->get("x")->asFloat(), (*it2)->get("y")->asFloat()),
					Size((*it2)->get("width")->asFloat(), (*it2)->get("height")->asFloat()),
					(*it2)->get("scale")->asFloat(),
					(*it2)->get("type")->asString()
				));
			}
			else if ((*it)->get("name")->_stringValue == string("windObstacles")) {

				allLevelObjects.push_back(WindObstacle::alloc(
					Vec2((*it2)->get("x")->asFloat(), (*it2)->get("y")->asFloat()),
					Size((*it2)->get("width")->asFloat(), (*it2)->get("height")->asFloat()), (*it2)->get("scale")->asFloat(),
					Vec2((*it2)->get("gustDirX")->asFloat(), (*it2)->get("gustDirY")->asFloat()), Vec2(0, 3.0f),0,
					(*it2)->get("type")->asString()
				));
			}
			else if ((*it)->get("name")->_stringValue == string("artObjects")) {
				Vec2 pos = Vec2((*it2)->get("x")->asFloat(), (*it2)->get("y")->asFloat());
				if (std::find(xOffsetArtObjects.begin(), xOffsetArtObjects.end(), (*it2)->get("type")->asString()) != xOffsetArtObjects.end()) {
					pos.x -= 0.5;

				}
				if (std::find(yOffsetArtObjects.begin(), yOffsetArtObjects.end(), (*it2)->get("type")->asString()) != yOffsetArtObjects.end()) {
					pos.y -= 0.5;

				}
				auto artObj = ArtObject::alloc(
					pos,
					Size((*it2)->get("width")->asFloat(), (*it2)->get("height")->asFloat()),
					(*it2)->get("scale")->asFloat(),
					(*it2)->get("angle")->asFloat(),
					(*it2)->get("layer")->asInt(),
					(*it2)->get("type")->asString()
				);

				allLevelObjects.push_back(artObj);
			}
		}
	}
	// These objects have NOT been added to the physics world.
	_objects = allLevelObjects;
	jsonReader->close();
	return allLevelObjects;
}
