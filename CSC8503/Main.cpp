#include "Window.h"

#include "Debug.h"

#include "StateMachine.h"
#include "StateTransition.h"
#include "State.h"

#include "GameServer.h"
#include "GameClient.h"

#include "NavigationGrid.h"
#include "NavigationMesh.h"

#include "TutorialGame.h"
#include "NetworkedGame.h"

#include "PushdownMachine.h"

#include "PushdownState.h"

#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourAction.h"

using namespace NCL;
using namespace CSC8503;

#include <chrono>
#include <thread>
#include <sstream>

vector<Vector3> testNodes;
void TestPathfinding() {
	NavigationGrid grid("TestGrid1.txt");

	NavigationPath outPath;
	Vector3 startPos(160, 0, 20);
	Vector3 endPos(	160, 0, 160);

	bool found = grid.FindPath(startPos, endPos, outPath);

	Vector3 pos;
	while (outPath.PopWaypoint(pos)) {
		testNodes.push_back(pos);
	}
}

void DisplayPathfinding() {
	for (int i = 1; i < testNodes.size(); i++) {
		Vector3 a = testNodes[i - 1];
		Vector3 b = testNodes[i];

		Debug::DrawLine(a, b, Vector4(0, 1,0, 1));
	}
}


void TestStateMachine() {
	StateMachine* testMachine = new StateMachine();
	int data = 0;

	State* A = new State([&](float dt)->void {
		std::cout << "I'm in State A!\n";
		data++;
		}
	);

	State* B = new State([&](float dt)->void {
		std::cout << "I'm in state B\n";
		data--;
		}
	);

	StateTransition* stateAB = new StateTransition(A, B, [&](const StateTransition& s)->bool {
		return data > 10;
		}
	);

	StateTransition* stateBA = new StateTransition(B, A, [&](const StateTransition& s)->bool {
		return data < 0;
		}
	);

	testMachine->AddState(A);
	testMachine->AddState(B);
	testMachine->AddTransition(stateAB);
	testMachine->AddTransition(stateBA);

	for (int i = 0; i < 100; i++) {
		testMachine->Update(1.0f);
	}

}
void TestBehaviourTree() {
	float behaviourTimer;
	float distanceToTarget;
	BehaviourAction* findKey = new BehaviourAction("FindKey", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Looking for the key\n";
			behaviourTimer = rand() % 100;
			state = Ongoing;
		}
		else if(state==Ongoing) {
			behaviourTimer -= dt;
			if (behaviourTimer <= 0.0f) {
				std::cout << "Found the key!\n";
				return Success;
			}
		}
		return state;
	});
	BehaviourAction* goToRoom = new BehaviourAction("GoToRoom", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Going to Loot room\n";
			state = Ongoing;
		}
		else if(state==Ongoing) {
			distanceToTarget -= dt;
			if (distanceToTarget<= 0.0f) {
				std::cout << "Reached the room!\n";
				return Success;
			}
		}
		return state;
	});
	BehaviourAction* openDoor= new BehaviourAction("OpenDoor", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Opening door\n";
			state = Success;
		}
		return state;
	});
	BehaviourAction* findTreasure = new BehaviourAction("Look for Treasure", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Looking for treasure\n";
			state = Ongoing;
		}
		else if (state == Ongoing) {
			bool found = rand() % 2;
			if (found) {
				std::cout << "I have found some treasure!\n";
				return Success;
			}
			std::cout << "No treasure here\n";
			return Failure;
		}
		return state;
	});
	BehaviourAction* findItems = new BehaviourAction("Look for Items", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Looking for items\n";
			state = Ongoing;
		}
		else if (state == Ongoing) {
			bool found = rand() % 2;
			if (found) {
				std::cout << "I have found some items!\n";
				return Success;
			}
			std::cout << "No items in here\n";
			return Failure;
		}
		return state;
	});

	BehaviourSequence* sequence = new BehaviourSequence("Room sequence");
	sequence->AddChild(findKey);
	sequence->AddChild(goToRoom);
	sequence->AddChild(openDoor);

	BehaviourSelector* selection = new BehaviourSelector("Loot Selection");
	selection->AddChild(findTreasure);
	selection->AddChild(findItems);

	BehaviourSequence* rootSequence = new BehaviourSequence("Root Sequence");
	rootSequence->AddChild(sequence);
	rootSequence->AddChild(selection);

	for (int i = 0; i < 5; i++)
	{
		rootSequence->Reset();
		behaviourTimer = 0.0f;
		distanceToTarget = rand() % 251;
		BehaviourState state= Ongoing;
		while (state == Ongoing) {
			state = rootSequence->Execute(1.0f);
		}
		if (state == Success) {
			std::cout << "What a successful adventure\n";
		}
		else if (state == Failure) {
			std::cout << "What a waste of time\n";
		}
	}
	std::cout << "All done!\n";
}

class TestPacketReceiver : public PacketReceiver {
public:
	TestPacketReceiver(string name) {
		this->name = name;
	}

	void ReceivePacket(int type, GamePacket* payload, int source) {
		if (type == String_Message) {
			StringPacket* realPacket = (StringPacket*)payload;
			string msg = realPacket->GetStringFromData();
			std::cout << name << " received message: " << msg << std::endl;
		}
	}
protected:
	string name;
};

void TestNetworking() {
	NetworkBase::Initialise();
	TestPacketReceiver serverReceiver("Server");
	TestPacketReceiver clientReceiver("Client");

	int port = NetworkBase::GetDefaultPort();

	GameServer* server = new GameServer(port, 1);
	GameClient* client = new GameClient();

	server->RegisterPacketHandler(String_Message, &serverReceiver);
	client->RegisterPacketHandler(String_Message, &clientReceiver);

	bool canConnect = client->Connect(127, 0, 0, 1, port);

	for (int i = 0; i < 100; ++i) {

		GamePacket serverGamePacket = StringPacket("Server says hello! " + std::to_string(i));
		GamePacket clientGamePacket = StringPacket("Client says hello! " + std::to_string(i));
		server->SendGlobalPacket(serverGamePacket);

		client->SendPacket(clientGamePacket);

		server->UpdateServer();
		client->UpdateClient();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	NetworkBase::Destroy();
}


/*

The main function should look pretty familar to you!
We make a window, and then go into a while loop that repeatedly
runs our 'game' until we press escape. Instead of making a 'renderer'
and updating it, we instead make a whole game, and repeatedly update that,
instead. 

This time, we've added some extra functionality to the window class - we can
hide or show the 

*/
int main() {

	//TestBehaviourTree();
	Window*w = Window::CreateGameWindow("CSC8503 Game technology!", 1280, 720);

	if (!w->HasInitialised()) {
		return -1;
	}	

	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);
	
	//TestStateMachine();
	//TestPathfinding();
	//TestNetworking();
	
	TutorialGame* g = new TutorialGame();
	w->GetTimer()->GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!
	while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE)) {
		float dt = w->GetTimer()->GetTimeDeltaSeconds();
		if (dt > 0.1f) {
			std::cout << "Skipping large time delta" << std::endl;
			continue; //must have hit a breakpoint or something to have a 1 second frame time!
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR)) {
			w->ShowConsole(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT)) {
			w->ShowConsole(false);
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::T)) {
			w->SetWindowPosition(0, 0);
		}

		w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));
		DisplayPathfinding();
		g->UpdateGame(dt);
	}
	Window::DestroyGameWindow();
}