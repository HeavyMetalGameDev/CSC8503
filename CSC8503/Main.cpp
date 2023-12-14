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
		textTimer += dt;
		Vector4 col1 = Debug::CYAN * sin(textTimer) - Debug::MAGENTA * (sin(textTimer));
		Vector4 col2 = Debug::MAGENTA * sin(textTimer) - Debug::CYAN * (sin(textTimer));
		Debug::Print("1: Start Game singleplayer", Vector2(25.2, 30.2),col1);
		Debug::Print("1: Start Game singleplayer", Vector2(25, 30), col2);

		Debug::Print("2: Start Game as client", Vector2(25.2, 45.2), col1);
		Debug::Print("2: Start Game as client", Vector2(25, 45), col2);

		Debug::Print("3: Start Game as server", Vector2(25.2, 60.2), col1);
		Debug::Print("3: Start Game as server", Vector2(25, 60), col2);

		Debug::Print("ESCAPE: Exit Game", Vector2(25.2, 75.2),col1);
		Debug::Print("ESCAPE: Exit Game", Vector2(25, 75), col2);
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
	float textTimer = 0;
};

void RunPushdownAutomata(Window* w) {
	TutorialGame* newGame = new TutorialGame();
	PushdownMachine machine(new MenuScreen(newGame));
	while (w->UpdateWindow()) {
		float dt = w->GetTimer().GetTimeDeltaSeconds();
		if (!machine.Update(dt))return;
	}
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
	if (!w->HasInitialised()) {
		return -1;
	}	

	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);

	w->GetTimer().GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!
	RunPushdownAutomata(w);
	
	//while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyCodes::ESCAPE)) {
		

	//	float dt = w->GetTimer().GetTimeDeltaSeconds();
	//	

	//}
	Window::DestroyGameWindow();
}