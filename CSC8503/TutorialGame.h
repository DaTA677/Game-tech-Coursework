#pragma once
#include "GameTechRenderer.h"
#ifdef USEVULKAN
#include "GameTechVulkanRenderer.h"
#endif
#include "PhysicsSystem.h"

#include "StateGameObject.h"
#include "EnemyGameObject.h"
#include "PlayerStateObject.h"
#include "BonusStateObject.h"
#include "ObstacleStateObject.h"
#include "DoorBehaviourObject.h"
#include "WinStateObject.h"

namespace NCL {
	namespace CSC8503 {
		class TutorialGame		{
		public:
			TutorialGame();
			~TutorialGame();

			void UpdateUI(float dt);

			virtual void UpdateGame(float dt);

			void CheckState();

		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			void InitWorld();

			/*
			These are some of the world/object creation functions I created when testing the functionality
			in the module. Feel free to mess around with them to see different objects being created in different
			test scenarios (constraints, collision types, and so on). 
			*/
			void InitGameExamples();

			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);

			void InitDefaultFloor();

			void InitSampleGameWorld();
			void InitGoatGameWorld();

			void InitGameWorldFromFile(const std::string& filename);

			ObstacleStateObject* AddSmallSquareObstacle(const Vector3& pos);

			ObstacleStateObject* AddLargeSquareObstacle(const Vector3& pos);

			void BridgeConstraintTest();

			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();

			void MoveMainPlayer();
			void CreateMainPlayer();
			void UpdateMainPlayerMovement(float dt);

			void ClearStates();

			void UpdateDebugState();
			void UpdateDebugKeys();

			void SetMainPlayer(PlayerStateObject* g) { mainPlayer = g; }
			GameObject* GetMainPlayer() const { return mainPlayer; }


			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddFloorToWorld(const Vector3& position, int size);
			GameObject* AddSpringToWorld(const Vector3& position, int size);
			WinStateObject* AddWinZoneToWorld(const Vector3& position, int size);
			BonusStateObject* AddHealingZoneToWorld(const Vector3& position, int size);
			BonusStateObject* AddMudZoneToWorld(const Vector3& position, int size);
			BonusStateObject* AddIceZoneToWorld(const Vector3& position, int size);
			BonusStateObject* AddLavaZoneToWorld(const Vector3& position, int size);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f, bool isHollow=false);
			GameObject* AddWallToWorld(const Vector3& position, int size);
			DoorBehaviourObject* AddDoorToWorld(const Vector3& position, int size);
			EnemyGameObject* AddEnemyStateToWorld(const Vector3& position);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);

			GameObject* AddPlayerToWorld(const Vector3& position);
			PlayerStateObject* AddPlayerStateToWorld(const Vector3& position);
			GameObject* AddEnemyToWorld(const Vector3& position);
			GameObject* AddBonusToWorld(const Vector3& position);

			StateGameObject* AddStateObjectToWorld(const Vector3& position);
			BonusStateObject* AddSpeedBonusToWorld(const Vector3& position);
			WinStateObject* AddKeyToWorld(const Vector3& position);
			StateGameObject* testStateObject;
			
			StateGameObject* AddPatrolStateObjectToWorld(const Vector3& position);

#ifdef USEVULKAN
			GameTechVulkanRenderer*	renderer;
#else
			GameTechRenderer* renderer;
#endif
			PhysicsSystem*		physics;
			GameWorld*			world;

			bool useGravity;
			bool inSelectionMode;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;

			MeshGeometry*	capsuleMesh = nullptr;
			MeshGeometry*	cubeMesh	= nullptr;
			MeshGeometry*	sphereMesh	= nullptr;

			TextureBase*	basicTex	= nullptr;
			ShaderBase*		basicShader = nullptr;

			//Coursework Meshes
			MeshGeometry*	charMesh	= nullptr;
			MeshGeometry*	enemyMesh	= nullptr;
			MeshGeometry*	bonusMesh	= nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
			Vector3 lockedOffset		= Vector3(0, 14, 20);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}

			GameObject* objClosest = nullptr;

			PlayerStateObject* mainPlayer = nullptr;
			float mainPlayerSpeed = 25.0f;
			bool moveMainPlayer = false;

			int gravityDirection;
			float linearDamping;


			int nodeSize;
			int gridWidth;
			int gridHeight;

			float gridOffsetx;
			float gridOffsety;

			float downTime = 0.0f;

			float score=0.0f;

			vector<StateGameObject*> states;

			int itemCount=0;
			int currentScene = 0;
			int maxScene = 6;
			bool mainPlayerOnGround = true;
			bool gameEnd=false;
			bool debug = false;
		};
	}
}

