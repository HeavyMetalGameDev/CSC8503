

#include "TutorialGame.h"

using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame() : controller(*Window::GetWindow()->GetKeyboard(), *Window::GetWindow()->GetMouse()) {
	world		= new GameWorld();
#ifdef USEVULKAN
	renderer	= new GameTechVulkanRenderer(*world);
	renderer->Init();
	renderer->InitStructures();
#else 
	renderer = new GameTechRenderer(*world);
#endif

	physics		= new PhysicsSystem(*world);

	forceMagnitude	= 10.0f;
	useGravity		= false;
	inSelectionMode = false;

	world->GetMainCamera().SetController(controller);

	controller.MapAxis(0, "Sidestep");
	controller.MapAxis(1, "UpDown");
	controller.MapAxis(2, "Forward");

	controller.MapAxis(3, "XLook");
	controller.MapAxis(4, "YLook");

	InitialiseAssets();
	physics->BuildStaticQuadTree();
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
	pickupMesh = renderer->LoadMesh("Diamondo.msh");
	coinMesh = renderer->LoadMesh("coin.msh");
	gooseMesh = renderer->LoadMesh("goose.msh");

	basicTex	= renderer->LoadTexture("checkerboard.png");
	basicShader = renderer->LoadShader("scene.vert", "scene.frag");

	InitCamera();
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

void TutorialGame::UpdateGame(float dt) {
	deltaTime = dt;
	
	switch (GetState()) {
	case STATE_MENU:
		renderer->Update(dt);
		renderer->Render();
		return;
		break;
	case STATE_PLAYING:
		break;

	case STATE_WIN:
		Debug::Print("WIN!!!!!!!!!!!", { 40,50 }, Debug::GREEN);
		renderer->Update(dt);
		renderer->Render();
		gameTimer = 0.1f;
		return;
		break;
	case STATE_LOSE:
		Debug::Print("LOSE!!!!!!!!!!!", { 40,50 }, Debug::RED);
		renderer->Update(dt);
		renderer->Render();
		gameTimer = 0.1f;
		return;
		break;
	}
	if (gameTimer <= 0)SetState(STATE_LOSE);

	gameTimer -= dt;
	Debug::Print("Time:" + std::to_string(((int)gameTimer)), { 5, 5 });
	Debug::Print("/"+std::to_string(totalPickups), Vector2(90, 10), Debug::YELLOW);
	world->GetMainCamera().UpdateCamera(dt);


	world->UpdateWorld(dt);
	renderer->Update(dt);
	
	physics->Update(dt);
	

	
	
	renderer->Render();
	Debug::UpdateRenderables(dt);
}

void TutorialGame::UpdateGameAsClient(float dt) {
	client->UpdateClient();

	//we send packets to the server asking to join until the connection is esablished and the server replies

	if (!hasClientInitialised) { 
		PlayerConnectPacket* connectPacket = new PlayerConnectPacket();
		client->SendPacket(*connectPacket);
		delete connectPacket;
		return;
	}
	Debug::Print(".", { 49,50 });
	Debug::Print("Time:" + std::to_string(((int)gameTimer)), { 5, 5 });
	Debug::Print("/" + std::to_string(totalPickups), Vector2(90, 10), Debug::YELLOW);
	deltaTime = dt;
	

	world->GetMainCamera().UpdateCamera(dt);

	ClientInputComponent* inp;
	if (!player)return;
	if (player->TryGetComponent<ClientInputComponent>(inp)) {
		GamePacket* p;
		player->GetNetworkObject()->WriteClientPacket(&p,inp);
		client->SendPacket(*p);
	}

	world->UpdateWorld(dt);
	renderer->Update(dt);
	renderer->Render();
	Debug::UpdateRenderables(dt);
}

void TutorialGame::UpdateGameAsServer(float dt) {
	deltaTime = dt;
	server->UpdateServer();

	if (gameTimer <= 0)SetState(STATE_LOSE);
	switch (GetState()) {
	case STATE_MENU:
		renderer->Update(dt);
		renderer->Render();
		return;
		break;
	case STATE_PLAYING:
		break;

	case STATE_WIN:
		Debug::Print("WIN!!!!!!!!!!!", { 40,50 }, Debug::GREEN);
		renderer->Update(dt);
		renderer->Render();
		gameTimer = 0.1f;
		return;
		break;
	case STATE_LOSE:
		Debug::Print("LOSE!!!!!!!!!!!", { 40,50 }, Debug::RED);
		renderer->Update(dt);
		renderer->Render();
		gameTimer = 0.1f;
		return;
		break;
	}
	world->GetMainCamera().UpdateCamera(dt);

	world->UpdateWorld(dt);
	renderer->Update(dt);
	ApplyPlayerInput();
	physics->Update(dt);

	//here we send full packets for each network object

	world->OperateOnContents([&](GameObject* g)->void
		{if (g->GetNetworkObject()) {
		GamePacket* fp;
		g->GetNetworkObject()->WriteFullPacket(&fp);
		server->SendGlobalPacket(*fp);
	}; });

	


	renderer->Render();
	Debug::UpdateRenderables(dt);
}


void TutorialGame::InitCamera() {
	world->GetMainCamera().SetNearPlane(0.1f);
	world->GetMainCamera().SetFarPlane(500.0f);
	world->GetMainCamera().SetPitch(-15.0f);
	world->GetMainCamera().SetYaw(315.0f);
	world->GetMainCamera().SetPosition(Vector3(-60, 40, 60));
	lockedObject = nullptr;
}

void TutorialGame::InitWorldSinglePlayer() {
	world->ClearAndErase();
	physics->Clear();
	gameTimer = 140;
	totalPickups = 0;
	SetState(STATE_PLAYING);
	
	//this should probably be all refactored into a function for reuse, oh well
	AddPlayerToWorld(Vector3(80, 0, 10),false,false);
    SetupWorld(false, false);

	world->StartWorld();
}


void TutorialGame::InitWorldClient() {
	
	isClient = true;
	NetworkBase::Initialise();
	port = NetworkBase::GetDefaultPort();
	client = new GameClient();
	client->RegisterPacketHandler(String_Message, this);
	client->RegisterPacketHandler(Full_State, this);
	client->RegisterPacketHandler(Player_Connected, this);
	client->RegisterPacketHandler(Game_info, this);

	bool canConnect = client->Connect(127, 0, 0, 1, port);
	world->ClearAndErase();
	physics->Clear();
	gameTimer = 140;
	totalPickups = 0;
	SetState(STATE_CLIENT);

    SetupWorld(true, false);
	world->StartWorld();
}

void TutorialGame::InitWorldServer() { //setup world as server
	isClient = false;
	NetworkBase::Initialise();
	port = NetworkBase::GetDefaultPort();
	server = new GameServer(port, 4);
	server->RegisterPacketHandler(String_Message, this);
	server->RegisterPacketHandler(Client_State, this);
	server->RegisterPacketHandler(Player_Connected, this);

	world->ClearAndErase();
	physics->Clear();
	gameTimer = 140;
	totalPickups = 0;
	SetState(STATE_SERVER);

    SetupWorld(true,true);
	world->StartWorld();
}

void TutorialGame::SetupWorld(bool isNetworked, bool isServerSide)
{
    CreateStaticLevel(isNetworked, isServerSide);
    InitDefaultFloor();

    AddOBBCubeToWorld(Vector3(70, -5, 10), Vector3(3, 1, 3), 0, isNetworked, isServerSide);
    AddCubeToWorld(Vector3(70, 3, 10), Vector3(1, 0.5f, 1), 0, isNetworked, isServerSide);
    AddSphereToWorld(Vector3(70, 0, 10), .3f, 0.7f, isNetworked, isServerSide);
    AddKeyDoorPairToWorld(Vector3(70, 3.5f, 10), Vector3(10, 0, 20), Debug::YELLOW, isNetworked, isServerSide);
    AddTreasurePoint(Vector3(85, -3, 10), isNetworked, isServerSide);

    AddCapsuleToWorld(Vector3(80, 0, 10), 1, 0.7f, isNetworked, isServerSide);
    AddRopeToWorld(Vector3(50, 5, 10), isNetworked, isServerSide);
    AddCubeWallToWorld(Vector3(30, -4, 6), isNetworked, isServerSide);
    AddJumppad(Vector3(20, -5, 40), isNetworked, isServerSide);

    std::vector<Vector3> patrolPoints;
    patrolPoints.emplace_back(Vector3(60, 0, 40));
    patrolPoints.emplace_back(Vector3(60, 0, 60));
    AddEnemyToWorld(Vector3(60, 0, 40), patrolPoints, isNetworked, isServerSide);
    AddGrappleUnlockerToWorld(Vector3(80, 0, 80), isNetworked, isServerSide);


    GameObject* t = AddTreasure(Vector3(80, 20, 10), isNetworked, isServerSide);
    GameObject* bte = AddBehaviourTreeEnemyToWorld(Vector3(80, 25, 10), isNetworked, isServerSide);
    BehaviourTreeEnemyComponent* btec;
    if (bte->TryGetComponent<BehaviourTreeEnemyComponent>(btec)) {
        btec->SetTreasureObject(t);
    }
}

void TutorialGame::ReceivePacket(int type, GamePacket* payload, int source) {
	if (isClient) {
        ReadPacketClient(type, payload);
	}
	else {
        ReadPacketServer(type, payload, source);
	}
}
void TutorialGame::ReadPacketServer(int type, GamePacket* payload, int source){
    switch (type) {
        case Client_State: {
            ClientPacket* realPacket = (ClientPacket*)payload;
            ProcessClientInput(realPacket);
            break;
        }
        case Player_Connected: { //this means a client is sending a connect message
            ProcessServerPlayerConnectPacket(source, payload);
            break;
        }
    }
}
void TutorialGame::ReadPacketClient(int type, GamePacket* payload)
{
    switch (type) {
        case Full_State: {
            ProcessClientFullPacket(payload);
            break;
        }
        case Player_Connected: { //if we recieve this, it tells us a new player has connected (could be this client)
            ProcessClientPlayerConnectedPacket(payload);
            break;
        }
        case Game_info: {
            ProcessClientGameInfoPacket(payload);
            break;
        }
    }
}
void TutorialGame::ProcessServerPlayerConnectPacket(int source, GamePacket* payload)
{
    if (prevClient == source)return; //just in case we receive lots of packets from the same client, ignore them and only send one packet back
    PlayerConnectPacket* realPacket = (PlayerConnectPacket*)payload;
    numPlayers++;
    GameObject* player = AddPlayerToWorld(Vector3(80, 0, 10), true, true);
    playerObjects.push_back(player);
    //add a player to world
    GamePacket* p;
    playerIDs[numPlayers - 1] = currentNetworkObjectID - 1;
    PlayerConnectServerAckPacket* pac = new PlayerConnectServerAckPacket();
    pac->numPlayers = numPlayers;
    pac->playerNetIDs[0] = playerIDs[0];
    pac->playerNetIDs[1] = playerIDs[1];
    pac->playerNetIDs[2] = playerIDs[2];
    pac->playerNetIDs[3] = playerIDs[3];
    server->SendGlobalPacket(*pac); //tell all clients a new player has joined
    prevClient = source;
}

void NCL::CSC8503::TutorialGame::ProcessClientFullPacket(GamePacket* payload)
{
    FullPacket* realPacket = (FullPacket*)payload;
    world->OperateOnContents(
        [&](GameObject* g) {
            if (g->GetNetworkObject()) {
                if (g->GetNetworkObject()->GetNetworkID() == realPacket->objectID) {
                    g->GetNetworkObject()->ReadFullPacket(*realPacket);
                }
            }
        });
}
void NCL::CSC8503::TutorialGame::ProcessClientGameInfoPacket(GamePacket* payload)
{
    GameInfoPacket* realPacket = (GameInfoPacket*)payload;
    ClientPlayerComponent* c;
    if (playerMap[realPacket->objectID]->TryGetComponent<ClientPlayerComponent>(c)) {
        c->SetCollectables(realPacket->collectables);
        c->SetHealth(realPacket->health);
        c->SetPoints(realPacket->points);
    }
}
void TutorialGame::ProcessClientPlayerConnectedPacket(GamePacket* payload)
{
    if (!hasClientInitialised) { //this means that this is a new client and objects for all current players are needed
        PlayerConnectServerAckPacket* realPacket = (PlayerConnectServerAckPacket*)payload;
        AddPlayerToWorld(Vector3(80, 0, 10), true, false, true, realPacket->playerNetIDs[realPacket->numPlayers - 1]);
        for (int i = 0; i < realPacket->numPlayers - 1; i++) {
            AddPlayerToWorld(Vector3(80, 0, 10), true, false, false, realPacket->playerNetIDs[i]);
        }
        hasClientInitialised = true;
    }
    else { //this means a new player has joined the game and a new player object is needed
        PlayerConnectServerAckPacket* realPacket = (PlayerConnectServerAckPacket*)payload;
        AddPlayerToWorld(Vector3(80, 0, 10), true, false, false, realPacket->playerNetIDs[realPacket->numPlayers - 1]);
    }
}
void TutorialGame::ProcessClientInput(ClientPacket* p) {

	playerInputsMap[p->objectID][0] = p->buttonstates[0];//w
	playerInputsMap[p->objectID][1] = p->buttonstates[1];//a
	playerInputsMap[p->objectID][2] = p->buttonstates[2];//s
	playerInputsMap[p->objectID][3] = p->buttonstates[3];//d
	playerInputsMap[p->objectID][4] = p->buttonstates[4];//space
	playerInputsMap[p->objectID][5] = p->buttonstates[5];//left click
	playerInputsMap[p->objectID][6] = p->buttonstates[6];//right click
	playerInputsMap[p->objectID][7] = p->buttonstates[7];//e

	playerCameraMap[p->objectID].pitch = p->camPitch;
	playerCameraMap[p->objectID].yaw = p->camYaw;
}

void TutorialGame::ApplyPlayerInput() { //this probably could have been done in a game object component
	for (std::pair<const int, bool[8]> inputPair : playerInputsMap) {
		if (!inputPair.first)continue;
		CameraInputStruct c = playerCameraMap[inputPair.first];
		GameObject* g = playerMap[inputPair.first];
		Transform playerTransform = g->GetTransform();
		Vector3 inputDirection(inputPair.second[3] - inputPair.second[1], 0, inputPair.second[2] - inputPair.second[0]);
		Matrix4 yawRotation = Matrix4::Rotation(c.yaw, Vector3(0, 1, 0));

		Vector3 direction;
		direction += yawRotation * Vector3(0, 0, inputDirection.z);
		direction += yawRotation * Vector3(inputDirection.x, 0, 0);
		g->GetPhysicsObject()->SetAwake();
		g->GetPhysicsObject()->AddForce(direction * deltaTime * 7000.0f);

	}
}


/// ----------------HERE LIES FUNCTIONS TO ADD OBJECTS TO THE GAME-------------------------READER BEWARE---------------------------
void TutorialGame::AddRopeToWorld(const Vector3& position, bool isNetworked, bool isServerSide) {
	Vector3 cubeSize = Vector3(0.2f, 0.3f, 0.2f);

	float invCubeMass = 0.4f;
	int numLinks = 12;
	float maxDistance = 0.7f;
	float cubeDistance = -0.8f;

	Vector3 startPos = position;

	GameObject* start = AddCubeToWorld(startPos, cubeSize, 0);

	GameObject* previous = start;

	for (int i = 0; i < numLinks; i++) {
		GameObject* block = AddCubeToWorld(startPos + Vector3(0, (i + 1) * cubeDistance, 0), cubeSize, invCubeMass,isNetworked,isServerSide);

		if (!isNetworked || isServerSide) {
			PositionConstraint* constraint = new PositionConstraint(previous, block, maxDistance);
			world->AddConstraint(constraint);
			previous = block;
		}
	}
}

/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject();

	Vector3 floorSize = Vector3(200, 2, 200);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume(),false,false,STATIC_LAYER));

	PhysicsObject* fo = floor->GetPhysicsObject();
	fo->SetInverseMass(0);
	fo->InitCubeInertia();

	PhysicsMaterial* floorPhys;
	if (world->TryGetPhysMat("Standard", floorPhys))fo->SetPhysMat(floorPhys);

	world->AddGameObject(floor);

	return floor;
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass, bool isNetworked, bool isServerSide) {
	GameObject* sphere = new GameObject();
	sphere->SetTag("Sphere");

	Vector3 sphereSize = Vector3(radius, radius, radius);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	if (!isNetworked || isServerSide) {
		SphereVolume* volume = new SphereVolume(radius);
		sphere->SetBoundingVolume((CollisionVolume*)volume);
		sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

		PhysicsObject* so = sphere->GetPhysicsObject();
		so->SetInverseMass(inverseMass);
		so->InitSphereInertia();

		PhysicsMaterial* spherePhys;
		if (world->TryGetPhysMat("Bouncy", spherePhys))so->SetPhysMat(spherePhys);
	}
	if (isNetworked) {
		sphere->SetNetworkObject(new NetworkObject(*sphere, currentNetworkObjectID));
		currentNetworkObjectID++;
	}
	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddTreasure(const Vector3& position, bool isNetworked, bool isServerSide) {
	float radius = 1.0f;
	GameObject* treasure = new GameObject();
	treasure->SetTag("Treasure");

	Vector3 treasureSize = Vector3(radius, radius, radius);


	treasure->GetTransform()
		.SetScale(treasureSize)
		.SetPosition(position);

	treasure->SetRenderObject(new RenderObject(&treasure->GetTransform(), gooseMesh, basicTex, basicShader));
	
	treasure->GetRenderObject()->SetColour(Debug::YELLOW);

	if (!isNetworked || isServerSide) {
		SphereVolume* volume = new SphereVolume(radius);
		treasure->SetBoundingVolume((CollisionVolume*)volume);
		treasure->SetPhysicsObject(new PhysicsObject(&treasure->GetTransform(), treasure->GetBoundingVolume()));
		PhysicsObject* so = treasure->GetPhysicsObject();
		so->SetInverseMass(0.2f);
		so->InitSphereInertia();

		PhysicsMaterial* treasurePhys;
		if (world->TryGetPhysMat("Standard", treasurePhys))so->SetPhysMat(treasurePhys);

	}
	if (isNetworked) {
		treasure->SetNetworkObject(new NetworkObject(*treasure, currentNetworkObjectID));
		currentNetworkObjectID++;
	}

	world->AddGameObject(treasure);

	return treasure;
}

GameObject* TutorialGame::AddTreasurePoint(const Vector3& position, bool isNetworked, bool isServerSide) {
	GameObject* trigger = new GameObject();
	trigger->SetTag("TreasurePoint");

	Vector3 triggerSize = Vector3(5, 5, 10);
	AABBVolume* volume = new AABBVolume(triggerSize*0.5f);
	trigger->SetBoundingVolume((CollisionVolume*)volume);

	trigger->GetTransform()
		.SetScale(triggerSize)
		.SetPosition(position);
	trigger->SetRenderObject(new RenderObject(&trigger->GetTransform(), cubeMesh, basicTex, basicShader));
	trigger->GetRenderObject()->SetColour({ 1,1,0,0.5f });
	if (!isNetworked || isServerSide) {

		trigger->SetPhysicsObject(new PhysicsObject(&trigger->GetTransform(), trigger->GetBoundingVolume(), true, true, PICKUP_SPHERE_LAYER));


		TreasureReturnPointComponent* trp = new TreasureReturnPointComponent(world);
		trigger->AddComponent(trp);
	}


	//PhysicsMaterial* spherePhys;
	//if (world->TryGetPhysMat("Bouncy", spherePhys))so->SetPhysMat(spherePhys);

	world->AddGameObject(trigger);

	return trigger;
}

GameObject* TutorialGame::AddSphereTriggerToWorld(const Vector3& position, float radius, bool isNetworked, bool isServerSide) {
	GameObject* sphere = new GameObject();
	sphere->SetTag("SphereTrigger");

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	if (isServerSide) {
		sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	}
	if (!isNetworked || isServerSide) {
		
		sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume(), true, true, PICKUP_SPHERE_LAYER));

		//PhysicsMaterial* spherePhys;
		//if (world->TryGetPhysMat("Bouncy", spherePhys))so->SetPhysMat(spherePhys);
	}

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddPointPickupToWorld(const Vector3& position, int points, bool isNetworked, bool isServerSide) {
	GameObject* sphere = new GameObject();
	sphere->SetTag("PointPickup");

	Vector3 sphereSize = Vector3(.5f, .5f, .5f);

	sphere->GetTransform()
		.SetScale(sphereSize*0.2f)
		.SetScale(sphereSize*0.2f)
		.SetScale(sphereSize*0.2f
		
		)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), coinMesh, basicTex, basicShader));
	sphere->GetRenderObject()->SetColour(Debug::YELLOW);

	if (!isNetworked || isServerSide) {
		SphereVolume* volume = new SphereVolume(.5f);
		sphere->SetBoundingVolume((CollisionVolume*)volume);
		sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume(), false, true, STATIC_LAYER));
		PointPickupComponent* ppc = new PointPickupComponent(world, sphere, points);
		sphere->AddComponent(ppc);
	}
	if (isNetworked) {
		sphere->SetNetworkObject(new NetworkObject(*sphere, currentNetworkObjectID));
		currentNetworkObjectID++;
	}


	world->AddGameObject(sphere);

	totalPickups++;

	return sphere;
}
GameObject* TutorialGame::AddGrappleUnlockerToWorld(const Vector3& position, bool isNetworked, bool isServerSide) {
	GameObject* sphere = new GameObject();
	sphere->SetTag("GrapplePickup");

	Vector3 sphereSize = Vector3(.5f, .5f, .5f);
	SphereVolume* volume = new SphereVolume(.5f);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), pickupMesh, basicTex, basicShader));
	if (!isNetworked || isServerSide) {
		sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume(), false, true, STATIC_LAYER));
		sphere->GetRenderObject()->SetColour(Debug::GREEN);

		UnlockGrappleComponent* ug = new UnlockGrappleComponent();
		sphere->AddComponent(ug);

		//PhysicsMaterial* spherePhys;
		//if (world->TryGetPhysMat("Bouncy", spherePhys))so->SetPhysMat(spherePhys);
	}

	world->AddGameObject(sphere);

	return sphere;
}
GameObject* TutorialGame::AddJumppad(const Vector3& position, bool isNetworked, bool isServerSide) {
	GameObject* jp = new GameObject();
	jp->SetTag("Jumppad");

	Vector3 size = Vector3(7.0f, 1.0f, 7.0f);
	AABBVolume* volume = new AABBVolume(size*0.5f);
	jp->SetBoundingVolume((CollisionVolume*)volume);

	jp->GetTransform()
		.SetScale(size)
		.SetPosition(position);

	jp->SetRenderObject(new RenderObject(&jp->GetTransform(), cubeMesh, basicTex, basicShader));
	jp->GetRenderObject()->SetColour(Debug::MAGENTA);
	if (!isNetworked || isServerSide) {
		jp->SetPhysicsObject(new PhysicsObject(&jp->GetTransform(), jp->GetBoundingVolume(), false, true, STATIC_LAYER));


		JumppadComponent* jpc = new JumppadComponent();
		//PhysicsMaterial* spherePhys;
		//if (world->TryGetPhysMat("Bouncy", spherePhys))so->SetPhysMat(spherePhys);
		jp->AddComponent(jpc);
	}

	world->AddGameObject(jp);

	return jp;
}

GameObject* TutorialGame::AddCapsuleToWorld(const Vector3& position, float size, float inverseMass, bool isNetworked, bool isServerSide) {
	GameObject* capsule = new GameObject();
	capsule->SetTag("Capsule");


	capsule->GetTransform()
		.SetScale(Vector3(size, size, size))
		.SetPosition(position);

	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, basicShader));

	if (!isNetworked || isServerSide) {
		CapsuleVolume* volume = new CapsuleVolume(size * 0.5f, size * 0.5f);
		capsule->SetBoundingVolume((CollisionVolume*)volume);
		capsule->SetPhysicsObject(new PhysicsObject(&capsule->GetTransform(), capsule->GetBoundingVolume()));

		PhysicsObject* co = capsule->GetPhysicsObject();
		co->SetInverseMass(inverseMass);
		co->InitSphereInertia();

		PhysicsMaterial* capPhys;
		if (world->TryGetPhysMat("Standard", capPhys))co->SetPhysMat(capPhys);
	}
	if (isNetworked) {
		capsule->SetNetworkObject(new NetworkObject(*capsule, currentNetworkObjectID));
		currentNetworkObjectID++;
	}

	world->AddGameObject(capsule);

	return capsule;
}

GameObject* TutorialGame::AddTestToWorld() {
	GameObject* cube = new GameObject();

	Vector3 sphereSize = Vector3(2, 2, 2);

	cube->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(sphereSize);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), sphereMesh, basicTex, basicShader));

	HoverComponent* hc = new HoverComponent();
	cube->AddComponent(hc);
	world->AddGameObject(cube);

	return cube;
}
GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass, bool isNetworked, bool isServerSide) {
	GameObject* cube = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);


	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));

	if (!isNetworked || isServerSide) {
		if (inverseMass == 0) {
			cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume(), false, false, STATIC_LAYER));
		}
		else {
			cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));
		}

		PhysicsObject* co = cube->GetPhysicsObject();

		co->InitCubeInertia();
		co->SetInverseMass(inverseMass);

		PhysicsMaterial* cubePhys;
		if (world->TryGetPhysMat("Standard", cubePhys))co->SetPhysMat(cubePhys);
	}
	if (isNetworked) {
		cube->SetNetworkObject(new NetworkObject(*cube, currentNetworkObjectID));
		currentNetworkObjectID++;
	}
	world->AddGameObject(cube);

	return cube;
}

void TutorialGame::AddCubeWallToWorld(const Vector3& position, bool isNetworked, bool isServerSide) {
	for (int x = 0; x < 5; x++) {
		for (int y = 0; y < 5; y++) {
			Vector3 cubePos = Vector3(position.x, position.y + y*2, position.z + x*2);
			AddCubeToWorld(cubePos, Vector3(0.98f, 0.98f, 0.98f), 0.5f,isNetworked,isServerSide);
		}
	}

}

GameObject* TutorialGame::AddOBBCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass, bool isNetworked, bool isServerSide) {
	GameObject* cube = new GameObject();

	OBBVolume* volume = new OBBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2)
		.SetOrientation(Quaternion::EulerAnglesToQuaternion(30,0,0));
	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));

	if (!isNetworked || isServerSide) {
		
		cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume(), false, false, STATIC_LAYER));

		PhysicsObject* co = cube->GetPhysicsObject();
		co->SetInverseMass(0);
		co->InitCubeInertia();

		PhysicsMaterial* cubePhys;
		if (world->TryGetPhysMat("Standard", cubePhys))co->SetPhysMat(cubePhys);
	}
	if (isNetworked) {
		cube->SetNetworkObject(new NetworkObject(*cube, currentNetworkObjectID));
		currentNetworkObjectID++;
	}
	world->AddGameObject(cube);

	return cube;
}

GameObject* TutorialGame::AddWallToWorld(const Vector3& position, Vector3 dimensions, bool isNetworked, bool isServerSide) {
	GameObject* cube = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));

	if (!isNetworked || isServerSide) {
		cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume(), false));

		PhysicsObject* co = cube->GetPhysicsObject();
		co->SetInverseMass(0);
		co->InitCubeInertia();
		co->SetCollisionLayer(STATIC_LAYER);

		PhysicsMaterial* cubePhys;
		if (world->TryGetPhysMat("Standard", cubePhys))co->SetPhysMat(cubePhys);
	}
	world->AddGameObject(cube);

	return cube;
}

GameObject* TutorialGame::AddPlayerToWorld(const Vector3& position, bool isNetworked, bool isServerSide,bool isThisPlayer, int networkID) {
	float meshSize		= 1.0f;
	float inverseMass	= 0.8f;

	GameObject* character = new GameObject();
	CapsuleVolume* volume  = new CapsuleVolume(meshSize*0.5f,meshSize*0.5f);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), capsuleMesh, nullptr, basicShader));
	
	character->SetTag("Player");
	if (!isNetworked) {
		character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume(), true, false, PLAYER_LAYER));

		character->SetCamera(&world->GetMainCamera());
		PhysicsObject* co = character->GetPhysicsObject();
		co->SetInverseMass(inverseMass);
		co->InitSphereInertia();

		PhysicsMaterial* playerPhys;
		PhysicsMaterial* bouncyPhys;
		if (world->TryGetPhysMat("Player", playerPhys))co->SetPhysMat(playerPhys);
		world->TryGetPhysMat("Bouncy", bouncyPhys);

		MovementApplierComponent* ma = new MovementApplierComponent(&character->GetTransform(), character->GetPhysicsObject(), 7000.0f);
		FirstPersonInputComponent* fps = new FirstPersonInputComponent(&world->GetMainCamera());
		PlayerInputComponent* pic = new PlayerInputComponent(character, &world->GetMainCamera());
		PlayerValuesComponent* pvc = new PlayerValuesComponent(world);

		GameObject* pickupObject = AddSphereTriggerToWorld(Vector3(-1, 10, 0), 0.8f, isNetworked, isServerSide);

		TriggerComponent* tc = new TriggerComponent();

		pickupObject->AddComponent(tc);

		ObjectPickupComponent* opc = new ObjectPickupComponent(character, pickupObject, &world->GetMainCamera(), tc);

		ma->SetInputComponent(fps);
		character->AddComponent(ma);
		character->AddComponent(pic);
		character->AddComponent(opc);
		character->AddComponent(pvc);

	}
	else {
		if (isServerSide) {
			character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume(), true, false, PLAYER_LAYER));
			PhysicsObject* co = character->GetPhysicsObject();
			co->SetInverseMass(inverseMass);
			co->InitSphereInertia();

			PhysicsMaterial* playerPhys;
			PhysicsMaterial* bouncyPhys;
			if (world->TryGetPhysMat("Player", playerPhys))co->SetPhysMat(playerPhys);
			world->TryGetPhysMat("Bouncy", bouncyPhys);

			GameObject* pickupObject = AddSphereTriggerToWorld(Vector3(-1, 10, 0), 0.8f, false, isServerSide);

			TriggerComponent* tc = new TriggerComponent();

			pickupObject->AddComponent(tc);
			ServerPlayerComponent* si = new ServerPlayerComponent(character, pickupObject, tc, playerInputsMap[currentNetworkObjectID], &playerCameraMap[currentNetworkObjectID],world,server);

			character->AddComponent(si);

			character->SetNetworkObject(new NetworkObject(*character, currentNetworkObjectID));
			playerMap[currentNetworkObjectID] = character;
			currentNetworkObjectID++;

		}
		else {
			if (isThisPlayer) {
				character->SetCamera(&world->GetMainCamera());
				ClientInputComponent* ci = new ClientInputComponent(character, &world->GetMainCamera(), character->GetNetworkObject());
				ClientPlayerComponent* cp = new ClientPlayerComponent();
				character->AddComponent(ci);
				character->AddComponent(cp);
				player = character;

				character->SetNetworkObject(new NetworkObject(*character, networkID));
				playerMap[networkID] = character;
				player = character;
			}
			else {
				character->SetNetworkObject(new NetworkObject(*character, networkID));
				playerMap[networkID] = character; //we dont increment currentNetworkID as we are using the networkID param instead
			}
		}
	}

	world->AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddEnemyToWorld(const Vector3& position, std::vector<Vector3>& patrolPoints, bool isNetworked, bool isServerSide) {
	float meshSize		= 1.0f;
	float inverseMass	= 0.5f;

	GameObject* character = new GameObject();
	character->SetTag("Enemy");

	CapsuleVolume* volume = new CapsuleVolume(meshSize * 0.5f, meshSize * 0.5f);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), capsuleMesh, nullptr, basicShader));
	character->GetRenderObject()->SetColour(Debug::RED);
	if (!isNetworked) {
		character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

		PhysicsObject* co = character->GetPhysicsObject();
		co->SetInverseMass(inverseMass);
		co->InitSphereInertia();
		PhysicsMaterial* enemyPhys;
		if (world->TryGetPhysMat("Player", enemyPhys))co->SetPhysMat(enemyPhys);

		MovementApplierComponent* ma = new MovementApplierComponent(&character->GetTransform(), character->GetPhysicsObject(), 7000.0f);
		StateMachineEnemyComponent* sme = new StateMachineEnemyComponent(character, patrolPoints);

		ma->SetInputComponent(sme);

		character->AddComponent(ma);
		character->AddComponent(sme);

	}
	else {
		character->SetNetworkObject(new NetworkObject(*character, currentNetworkObjectID));
		currentNetworkObjectID++;
		if (isServerSide) {
			character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

			PhysicsObject* co = character->GetPhysicsObject();
			co->SetInverseMass(inverseMass);
			co->InitSphereInertia();
			PhysicsMaterial* enemyPhys;
			if (world->TryGetPhysMat("Player", enemyPhys))co->SetPhysMat(enemyPhys);

			MovementApplierComponent* ma = new MovementApplierComponent(&character->GetTransform(), character->GetPhysicsObject(), 7000.0f);
			ServerStateMachineEnemyComponent* ssme = new ServerStateMachineEnemyComponent(character, patrolPoints,world);
			ssme->SetPlayerVector(&playerObjects);

			ma->SetInputComponent(ssme);

			character->AddComponent(ma);
			character->AddComponent(ssme);
		}
	}
	
	
	

	world->AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddBehaviourTreeEnemyToWorld(const Vector3& position, bool isNetworked, bool isServerSide) {
	float radius = 3.0f;
	float inverseMass = 0.5f;

	GameObject* character = new GameObject();
	character->SetTag("Enemy");
	Vector3 charSize = Vector3(radius, radius, radius);

	character->GetTransform()
		.SetScale(charSize)
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), sphereMesh, nullptr, basicShader));
	character->GetRenderObject()->SetColour(Debug::RED);
	if (!isNetworked) {
		SphereVolume* volume = new SphereVolume(radius);
		character->SetBoundingVolume((CollisionVolume*)volume);
		character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

		PhysicsObject* co = character->GetPhysicsObject();
		co->SetInverseMass(inverseMass);
		co->InitSphereInertia();
		PhysicsMaterial* enemyPhys;
		if (world->TryGetPhysMat("Player", enemyPhys))co->SetPhysMat(enemyPhys);

		BehaviourTreeEnemyComponent* btea = new BehaviourTreeEnemyComponent(character);
		character->AddComponent(btea);

	}
	else {
		character->SetNetworkObject(new NetworkObject(*character, currentNetworkObjectID));
		currentNetworkObjectID++;
		if (isServerSide) {
			SphereVolume* volume = new SphereVolume(radius);
			character->SetBoundingVolume((CollisionVolume*)volume);
			character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

			PhysicsObject* co = character->GetPhysicsObject();
			co->SetInverseMass(inverseMass);
			co->InitSphereInertia();
			PhysicsMaterial* enemyPhys;
			if (world->TryGetPhysMat("Player", enemyPhys))co->SetPhysMat(enemyPhys);

			BehaviourTreeEnemyComponent* btea = new BehaviourTreeEnemyComponent(character);
			character->AddComponent(btea);
		}
	}




	world->AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddKeyDoorPairToWorld(const Vector3& keyPosition, const Vector3& doorPosition, const Vector4& colour, bool isNetworked, bool isServerSide) { //returns the key object
	GameObject* key = new GameObject();
	GameObject* door = new GameObject();
	Vector3 keyScale(0.2f, 0.2f, 0.3f);
	Vector3 doorScale(5, 5,5);

	AABBVolume* keyVolume = new AABBVolume(keyScale);
	key->SetBoundingVolume((CollisionVolume*)keyVolume);

	AABBVolume* doorVolume = new AABBVolume(doorScale);
	door->SetBoundingVolume((CollisionVolume*)doorVolume);

	key->GetTransform()
		.SetPosition(keyPosition)
		.SetScale(keyScale * 2);

	door->GetTransform()
		.SetPosition(doorPosition)
		.SetScale(doorScale * 2);

	key->SetRenderObject(new RenderObject(&key->GetTransform(), cubeMesh, basicTex, basicShader));

	key->GetRenderObject()->SetColour(colour);

	door->SetRenderObject(new RenderObject(&door->GetTransform(), cubeMesh, basicTex, basicShader));
	door->GetRenderObject()->SetColour(colour);

	if (!isNetworked || isServerSide) {
		door->SetPhysicsObject(new PhysicsObject(&door->GetTransform(), door->GetBoundingVolume(), false,false,STATIC_LAYER)); //door is static
		key->SetPhysicsObject(new PhysicsObject(&key->GetTransform(), key->GetBoundingVolume()));
		PhysicsObject* ko = key->GetPhysicsObject();
		ko->SetInverseMass(0.7f);
		ko->InitCubeInertia();

		PhysicsObject* doorO = door->GetPhysicsObject();
		doorO->SetInverseMass(0);
		doorO->InitCubeInertia();

		PhysicsMaterial* keyPhys;
		if (world->TryGetPhysMat("Standard", keyPhys))ko->SetPhysMat(keyPhys);

		PhysicsMaterial* doorPhys;
		if (world->TryGetPhysMat("Standard", doorPhys))doorO->SetPhysMat(doorPhys);

		KeyComponent* kc = new KeyComponent(key, door, world);
		key->AddComponent(kc);
	}
	if (isNetworked) {
		key->SetNetworkObject(new NetworkObject(*key, currentNetworkObjectID));
		currentNetworkObjectID++;

		door->SetNetworkObject(new NetworkObject(*door, currentNetworkObjectID));
		currentNetworkObjectID++;
	}
	world->AddGameObject(key);
	world->AddGameObject(door);

	return key;
}


void TutorialGame::InitDefaultFloor() {
	AddFloorToWorld(Vector3(0, -7, 0)); 
}


void TutorialGame::CreateStaticLevel(bool isNetworked, bool isServerSide) { //read from the file to create the level
	std::ifstream infile(Assets::DATADIR + "TestGrid1.txt");
	int nodeSize;
	int gridWidth;
	int gridHeight;
	infile >> nodeSize;
	infile >> gridWidth;
	infile >> gridHeight;
	for (int y = 0; y < gridHeight; ++y) {
		for (int x = 0; x < gridWidth; ++x) {
			char type = 0;
			infile >> type;
			if (type == 'r') { //roof
				AddWallToWorld(Vector3((float)(x * nodeSize), nodeSize * 1, (float)(y * nodeSize)), Vector3(nodeSize, nodeSize, nodeSize) * 0.5f, isNetworked, isServerSide);
				AddPointPickupToWorld(Vector3((float)(x * nodeSize), -4, (float)(y * nodeSize)), 10,isNetworked,isServerSide);
			}
			else if (type != '.') {
				int intType = type - '0';
				for(int i=0;i<intType;i++)
				AddWallToWorld(Vector3((float)(x * nodeSize), nodeSize*i, (float)(y * nodeSize)), Vector3(nodeSize, nodeSize, nodeSize) * 0.5f, isNetworked, isServerSide);
			}
			else {
				AddPointPickupToWorld(Vector3((float)(x * nodeSize), -4, (float)(y * nodeSize)),10, isNetworked, isServerSide);
			}
		}
	}
	infile.close();
}


