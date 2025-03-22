//
//  LevelModel.h
//  SweetSweetBetrayal
//
//  Created by Benjamin Isaacson on 3/8/25
//

#ifndef __LEVEL_MODEL_H__
#define __LEVEL_MODEL_H__
#include <cugl/cugl.h>
#include <vector>
#include <any>
#include "Object.h"
#include "Platform.h"
#include "Spike.h"
#include "WindObstacle.h"
#include "Treasure.h"

class LevelModel {

private:
	Size _levelSize;
	vector<shared_ptr<Object>> _objects;

	
public: 

	shared_ptr<JsonValue> createJsonObject(map<std::string, std::any>& dict);

	template <typename T>
	shared_ptr <JsonValue> createJsonObjectList(string name, vector<shared_ptr<T>>& objects);

	void createJsonFromLevel(string fileName, Size size, vector<shared_ptr<Platform>>& platforms, vector<shared_ptr<Spike>>& spikes,
		vector<shared_ptr<Treasure>>& treasures, vector<shared_ptr<WindObstacle>>& windObstacles);

	/** Creates a JSON file based on an in-game level. 
	* @param size The size (width, height) of the level.
	* @param objects A list of all objects in the level.
	*/
	void createJsonFromLevel(string fileName, Size size, vector<shared_ptr<Object>>* objects);

	/** Initializes the in-game level based on the JSON file specifying it.
	* This method creates the objects in the level and defines its size.
	* Note that only one level can be stored in this class at a time.
	* If you call createLevelFromJson a second time, all the original level information will be overwritten.
	* @param fileName The name of the JSON file containing the level information.
	*/
	vector<shared_ptr<Object>> createLevelFromJson(string fileName);

	/** Returns the level size */
	Size getLevelSize() {
		return _levelSize;
	}

	/** Returns the object list. Note that these objects are NOT set up in the physics world. */
	vector<shared_ptr<Object>> getObjects() {
		return _objects;
	}
};

#endif /* __LEVEL_MODEL_H__ */