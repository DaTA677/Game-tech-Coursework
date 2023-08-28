#include "TutorialGame.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TextureLoader.h"
#include "Assets.h"

#include "PositionConstraint.h"
#include "OrientationConstraint.h"
#include "StateGameObject.h"
#include <string>
#include <NavigationGrid.h>
#include <fstream>
#include "EnemyGameObject.h"
#include "StateGameObject.h"
#include "PlayerStateObject.h"
#include"BonusStateObject.h"
#include "ObstacleStateObject.h"
#include "BehaviourGameObject.h"
#include "DoorBehaviourObject.h"
#include "WinStateObject.h"

using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame()	{
	world		= new GameWorld();
#ifdef USEVULKAN
	renderer	= new GameTechVulkanRenderer(*world);
#else 
	renderer = new GameTechRenderer(*world);
#endif

	physics		= new PhysicsSystem(*world);

	forceMagnitude	= 10.0f;
	useGravity		= false;
	inSelectionMode = false;

	gravityDirection = 1;
	linearDamping = physics->GetLinearDamping();


	InitialiseAssets();
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes, 
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::InitialiseAssets() {
	cubeMesh	= renderer->LoadMesh("cube.msh");
	sphereMesh	= renderer->LoadMesh("sphere.msh");
	charMesh	= renderer->LoadMesh("goat.msh");
	enemyMesh	= renderer->LoadMesh("Keeper.msh");
	bonusMesh	= renderer->LoadMesh("apple.msh");
	capsuleMesh = renderer->LoadMesh("capsule.msh");

	basicTex	= renderer->LoadTexture("checkerboard.png");
	basicShader = renderer->LoadShader("scene.vert", "scene.frag");

	InitCamera();
	InitWorld();
}

TutorialGame::~TutorialGame()	{
	delete cubeMesh;
	delete sphereMesh;
	delete charMesh;
	delete enemyMesh;
	delete bonusMesh;

	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;
}

void TutorialGame::UpdateUI(float dt) {
	string s = "";
	switch (currentScene) {
	case 0:
		s = "Goat Imitator";
		Debug::Print(s, Vector2(50 - s.length(), 50-5), Vector4(0.75, 0.5, 0.25, 1));
		s = "[P]lay game";
		Debug::Print(s, Vector2(50 - s.length(), 50), Debug::WHITE);
		s = "[E]xit game";
		Debug::Print(s, Vector2(50 - s.length(), 50+5 ), Debug::WHITE);
		break;
	case 1:
		s = "Goat Imitator";
		Debug::Print(s, Vector2(50 - s.length(), 5), Vector4(0.75, 0.5, 0.25, 1));
		s = "W                ";
		Debug::Print(s, Vector2(50 - s.length(), 12), Debug::RED);
		s = "S                 ";
		Debug::Print(s, Vector2(50 - s.length(), 15), Debug::RED);
		s = "A             ";
		Debug::Print(s, Vector2(50 - s.length(), 18), Debug::RED);
		s = "D              ";
		Debug::Print(s, Vector2(50 - s.length(), 21), Debug::RED);
		s = "Space        ";
		Debug::Print(s, Vector2(50 - s.length(), 24), Debug::RED);
		s = "  to move forward";
		Debug::Print(s, Vector2(50 - s.length(), 12),Debug::WHITE);
		s = "  to move backward";
		Debug::Print(s, Vector2(50 - s.length(), 15), Debug::WHITE);
		s = "  to move left";
		Debug::Print(s, Vector2(50 - s.length(), 18), Debug::WHITE);
		s = "  to move right";
		Debug::Print(s, Vector2(50 - s.length(), 21), Debug::WHITE);
		s = "      to jump";
		Debug::Print(s, Vector2(50 - s.length(), 24), Debug::WHITE);
		s = "Move to Yellow square to go to next level";
		Debug::Print(s, Vector2(50 - s.length(), 27), Debug::WHITE);
		break;
	case 2:
		s = "Lava                     ";
		Debug::Print(s, Vector2(50 - s.length(), 12), Debug::RED);
		s = "Meadow                     ";
		Debug::Print(s, Vector2(50 - s.length(), 15), Debug::GREEN);
		s = "Bonus                     ";
		Debug::Print(s, Vector2(50 - s.length(), 18), Debug::BLUE);
		s = "Mud               ";
		Debug::Print(s, Vector2(50 - s.length(), 21), Vector4(0.58f, 0.29f, 0, 1));
		s = "Ice                     ";
		Debug::Print(s, Vector2(50 - s.length(), 24), Debug::CYAN);
		s = "Door                                      ";
		Debug::Print(s, Vector2(50 - s.length(), 27), Debug::BLACK);
		s = "Key                           ";
		Debug::Print(s, Vector2(50 - s.length(), 30), Debug::YELLOW);
		s = "     hurts your character";
		Debug::Print(s, Vector2(50 - s.length(), 12), Debug::WHITE);
		s = "       heals your character";
		Debug::Print(s, Vector2(50 - s.length(), 15), Debug::WHITE);
		s = "      increases your speed";
		Debug::Print(s, Vector2(50 - s.length(), 18), Debug::WHITE);
		s = "    slows you down";
		Debug::Print(s, Vector2(50 - s.length(), 21), Debug::WHITE);
		s = "    increases your speed";
		Debug::Print(s, Vector2(50 - s.length(), 24), Debug::WHITE);
		s = "     opens when certain conditions are met";
		Debug::Print(s, Vector2(50 - s.length(), 27), Debug::WHITE);
		s = "  if present opens your door";
		Debug::Print(s, Vector2(50 - s.length(), 30), Debug::WHITE);
		break;
	case 3:
		s = "Hitting obstacles destroys them and gets you points";
		Debug::Print(s, Vector2(50 - s.length(), 18), Debug::WHITE);
		s = "Clear Obstacles to open door and reach exit";
		Debug::Print(s, Vector2(50 - s.length(), 21), Debug::WHITE);
		break;
	case 4:
		s = "Enemies flee when seeing you";
		Debug::Print(s, Vector2(50 - s.length(), 18), Debug::WHITE);
		s = "Get more points if enemies clear obstacles";
		Debug::Print(s, Vector2(50 - s.length(), 21), Debug::WHITE);
		break;
	}
}

void TutorialGame::UpdateGame(float dt) {

	UpdateUI(dt);
	if (currentScene != 0) {
		if ((world->GetTrigger() & 1) == 1) {//bit 0 to check move to next level
			currentScene++;
			if (currentScene == maxScene) {
				world->ToggleTriggerbit(4);// bit 4 to check win game
				world->ToggleTriggerbit(0);
			}
			else {
				InitWorld();
				selectionObject = nullptr;
			}				
		}
		int trigger = world->GetTrigger();
		if (mainPlayer->GetHealth() <= 0.0f && ((trigger & 2) != 2)) {
			world->ToggleTriggerbit(1);// bit 1 to lose game
		}
		
		if (((trigger & 16) == 16) || ((trigger & 2)== 2)) {
			string s = "";
			if ((world->GetTrigger() & 16) != 0) {
				s = "You Win";
				Debug::Print(s, Vector2(50 - s.length(), 50 - 10));
			}
			else {
				s = "You Lose";
				Debug::Print(s, Vector2(50 - s.length(), 50 - 10));
			}
			s = "Score:" + std::to_string(score);
			Debug::Print(s, Vector2(50 - s.length(), 50 - 5));
			s = "Replay?";
			Debug::Print(s, Vector2(50 - s.length(), 50));
			s = "[Y]es   [N]o";
			Debug::Print(s, Vector2(50 - s.length(), 50 + 5));

			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Y)) {
				currentScene = 0;
				InitWorld();
				selectionObject = nullptr;
			}
			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::N)) {
				exit(0);
			}
			
			
		}
		else {
			if (itemCount == 0) {
				world->ToggleTriggerbit(3);// bit 3 for item check
				itemCount = -1;
			}

			if (itemCount != -1) {
				Debug::Print("Items left:" + std::to_string(itemCount), Vector2(5, 15), Debug::WHITE);
			}

			if (!inSelectionMode) {
				world->GetMainCamera()->UpdateCamera(dt);
			}

			if (testStateObject) testStateObject->Update(dt);

			if (states.size() > 0) {
				auto iter = states.begin();
				for (; iter != states.end(); iter++) {
					(*iter)->Update(dt);
				}
			}

			if (moveMainPlayer) {
				int cameraDist = 15;
				float yaw = world->GetMainCamera()->GetYaw();

				Vector3 localPos = mainPlayer->GetTransform().GetPosition();
				/* camera x and z caluclations based on https://www.gamedev.net/articles/programming/general-and-gameplay-programming/a-simple-third-person-camera-r1591/#:~:text=Here%27s%20some%20code%20that%20calculates%20the%20camera%27s%20position%3A,-%20270.0f%29%20%2A%20M_PI%20%2F%20180%29%20%2B%20zpos%3B */
				float camerax = cameraDist * cos((yaw + 270) * 3.14f / 180) + localPos.x;
				float cameray = cameraDist * sin((world->GetMainCamera()->GetPitch()) * 3.14f / 180);
				float cameraz = cameraDist * sin((yaw - 270) * 3.14f / 180) + localPos.z;

				world->GetMainCamera()->SetPosition(Vector3(camerax, -cameray + localPos.y, cameraz));

				float currentHealth = mainPlayer->GetHealth();
				float maxHealth = mainPlayer->GetMaxHealth();

				Debug::Print("Health:" + std::to_string(mainPlayer->GetHealth()), Vector2(5, 5), Vector4(1, currentHealth / maxHealth, currentHealth / maxHealth, 1));
				Debug::Print("Score:" + std::to_string(mainPlayer->GetScore() + score), Vector2(5, 10), Debug::YELLOW);
				//Matrix4 vM = Matrix4::BuildViewMatrix(camPos, mainPlayer->GetTransform().GetPosition(), Vector3(0, 1, 0));
				//Quaternion q(vM.Inverse());
				;

			}

			UpdateKeys();
			UpdateMainPlayerMovement(dt);

			if (debug) {
				UpdateDebugKeys();
				UpdateDebugState();
				
			}
			world->UpdateWorld(dt);
			renderer->Update(dt);
			physics->Update(dt);

			CheckState();
		}		
	}

	else {
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P)) {
			currentScene++;
			InitWorld();
			selectionObject = nullptr;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::E)) {
			exit(0);
		}
	}
	

	renderer->Render();
	Debug::UpdateRenderables(dt);
}

void TutorialGame::UpdateDebugState() {
	if (lockedObject != nullptr) {
		Vector3 objPos = lockedObject->GetTransform().GetPosition();
		Vector3 camPos = objPos + lockedOffset;

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0, 1, 0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!

		world->GetMainCamera()->SetPosition(camPos);
		world->GetMainCamera()->SetPitch(angles.x);
		world->GetMainCamera()->SetYaw(angles.y);
	}

	Debug::Print("world trigger:" + std::to_string(world->GetTrigger()), Vector2(5, 25));

	if (useGravity) {
		Debug::Print("(G)ravity on", Vector2(5, 95), Debug::RED);
	}
	else {
		Debug::Print("(G)ravity off", Vector2(5, 95), Debug::RED);
	}
	switch (gravityDirection) {
	case 1:
		Debug::Print("Gravity direction: DOWN", Vector2(5, 80), Debug::RED);
		break;
	case 2:
		Debug::Print("Gravity direction: UP", Vector2(5, 80), Debug::RED);
		break;
	case 3:
		Debug::Print("Gravity direction: LEFT", Vector2(5, 80), Debug::RED);
		break;
	case 4:
		Debug::Print("Gravity direction: RIGHT", Vector2(5, 80), Debug::RED);
		break;
	}

	Debug::Print("Linear Damping:" + std::to_string(linearDamping), Vector2(5, 75), Debug::RED);


	RayCollision closestCollision;
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::K) && selectionObject) {
		Vector3 rayPos;
		Vector3 rayDir;

		rayDir = selectionObject->GetTransform().GetOrientation() * Vector3(0, 0, -1);//Identify front vector

		rayPos = selectionObject->GetTransform().GetPosition();

		Ray r = Ray(rayPos, rayDir);

		if (world->Raycast(r, closestCollision, true, selectionObject, selectionObject->GetIgnoreLayer())) {
			if (objClosest) {
				objClosest->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
			}
			objClosest = (GameObject*)closestCollision.node;

			objClosest->GetRenderObject()->SetColour(Vector4(1, 0, 1, 1));
			Debug::DrawLine(rayPos, closestCollision.collidedAt, Vector4(1, 0, 0, 1), 10);
		}
	}

	Debug::DrawLine(Vector3(), Vector3(0, 100, 0), Vector4(1, 0, 0, 1));
	SelectObject();
	MoveSelectedObject();
}

void NCL::CSC8503::TutorialGame::UpdateDebugKeys()
{
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM7)) {
		physics->SetGravity(Vector3(0.0f, 9.8f, 0.0f));
		gravityDirection = 2;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM8)) {
		physics->SetGravity(Vector3(-9.8f, 0.0f, 0.0f));
		gravityDirection = 3;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM9)) {
		physics->SetGravity(Vector3(0.0f, -9.8f, 0.0f));
		gravityDirection = 1;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM0)) {
		physics->SetGravity(Vector3(9.8f, 0.0f, 0.0f));
		gravityDirection = 4;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::MINUS)) {
		linearDamping -= 0.01;
		physics->SetLinearDamping(linearDamping);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PLUS)) {
		linearDamping += 0.01;
		physics->SetLinearDamping(linearDamping);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F3)) {
		MoveMainPlayer(); //F3 will toggle the camera and player movements
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM4)) {
		world->SetTrigger(4);

	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM5)) {
		world->SetTrigger(8);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM3)) {
		world->SetTrigger(0);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) {
		useGravity = !useGravity; //Toggle gravity!
		physics->UseGravity(useGravity);
	}

	if (lockedObject) {
		LockedObjectMovement();
	}
	else {
		DebugObjectMovement();
	}
}

void TutorialGame::CheckState() {
	vector<StateGameObject*>temp;
	for (int i = 0; i < states.size(); i++) {
		if ((!states[i]->IsActive() || states[i]->ToDelete()) && !states[i]->IsAdded()) {
			score += states[i]->GetScore();
			if (states[i]->GetObjectType() == ObjectId::OBSTACLE) itemCount--;
			states[i]->SetAdded(true);
		}
		if(states[i]->ToDelete()){
			StateGameObject* s = states[i];
			world->RemoveGameObject(s,true);
			
		}
		else {
			temp.emplace_back(states[i]);
		}
	}
	states=temp;
}

void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
	}	
	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7)) {
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8)) {
		world->ShuffleObjects(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F6)) {
		debug = !debug;
	}
	
}

void TutorialGame::LockedObjectMovement() {
	Matrix4 view		= world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld	= view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();


	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
		selectionObject->GetPhysicsObject()->AddForce(fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
		selectionObject->GetPhysicsObject()->AddForce(-fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NEXT)) {
		selectionObject->GetPhysicsObject()->AddForce(Vector3(0,-10,0));
	}
}

void TutorialGame::DebugObjectMovement() {
//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject) {
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM7)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM8)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM5)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}
	}
}

void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
	lockedObject = nullptr;
}

void TutorialGame::ClearStates() {
	states.clear();
}

void TutorialGame::InitWorld() {
	if (mainPlayer && (world->GetTrigger()& 1) !=0) score+=mainPlayer->GetScore();
	world->ClearAndErase();
	world->SetTrigger(0);
	physics->Clear();
	ClearStates();

	switch (currentScene) {
	case 1:InitGameWorldFromFile("Tutorial1.txt");
		break;
	case 2:InitGameWorldFromFile("Tutorial2.txt");
		break;
	case 3:InitGameWorldFromFile("Tutorial3.txt");
		break;
	case 4:InitGameWorldFromFile("Tutorial4.txt");
		break;
	case 5:InitGameWorldFromFile("TestGrid2.txt");
		break;
		
	}
	//testStateObject = AddPatrolStateObjectToWorld(Vector3(10, 10, 10));
	//InitDefaultFloor();
	//InitMixedGridWorld(15, 15, 3.5f, 3.5f);

	//InitGameExamples();
	//BridgeConstraintTest();
	//InitDefaultFloor();
	//InitSampleGameWorld();
	//InitGoatGameWorld();
	//AddWallToWorld(Vector3(0, 0, 0),1);
	//InitGameWorldFromFile("TestGrid2.txt");
}

void TutorialGame::MoveMainPlayer() {
	moveMainPlayer = !moveMainPlayer;
	world->GetMainCamera()->MoveCamera();
}

void TutorialGame::InitGoatGameWorld() {
	mainPlayer = AddPlayerStateToWorld(Vector3(0, 0, 0));
	world->GetMainCamera()->SetPosition(mainPlayer->GetTransform().GetPosition() + Vector3(-5, 15, 20));
	if (!moveMainPlayer) MoveMainPlayer();
	mainPlayer->GetTransform().SetOrientation(Quaternion(Matrix4::Rotation(world->GetMainCamera()->GetYaw(),Vector3(0,1,0))));
	InitDefaultFloor();
	AddCubeToWorld(Vector3(0, 0, -10), Vector3(2, 2, 2), 0.1f);
	testStateObject = AddPatrolStateObjectToWorld(Vector3(10, 10, 10));
	

}

void TutorialGame::InitGameWorldFromFile(const std::string& filename) {

	std::ifstream infile(Assets::DATADIR + filename);

	infile >> nodeSize;
	infile >> gridWidth;
	infile >> gridHeight;

	itemCount = 0;
	//allNodes = new GridNode[gridWidth * gridHeight];

	for (int y = 0; y < gridHeight; ++y) {
		for (int x = 0; x < gridWidth; ++x) {
			//GridNode& n = allNodes[(gridWidth * y) + x];
			char type = 0;
			infile >> type;
			Vector3 pos = Vector3((float)((x+gridOffsetx) * nodeSize), 0, (float)((y+gridOffsety) * nodeSize));
			if (type == 'x') {
				AddWallToWorld(pos, nodeSize / 2);
				continue;
			}
			if (type == 's') {
				GameObject* w = AddWallToWorld(pos, nodeSize / 2);
				w->SetIgnoreLayer(2);
				AddFloorToWorld(pos, nodeSize / 2);
				
				continue;
			}
			if (type == '.') {
				AddFloorToWorld(pos, nodeSize / 2);
				continue;
			}
			if (type == 'p') {
				mainPlayer=AddPlayerStateToWorld(pos);
				if (!moveMainPlayer) MoveMainPlayer();
				mainPlayer->GetTransform().SetOrientation(Quaternion(Matrix4::Rotation(world->GetMainCamera()->GetYaw(), Vector3(0, 1, 0))));
				AddFloorToWorld(pos, nodeSize / 2);
				states.emplace_back(mainPlayer);
				continue;
			} 
			if (type == 'e') {
				states.emplace_back(AddEnemyStateToWorld(pos));
				AddFloorToWorld(pos, nodeSize / 2);
				continue;
			} 
			if (type == 'b') {
				states.emplace_back(AddSpeedBonusToWorld(pos));
				AddFloorToWorld(pos, nodeSize / 2);
				continue;
			}
			if (type == 'l') {
				AddLavaZoneToWorld(pos,nodeSize/2);
				continue;
			}
			if (type == 'h') {
				AddHealingZoneToWorld(pos, nodeSize / 2);
				continue;
			}
			if (type == 'q') {
				states.emplace_back(AddSmallSquareObstacle(pos));
				AddFloorToWorld(pos, nodeSize / 2);
				itemCount++;
				continue;
			}
			if (type == 'a') {
				states.emplace_back(AddLargeSquareObstacle(pos));
				AddFloorToWorld(pos, nodeSize / 2);
				itemCount++;
				continue;
			}
			if (type == 'm') {
				AddMudZoneToWorld(pos, nodeSize / 2);
				continue;
			}
			if (type == 'i') {
				AddIceZoneToWorld(pos, nodeSize / 2);
				continue;
			}
			if (type == 'd') {
				states.emplace_back(AddDoorToWorld(pos, nodeSize / 2));
				AddFloorToWorld(pos,nodeSize/2);
				continue;
			}
			if (type == 'k') {
				AddKeyToWorld(pos);
				AddFloorToWorld(pos, nodeSize / 2);
				continue;
			}
			if (type == 'w') {
				AddWinZoneToWorld(pos, nodeSize / 2);
				continue;
			}
			if (type == 'j') {
				GameObject* w = AddWallToWorld(pos, nodeSize / 2);
				w->SetIgnoreLayer(2);
				AddKeyToWorld(pos);
				AddFloorToWorld(pos, nodeSize / 2);

				continue;
			}if (type == 'r') {
				AddSpringToWorld(pos, nodeSize / 2);

				continue;
			}
		}
	}
	itemCount = itemCount == 0 ? -1 : itemCount;
	useGravity = true;
	physics->UseGravity(useGravity);
	infile.close();
}

ObstacleStateObject* TutorialGame::AddSmallSquareObstacle(const Vector3& pos) {
	ObstacleStateObject* cube = new ObstacleStateObject("Cube");

	AABBVolume* volume = new AABBVolume(Vector3(1,1,1));
	cube->SetBoundingVolume((CollisionVolume*)volume);


	cube->GetTransform()
		.SetPosition(pos)
		.SetScale(Vector3(1, 1, 1) * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->GetRenderObject()->SetColour(Vector4(0.5f, 0.5f, 0.0f, 1.0f));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(0.0009f);
	cube->GetPhysicsObject()->InitCubeInertia();

	cube->SetLayer(1);
	cube->SetIgnoreLayer(4);
	cube->SetScore(150);

	world->AddGameObject(cube);

	return cube;
}
ObstacleStateObject* TutorialGame::AddLargeSquareObstacle(const Vector3& pos) {
	ObstacleStateObject* cube = new ObstacleStateObject("Cube");
	Vector3 dimensions = Vector3(2, 6, 2);
	AABBVolume* volume = new AABBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);


	cube->GetTransform()
		.SetPosition(pos)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->GetRenderObject()->SetColour(Vector4(0.5f, 0.5f, 0.0f, 1.0f));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(0.00009f);
	cube->GetPhysicsObject()->InitCubeInertia();

	cube->SetLayer(1);
	cube->SetIgnoreLayer(4);
	cube->SetScore(500);
	cube->SetHealth(100);

	world->AddGameObject(cube);

	return cube;
}

void TutorialGame::UpdateMainPlayerMovement(float dt) {
	
	if (moveMainPlayer ) {
		Matrix4 directionMatrix = Matrix4::Rotation(world->GetMainCamera()->GetYaw(), Vector3(0, 1, 0));
		
		if (!(mainPlayer->IsIdle())) {
			mainPlayer->GetTransform().SetOrientation(Quaternion::Slerp(mainPlayer->GetTransform().GetOrientation(), Quaternion(directionMatrix), mainPlayerSpeed * dt));
		}
		//world->GetMainCamera()->SetPosition(mainPlayer->GetTransform().GetPosition() + Vector3(-5, 15, 20));
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
			
			Vector3 forward = directionMatrix * Vector3(0, 0, -1);
			forward.Normalise();
			//forward.y = 0;

			//Vector3 goatForward = mainPlayer->GetTransform().GetOrientation() * Vector3(0, 0, -1);
			//goatForward.Normalise();
			//goatForward.y = 0;

			//Vector3 angles = mainPlayer->GetTransform().GetOrientation().ToEuler();
			//float yaw = angles.y;
			//if (yaw < 0) {
			//	yaw += 360.0f;
			//}
			//if (yaw > 360.0f) {
			//	yaw -= 360.0f;
			//}
			//yaw= yaw - world->GetMainCamera()->GetYaw();

			//selectionObject->GetPhysicsObject()->AddForce(forward * 100);
			/*Vector3 force = (direction)-selectionObject->GetTransform().GetOrientation() * Vector3(0, 0, -1);
			force.y = 0;*/

			/*selectionObject->GetPhysicsObject()->ApplyAngularImpulse(force);*/
			mainPlayer->GetPhysicsObject()->ApplyLinearImpulse(forward * mainPlayerSpeed*dt);
			//mainPlayer->GetPhysicsObject()->AddTorque(goatForward-forward*(yaw<0?-1:1));
			
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
			Vector3 forward = directionMatrix * Vector3(-1, 0, 0);
			forward.Normalise();
			forward.y = 0;
			mainPlayer->GetPhysicsObject()->ApplyLinearImpulse(forward * mainPlayerSpeed * dt);
			
			//selectionObject->GetPhysicsObject()->AddForce(selectionObject->GetPhysicsObject()->GetLeftVector() * 100);
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
			
			Vector3 forward = directionMatrix * Vector3(1, 0, 0);
			forward.Normalise();
			forward.y = 0;
			mainPlayer->GetPhysicsObject()->ApplyLinearImpulse(forward * mainPlayerSpeed * dt);
			
			//selectionObject->GetPhysicsObject()->AddForce(selectionObject->GetPhysicsObject()->GetRightVector() * 100);
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
			
			Vector3 forward = directionMatrix * Vector3(0, 0, 1);
			forward.Normalise();
			forward.y = 0;
			mainPlayer->GetPhysicsObject()->ApplyLinearImpulse(forward * mainPlayerSpeed * dt);
			
			//selectionObject->GetPhysicsObject()->AddForce(selectionObject->GetPhysicsObject()->GetBackwardVector() * 100);
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
			//Player Ray casting
			Ray downward = Ray(mainPlayer->GetTransform().GetPosition(), Vector3(0, -1, 0));
			RayCollision coll;
			if (world->Raycast(downward, coll, true, mainPlayer)) {
				if (coll.rayDistance < 3) mainPlayerOnGround = true;
				else mainPlayerOnGround = false;
			}
			if(mainPlayerOnGround)
			mainPlayer->GetPhysicsObject()->ApplyLinearImpulse(mainPlayer->GetPhysicsObject()->GetUpVector() * 20);
		}

		/*if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUMPAD1)) {
			selectionObject->GetPhysicsObject()->AddForce(selectionObject->GetPhysicsObject()->GetDownVector() * 100);
		}*/
	}
}

void TutorialGame::BridgeConstraintTest() {
	Vector3 cubeSize = Vector3(8, 5, 5);

	float invCubeMass = 5;
	int numLinks = 5;
	int maxDistance = 25;
	float cubeDistance = 20;

	Vector3 startPos = Vector3(0, 100, 0);

	GameObject* start = AddCubeToWorld(startPos + Vector3(0, 0, 0), cubeSize, 0);
	GameObject* end = AddCubeToWorld(startPos + Vector3((numLinks+1)* cubeDistance, 0, 0), cubeSize, 0);
	end->GetRenderObject()->SetColour(Vector4(1, 1, 0, 1));

	GameObject* previous = start;

	for (int i = 0; i < numLinks; i++) {
		GameObject* block = AddCubeToWorld(startPos + Vector3((i + 1) * cubeDistance, 0, 0), cubeSize, invCubeMass);
		PositionConstraint* constraint = new PositionConstraint(previous, block, maxDistance);
		world->AddConstraint(constraint);
		previous = block;
	}

	PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
	world->AddConstraint(constraint);
}

/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject();

	floor->SetName("Floor");
	Vector3 floorSize = Vector3(512, 2, 512);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	floor->SetLayer(1);

	world->AddGameObject(floor);

	return floor;
}

GameObject* TutorialGame::AddFloorToWorld(const Vector3& position, int size) {
	GameObject* floor = new GameObject();

	floor->SetName("Floor");
	Vector3 floorSize = Vector3(size, 2, size);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position+Vector3(0,-(size/2), 0));

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	floor->SetLayer(1);
	floor->SetObjectType(ObjectId::WORLD);
	world->AddGameObject(floor);

	return floor;
}


GameObject* TutorialGame::AddSpringToWorld(const Vector3& position, int size) {
	GameObject* floor = new GameObject();

	floor->SetName("Spring");
	Vector3 floorSize = Vector3(size, 2, size);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position + Vector3(0, -(size / 2), 0));

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->GetRenderObject()->SetColour(Debug::MAGENTA);
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	floor->SetLayer(1);
	floor->SetObjectType(ObjectId::SPRING);
	world->AddGameObject(floor);

	return floor;
}

WinStateObject* TutorialGame::AddWinZoneToWorld(const Vector3& position, int size) {
	WinStateObject* floor = new WinStateObject();
	floor->SetWorld(world);
	floor->SetName("Win Zone");
	Vector3 floorSize = Vector3(size, 2, size);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position + Vector3(0, -(size / 2), 0));

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->GetRenderObject()->SetColour(Debug::YELLOW);
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	floor->SetLayer(1);
	world->AddGameObject(floor);

	return floor;
}



BonusStateObject* TutorialGame::AddHealingZoneToWorld(const Vector3& position, int size) {
	BonusStateObject* floor = new BonusStateObject();

	floor->SetName("Healing Floor");
	Vector3 floorSize = Vector3(size, 2, size);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position + Vector3(0, -(size / 2), 0));

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	floor->SetLayer(1);
	floor->SetObjectType(ObjectId::HEALTHBONUS);
	floor->SetBonusValue(0.5);

	world->AddGameObject(floor);

	return floor;
}
BonusStateObject* TutorialGame::AddMudZoneToWorld(const Vector3& position, int size) {
	BonusStateObject* floor = new BonusStateObject();

	floor->SetName("Mud");
	Vector3 floorSize = Vector3(size, 2, size);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position + Vector3(0, -(size / 2), 0));

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->GetRenderObject()->SetColour(Vector4(0.58f, 0.29f, 0, 1));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	floor->SetLayer(1);
	floor->SetObjectType(ObjectId::SPEEDREDUCTION);
	floor->SetBonusValue(0.1f);

	world->AddGameObject(floor);

	return floor;
}
BonusStateObject* TutorialGame::AddIceZoneToWorld(const Vector3& position, int size) {
	BonusStateObject* floor = new BonusStateObject();

	floor->SetName("Ice");
	Vector3 floorSize = Vector3(size, 2, size);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position + Vector3(0, -(size / 2), 0));

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->GetRenderObject()->SetColour(Vector4(0.0f, 0.5f, 0.5f, 1));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	floor->SetLayer(1);
	floor->SetObjectType(ObjectId::SPEEDBONUS);
	floor->SetBonusValue(0.05f);

	world->AddGameObject(floor);

	return floor;
}

BonusStateObject* TutorialGame::AddLavaZoneToWorld(const Vector3& position, int size) {
	BonusStateObject* floor = new BonusStateObject();

	floor->SetName("Healing Floor");
	Vector3 floorSize = Vector3(size, 2, size);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position + Vector3(0, -(size / 2), 0));

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	floor->SetLayer(1);
	floor->SetObjectType(ObjectId::HEALTHREDUCTION);
	floor->SetBonusValue(0.5);

	world->AddGameObject(floor);

	return floor;
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass, bool isHollow) {
	GameObject* sphere = new GameObject();

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	if (!isHollow) {
		sphere->GetPhysicsObject()->InitSphereInertia();
		sphere->SetName(string("Sphere"));
		sphere->GetPhysicsObject()->SetElasticity(0.6f);
		sphere->GetPhysicsObject()->SetInverseMass(inverseMass*0.1);
	}
	else {
		sphere->GetPhysicsObject()->InitHollowSphereInertia();
		sphere->SetName(string("Hollow Sphere"));
		sphere->GetPhysicsObject()->SetElasticity(0.9f);
		sphere->GetRenderObject()->SetColour(Vector4(1, 1, 0, 1));
	}

	sphere->SetLayer(8);
	sphere->SetIgnoreLayer(6);

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddWallToWorld(const Vector3& position, int size) {
	GameObject* wall = new GameObject("Wall");

	Vector3 wallSize = Vector3(1,3,1)*Vector3(size,1,size);
	CollisionVolume* volume;
	if (currentScene == 1) volume = (CollisionVolume*)new OBBVolume(wallSize);
	else  volume = (CollisionVolume*)new AABBVolume(wallSize);
	wall->SetBoundingVolume((CollisionVolume*)volume);

	wall->GetTransform()
		.SetScale(wallSize*2)
		.SetPosition(position);

	wall->SetRenderObject(new RenderObject(&wall->GetTransform(), cubeMesh, basicTex, basicShader));
	wall->GetRenderObject()->SetColour(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
	wall->SetPhysicsObject(new PhysicsObject(&wall->GetTransform(), wall->GetBoundingVolume()));
	wall->GetPhysicsObject()->InitCubeInertia();

	wall->GetPhysicsObject()->SetInverseMass(0);


	wall->SetLayer(1);
	wall->SetIgnoreLayer(128);

	world->AddGameObject(wall);

	return wall;
}

DoorBehaviourObject* TutorialGame::AddDoorToWorld(const Vector3& position, int size) {
	DoorBehaviourObject* wall = new DoorBehaviourObject("Door");
	
	Vector3 wallSize = Vector3(1, 3, 1) * Vector3(size, 1, size);
	AABBVolume* volume = new AABBVolume(wallSize);
	wall->SetBoundingVolume((CollisionVolume*)volume);

	wall->GetTransform()
		.SetScale(wallSize * 2)
		.SetPosition(position);

	wall->SetRenderObject(new RenderObject(&wall->GetTransform(), cubeMesh, basicTex, basicShader));
	wall->GetRenderObject()->SetColour(Vector4(0,0,0, 1));
	wall->SetPhysicsObject(new PhysicsObject(&wall->GetTransform(), wall->GetBoundingVolume()));
	wall->GetPhysicsObject()->InitCubeInertia();

	wall->GetPhysicsObject()->SetInverseMass(0.00009f);

	GameObject* door = new GameObject("Door open location");
	door->GetTransform().SetScale(Vector3(0.5f, 0.5f, 0.5f) * 2).SetPosition(position + Vector3(0, size * 2, 0));
	door->SetPhysicsObject(new PhysicsObject(&door->GetTransform(), (CollisionVolume*)(new AABBVolume(Vector3(0.5f, 0.5f, 0.5f)))));
	door->GetPhysicsObject()->InitCubeInertia();
	door->GetPhysicsObject()->SetInverseMass(0);
	
	wall->SetOpenNode(door);
	PositionConstraint* posCon = new PositionConstraint(door, wall, 5);
	wall->SetOpenNodeConstraint(posCon);

	wall->SetLayer(1);
	wall->SetIgnoreLayer(128);

	wall->SetGameWorld(world);
	world->AddGameObject(wall);
	world->AddGameObject(door);

	return wall;
}

EnemyGameObject* TutorialGame::AddEnemyStateToWorld(const Vector3& position) {
	EnemyGameObject* enemy = new EnemyGameObject();
	float meshSize = 3.0f;
	float inverseMass = 0.5f;

	enemy->SetName("Enemy");
	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.7f, 0.3f) * meshSize);
	enemy->SetBoundingVolume((CollisionVolume*)volume);

	enemy->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	enemy->SetRenderObject(new RenderObject(&enemy->GetTransform(), enemyMesh, nullptr, basicShader));
	enemy->SetPhysicsObject(new PhysicsObject(&enemy->GetTransform(), enemy->GetBoundingVolume()));

	enemy->GetPhysicsObject()->SetInverseMass(inverseMass);
	enemy->GetPhysicsObject()->InitSphereInertia();

	enemy->SetLayer(8);
	enemy->SetIgnoreLayer(128);

	enemy->AddPatrolNode(position);
	enemy->AddPatrolNode(position + Vector3(-20, 0, 0));
	enemy->AddPatrolNode(position + Vector3(-20, 0, -5));
	enemy->AddPatrolNode(position + Vector3(0, 0, -5));

	if(mainPlayer)	enemy->SetPlayer(mainPlayer);

	enemy->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));

	world->AddGameObject(enemy);
	return enemy;
}


GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject("Cube");

	AABBVolume* volume = new AABBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	
	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	cube->SetLayer(4);
	cube->SetIgnoreLayer(4);

	world->AddGameObject(cube);

	return cube;
}

GameObject* TutorialGame::AddPlayerToWorld(const Vector3& position) {
	float meshSize		= 1.0f;
	float inverseMass	= 0.5f;

	GameObject* character = new GameObject("Player");
	SphereVolume* volume  = new SphereVolume(1.0f);
	//AABBVolume* volume = new AABBVolume(Vector3(1, 1, 1));
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), charMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	character->SetLayer(2);
	character->SetIgnoreLayer(128);


	world->AddGameObject(character);

	return character;
}

PlayerStateObject* TutorialGame::AddPlayerStateToWorld(const Vector3& position) {
	float meshSize = 1.0f;
	float inverseMass = 0.5f;

	PlayerStateObject* character = new PlayerStateObject("Player");
	SphereVolume* volume = new SphereVolume(1.0f);
	//AABBVolume* volume = new AABBVolume(Vector3(1, 1, 1));
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), charMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	character->SetLayer(2);
	character->SetIgnoreLayer(128);

	character->GetRenderObject()->SetColour(Vector4(0, 0, 1, 1));
	character->SetMaxHealth(10);
	character->SetHealth(10.0f);
	character->SetScore(0.0f);

	world->AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddEnemyToWorld(const Vector3& position) {
	float meshSize		= 3.0f;
	float inverseMass	= 0.5f;

	GameObject* character = new GameObject("Enemy");

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	character->SetLayer(8);
	character->SetIgnoreLayer(4);

	world->AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddBonusToWorld(const Vector3& position) {
	GameObject* apple = new GameObject("Bonus");

	SphereVolume* volume = new SphereVolume(0.5f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(2, 2, 2))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), bonusMesh, nullptr, basicShader));
	apple->GetRenderObject()->SetColour(Debug::BLUE);
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	apple->SetIgnoreLayer(1);

	world->AddGameObject(apple);

	return apple;
}

StateGameObject* TutorialGame::AddStateObjectToWorld(const Vector3& position)
{
	StateGameObject* apple = new StateGameObject();

	SphereVolume* volume = new SphereVolume(2);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(2, 2, 2))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), sphereMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	apple->SetIgnoreLayer(128);

	world->AddGameObject(apple);

	return apple;
}
BonusStateObject* TutorialGame::AddSpeedBonusToWorld(const Vector3& position)
{
	BonusStateObject* apple = new BonusStateObject();

	apple->SetName("Speed");

	SphereVolume* volume = new SphereVolume(2);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(2, 2, 2))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), sphereMesh, nullptr, basicShader));
	apple->GetRenderObject()->SetColour(Debug::BLUE);
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();
	apple->SetLayer(4);
	apple->SetIgnoreLayer(128);
	apple->SetObjectType(ObjectId::SPEEDBONUS);
	apple->SetBonusValue(0.5);
	apple->SetScore(100);


	world->AddGameObject(apple);

	return apple;
}

WinStateObject* TutorialGame::AddKeyToWorld(const Vector3& position)
{
	WinStateObject* apple = new WinStateObject();
	apple->SetWorld(world);
	apple->SetName("Key");

	SphereVolume* volume = new SphereVolume(2);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(2, 2, 2))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), sphereMesh, nullptr, basicShader));
	apple->GetRenderObject()->SetColour(Debug::YELLOW);
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();
	apple->SetLayer(4);
	apple->SetIgnoreLayer(128);
	apple->SetObjectType(ObjectId::KEY);

	world->AddGameObject(apple);

	return apple;
}

StateGameObject* TutorialGame::AddPatrolStateObjectToWorld(const Vector3& position)
{

	StateGameObject* apple = new StateGameObject();

	SphereVolume* volume = new SphereVolume(2);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(2, 2, 2))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), sphereMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	apple->SetIgnoreLayer(128);

	apple->AddPatrolNode(position);
	apple->AddPatrolNode(Vector3(30, 0, 0));
	apple->AddPatrolNode(Vector3(30, 0, 30));
	apple->AddPatrolNode(Vector3(0, 0, 30));

	Debug::DrawLine(Vector3(30, 0, 0), Vector3(30, 0, 0) + Vector3(0, 1, 0), Debug::CYAN, 1000.0f);
	Debug::DrawLine(Vector3(30, 0, 30), Vector3(30, 0, 30) + Vector3(0, 1, 0), Debug::CYAN, 1000.0f);
	Debug::DrawLine(Vector3(0, 0, 30), Vector3(0, 0, 30) + Vector3(0, 1, 0), Debug::CYAN, 1000.0f);

	apple->GenerateStates();

	world->AddGameObject(apple);

	return apple;
}

void TutorialGame::InitSampleGameWorld() {
	AddFloorToWorld(Vector3(0, -20, 0));
	AddCubeToWorld(Vector3(0, 20, 0), Vector3(1, 0.5, 0.7), 0.04);
	AddCubeToWorld(Vector3(5, 20, 0), Vector3(1, 0.5, 1), 0.4);

	AddSphereToWorld(Vector3(5, 15, 0), 1.0f, 0.4, true);
	AddSphereToWorld(Vector3(0, 15, 0), 1.0f, 0.4, false);

	AddPlayerToWorld(Vector3(0, 0, 0));
	AddEnemyToWorld(Vector3(0, 0, 5));
}

void TutorialGame::InitDefaultFloor() {
	AddFloorToWorld(Vector3(0, -20, 0));
}

void TutorialGame::InitGameExamples() {
	AddPlayerToWorld(Vector3(0, 5, 0));
	AddEnemyToWorld(Vector3(5, 5, 0));
	AddBonusToWorld(Vector3(10, 5, 0));
}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(position, radius, 1.0f);
		}
	}
	AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);

			if (rand() % 2) {
				AddCubeToWorld(position, cubeDims);
			}
			else {
				AddSphereToWorld(position, sphereRadius,10.0f, (rand()%2==0?true:false));
			}
		}
	}
}

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 1; x < numCols+1; ++x) {
		for (int z = 1; z < numRows+1; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, 1.0f);
		}
	}
}

/*
Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be 
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around. 

*/
bool TutorialGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) {
		Debug::Print("Press Q to change to camera mode!", Vector2(5, 85));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT)) {
			if (selectionObject) {	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;
				Debug::DrawLine(ray.GetPosition(), closestCollision.collidedAt, Vector4(1, 1, 1, 1), 100.0f);
				selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				return true;
			}
			else {
				return false;
			}
		}
		if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L)) {
			if (selectionObject) {
				if (lockedObject == selectionObject) {
					lockedObject = nullptr;
				}
				else {
					lockedObject = selectionObject;
				}
			}
		}
	}
	else {
		Debug::Print("Press Q to change to select mode!", Vector2(5, 85));
	}
	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/

void TutorialGame::MoveSelectedObject() {
	Debug::Print("Click Force:" + std::to_string(forceMagnitude), Vector2(5, 90));
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject) {
		return;//we haven't selected anything!
	}
	//Push the selected object!
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

		RayCollision closestCollision;
		if (world->Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
			}
		}
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUMPAD3)) {
		Matrix4 directionMatrix = Matrix4::Rotation(world->GetMainCamera()->GetYaw(), Vector3(0, 1, 0));
		Vector3 forward = directionMatrix*Vector3(0,0,-1);
		forward.Normalise();
		forward.y = 0;
		
		//selectionObject->GetPhysicsObject()->AddForce(forward * 100);
		/*Vector3 force = (direction)-selectionObject->GetTransform().GetOrientation() * Vector3(0, 0, -1);
		force.y = 0;*/

		/*selectionObject->GetPhysicsObject()->ApplyAngularImpulse(force);*/
		selectionObject->GetPhysicsObject()->ApplyLinearImpulse(forward * 100);
		selectionObject->GetTransform().SetOrientation(Quaternion::Lerp(selectionObject->GetTransform().GetOrientation(),Quaternion(directionMatrix),1));
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUMPAD4)) {
		Vector3 direction = Matrix4::Rotation(world->GetMainCamera()->GetYaw(), Vector3(0, 1, 0)) * Matrix4::Rotation(world->GetMainCamera()->GetPitch(), Vector3(-1, 0, 0)) * Vector3(-1, 0, 0);
		direction.Normalise();
		direction.y = 0;
		Vector3 force = (direction)-selectionObject->GetTransform().GetOrientation() * Vector3(0, 0, -1);
		force.y = 0;
		selectionObject->GetPhysicsObject()->ApplyAngularImpulse(force);
		selectionObject->GetPhysicsObject()->ApplyLinearImpulse(direction * 100);
		//selectionObject->GetPhysicsObject()->AddForce(selectionObject->GetPhysicsObject()->GetLeftVector() * 100);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUMPAD6)) {
		Vector3 direction = Matrix4::Rotation(world->GetMainCamera()->GetYaw(), Vector3(0, 1, 0)) * Matrix4::Rotation(world->GetMainCamera()->GetPitch(), Vector3(1, 0, 0)) * Vector3(1, 0, 0);
		direction.Normalise();
		direction.y = 0;
		Vector3 force = (direction)-selectionObject->GetTransform().GetOrientation() * Vector3(0, 0, -1);
		force.y = 0;
		selectionObject->GetPhysicsObject()->ApplyAngularImpulse(force);
		selectionObject->GetPhysicsObject()->ApplyLinearImpulse(direction * 100);
		//selectionObject->GetPhysicsObject()->AddForce(selectionObject->GetPhysicsObject()->GetRightVector() * 100);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUMPAD2)) {
		Vector3 direction = Matrix4::Rotation(world->GetMainCamera()->GetYaw(), Vector3(0, 1, 0)) * Matrix4::Rotation(world->GetMainCamera()->GetPitch(), Vector3(0, 0, 1)) * Vector3(0, 0, 1);
		direction.Normalise();
		direction.y = 0;
		Vector3 force = (direction)-selectionObject->GetTransform().GetOrientation() * Vector3(0, 0, -1);
		force.y = 0;
		selectionObject->GetPhysicsObject()->ApplyAngularImpulse(force);
		selectionObject->GetPhysicsObject()->ApplyLinearImpulse(direction * 100);
		//selectionObject->GetPhysicsObject()->AddForce(selectionObject->GetPhysicsObject()->GetBackwardVector() * 100);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUMPAD9)) {

		selectionObject->GetPhysicsObject()->AddForce(selectionObject->GetPhysicsObject()->GetUpVector() * 100);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUMPAD1)) {

		selectionObject->GetPhysicsObject()->AddForce(selectionObject->GetPhysicsObject()->GetDownVector() * 100);
	}
}


