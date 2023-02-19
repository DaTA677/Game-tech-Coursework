#pragma once
#include "Vector3.h"
#include "GameObject.h"
#include <vector>

namespace NCL {
	namespace CSC8503 {
		class LevelGenerator {
			public:
				static GameObject* AddWall(Vector3 pos);
				static GameObject* AddFloor(Vector3 dim);
				static GameObject* AddEnemy(Vector3 pos);
				static GameObject* AddPlayer(Vector3 pos);
				static GameObject* AddBridge(Vector3 pos);
				static GameObject* AddBonus(Vector3 pos);
				static GameObject* AddObstacle(Vector3 pos);

		protected:
			static float WallDimension;
			static float FloorDimension;
		};

	}
}
