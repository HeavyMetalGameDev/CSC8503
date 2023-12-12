#include "Window.h"

#include "Debug.h"

#include "StateMachine.h"
#include "StateTransition.h"
#include "State.h"

#include "GameTechRenderer.h"

#include "GameServer.h"
#include "GameClient.h"

#include "NavigationGrid.h"
#include "NavigationMesh.h"

#include "TutorialGame.h"

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

class PauseScreen : public PushdownState {
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		if (Window::GetKeyboard()->KeyPressed(KeyCodes::P))return PushdownResult::Pop;
		return PushdownResult::NoChange;
	}
};
class SinglePlayerGameScreen : public PushdownState {
	public:
		SinglePlayerGameScreen(TutorialGame* t){
		g = t;
		g->InitWorldSinglePlayer();
	}

	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		if (dt > 0.1f) {
			std::cout << "Skipping large time delta" << std::endl;
			return PushdownResult::NoChange; //must have hit a breakpoint or something to have a 1 second frame time!
		}
		if (Window::GetKeyboard()->KeyPressed(KeyCodes::ESCAPE)) {
			return PushdownResult::Pop;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyCodes::P)) {
			Debug::Print("(P)aused", Vector2(40, 50));
			g->UpdateGame(dt); //do a game update so the text can display
			*newState = new PauseScreen();
			return PushdownResult::Push;
		}
		g->UpdateGame(dt);
		return PushdownResult::NoChange;
	};

protected:
	TutorialGame* g;
	std::vector<Vector3> testNodes;
};

class ClientGameScreen : public PushdownState {
public:
	ClientGameScreen(TutorialGame* t) {
		g = t;
		g->InitWorldClient();
	}

	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		if (Window::GetKeyboard()->KeyPressed(KeyCodes::ESCAPE)) {
			return PushdownResult::Pop;
		}
		g->UpdateGameAsClient(dt);
		return PushdownResult::NoChange;
	};

protected:
	TutorialGame* g;
	std::vector<Vector3> testNodes;
};

class ServerGameScreen : public PushdownState {
public:
	ServerGameScreen(TutorialGame* t) {
		g = t;
		g->InitWorldServer();
	}

	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		if (dt > 0.1f) {
			std::cout << "Skipping large time delta" << std::endl;
			return PushdownResult::NoChange; //must have hit a breakpoint or something to have a 1 second frame time!
		}
		if (Window::GetKeyboard()->KeyPressed(KeyCodes::ESCAPE)) {
			return PushdownResult::Pop;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyCodes::P)) {
			Debug::Print("(P)aused", Vector2(40, 50));
			g->UpdateGameAsServer(dt); //do a game update so the text can display
			*newState = new PauseScreen();
			return PushdownResult::Push;
		}
		g->UpdateGameAsServer(dt);
		return PushdownResult::NoChange;
	};

protected:
	TutorialGame* g;
	std::vector<Vector3> testNodes;
};


class MenuScreen : public PushdownState {
public:
	MenuScreen(TutorialGame* t) {
		g = t;
	}
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		Debug::Print("1: Start Game singleplayer", Vector2(40, 30));
		Debug::Print("2: Start Game as client", Vector2(40, 45));
		Debug::Print("3: Start Game as server", Vector2(40, 60));
		Debug::Print("ESCAPE: Exit Game", Vector2(40, 75));
		g->UpdateGame(0);
		if (Window::GetKeyboard()->KeyPressed(KeyCodes::NUM1)) {
			*newState = new SinglePlayerGameScreen(g);
			return PushdownResult::Push;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyCodes::NUM2)) {
			*newState = new ClientGameScreen(g);
			return PushdownResult::Push;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyCodes::NUM3)) {
			*newState = new ServerGameScreen(g);
			return PushdownResult::Push;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyCodes::ESCAPE)) {
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}
	void OnAwake() override {
		g->SetState(GameState::STATE_MENU);
	}
protected:
	TutorialGame* g;
};

void RunPushdownAutomata(Window* w) {
	TutorialGame* newGame = new TutorialGame();
	PushdownMachine machine(new MenuScreen(newGame));
	while (w->UpdateWindow()) {
		float dt = w->GetTimer().GetTimeDeltaSeconds();
		if (!machine.Update(dt))return;
	}
}
vector<Vector3> testNodes;
void TestPathfinding() {
	NavigationGrid grid("TestGrid1.txt");

	NavigationPath outPath;

	Vector3 startPos(80, 0, 10);
	Vector3 endPos(80, 0, 80);

	bool found = grid.FindPath(startPos, endPos, outPath);

	Vector3 pos;
	while (outPath.PopWaypoint(pos))testNodes.push_back(pos);
}



void TestStateMachine() {
	StateMachine* testMachine = new StateMachine();
	int data = 0;

	State* a = new State([&](float dt)->void {std::cout << "STATE A\n"; data++; });

	State* b = new State([&](float dt)->void {std::cout << "STATE B\n"; data--; });

	StateTransition* stateAB = new StateTransition(a, b, [&](void)->bool {return data > 10; });
	StateTransition * stateBA = new StateTransition(b, a, [&](void)->bool {return data < 0; });

	testMachine->AddState(a);
	testMachine->AddState(b);
	testMachine->AddTransition(stateAB);
	testMachine->AddTransition(stateBA);

	for (int i = 0; i < 100; i++)testMachine->Update(1.0f);
}

void TestBehaviourTree() {
	float behaviourTimer;
	float distanceToTarget;
	BehaviourAction* findKey = new BehaviourAction("Find Key", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "looking for a key!\n";
			behaviourTimer = rand() % 100;
			state = Ongoing;
		}
		else if (state == Ongoing) {
			behaviourTimer -= dt;
			if (behaviourTimer <= 0.0f) {
				std::cout << "Found a key!\n";
				return Success;
			}
		}
		return state;
		}
	);

	BehaviourAction* goToRoom = new BehaviourAction("Go to room", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Going to the loot room!\n";
			state = Ongoing;
		}
		else if (state = Ongoing) {
			distanceToTarget -= dt;
			if (distanceToTarget <= 0.0f) {
				std::cout << "Reached room!\n";
				return Success;
			}
		}
		return state;
		}
	);

	BehaviourAction* openDoor = new BehaviourAction("Open Door", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Opening Door!\n";
			return Success;
		}
		return state;
		}
	);

	BehaviourAction* lookForTreasure = new BehaviourAction("Look for treasure", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Looking For Treasure!\n";
			return Ongoing;
		}
		else if (state == Ongoing) {
			bool found = rand() % 2;
			if (found) {
				std::cout << "I found some treasure!\n";
				return Success;
			}
			std::cout << "No treasure in here...\n";
			return Failure;
		}
		return state;
		}
	);

	BehaviourAction* lookForItems = new BehaviourAction("Look for items", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Looking For Items!\n";
			return Ongoing;
		}
		else if (state == Ongoing) {
			bool found = rand() % 2;
			if (found) {
				std::cout << "I found some items!\n";
				return Success;
			}
			std::cout << "No items in here...\n";
			return Failure;
		}
		return state;
		}
	);

	BehaviourSequence* sequence = new BehaviourSequence("Room Sequence");
	sequence->AddChild(findKey);
	sequence->AddChild(goToRoom);
	sequence->AddChild(openDoor);

	BehaviourSelector* selection = new BehaviourSelector("Loot Selection");
	selection->AddChild(lookForTreasure);
	selection->AddChild(lookForItems);

	BehaviourSequence* rootSequence = new BehaviourSequence("Root Sequence");
	rootSequence->AddChild(sequence);
	rootSequence->AddChild(selection);

	for (int i = 0; i < 5; i++) {
		rootSequence->Reset();
		behaviourTimer = 0.0f;
		distanceToTarget = rand() % 250;
		BehaviourState state = Ongoing;
		std::cout << "We're going on an adventure!\n";
		while (state == Ongoing) {
			state = rootSequence->Execute(1.0f);
		}
		if (state == Success)std::cout << "What a successful adventure!\n";
		else if (state == Failure)std::cout << "What a waste of time!\n";
	}
	std::cout << "All Done!\n";
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
	Window*w = Window::CreateGameWindow("Video game!", 1280, 720);

	//TestPushDownAutomata(w);
	//TestNetworking();
	if (!w->HasInitialised()) {
		return -1;
	}	

	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);

	TestPathfinding();
	//TestBehaviourTree();
	w->GetTimer().GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!
	RunPushdownAutomata(w);
	
	//while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyCodes::ESCAPE)) {
		

	//	float dt = w->GetTimer().GetTimeDeltaSeconds();
	//	

	//}
	Window::DestroyGameWindow();
}