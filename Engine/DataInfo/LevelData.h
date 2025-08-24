#pragma once
#include <string>
#include <vector>

#include "Math/Vector3.h"


/// ===LevelData=== ///
struct LevelData {

	/// ===JSONObjectData=== ///
	struct JsonObjectData {
		std::string fileName;
		Vector3 translation;
		Vector3 rotation;
		Vector3 scaling;
	};

	// vector配列
	std::vector<JsonObjectData> objects;
};