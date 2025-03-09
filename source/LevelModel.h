//
//  LevelModel.h
//  SweetSweetBetrayal
//
//  Created by Benjamin Isaacson on 3/8/25
//

#ifndef __LEVEL_MODEL_H__
#define __LEVEL_MODEL_H__
#include <cugl/cugl.h>
#include <unordered_set>
#include <vector>
#include "Object.h"
#include "Platform.h"
#include "Spike.h"
#include "WindObstacle.h"
#include "Treasure.h"

class LevelModel {
public: 
	shared_ptr<JsonValue> createJsonObject(map<std::string, double> dict);

	template <typename T>
	shared_ptr <JsonValue> createJsonObjectList(string name, vector<shared_ptr<T>> objects);

	void createJsonFromLevel(Size size, vector<shared_ptr<Platform>> platforms, vector<shared_ptr<Spike>> spikes,
		vector<shared_ptr<Treasure>> treasures, vector<shared_ptr<WindObstacle>> windObstacles);

	void createJsonFromLevel(Size size, vector<shared_ptr<Object>> objects);
};

#endif /* __LEVEL_MODEL_H__ */