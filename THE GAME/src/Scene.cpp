#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Log.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Player.h"
#include "Map.h"
#include "Item.h"
#include "Enemy.h"
#include "Murcielago.h"
#include "CheckPoint.h"
#include "GuiControl.h"
#include "GuiManager.h"
#include "Spring.h"
#include "BOO.h"

Scene::Scene() : Module()
{
	
	name = "scene";
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;
	
	//L04: TODO 3b: Instantiate the player using the entity manager
	player = (Player*)Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER);
	player->SetParameters(configParameters.child("entities").child("player"));
	
	//L08 Create a new item using the entity manager and set the position to (200, 672) to test
	for(pugi::xml_node itemNode = configParameters.child("entities").child("items").child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Item* item = (Item*) Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
		item->SetParameters(itemNode);
	}

	// Create a enemy using the entity manager 
	for (pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").child("enemy0"); enemyNode; enemyNode = enemyNode.next_sibling("enemy0"))
	{
		Enemy* enemy = (Enemy*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY);
		enemy->SetParameters(enemyNode);
	 	enemyList.push_back(enemy);
	}
	for (pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").child("enemy1"); enemyNode; enemyNode = enemyNode.next_sibling("enemy1"))
	{
		Enemy* enemy2 = (Enemy*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY);
		enemy2->SetParameters(enemyNode);
		enemyList.push_back(enemy2);
	}

	for (pugi::xml_node CheckPointNode = configParameters.child("entities").child("items").child("checkpoint"); CheckPointNode; CheckPointNode = CheckPointNode.next_sibling("checkpoint"))
	{
		checkPoint = (CheckPoint*)Engine::GetInstance().entityManager->CreateEntity(EntityType::CHECKPOINT);
		checkPoint->SetParameters(CheckPointNode);
	}

	for (pugi::xml_node enemyNode1 = configParameters.child("entities").child("enemies").child("murcielago0"); enemyNode1; enemyNode1 = enemyNode1.next_sibling("murcielago0"))
	{
		EnemyInClass* enemy1 = (EnemyInClass*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMYBFS);
		enemy1->SetParameters(enemyNode1);
		batEnemyList.push_back(enemy1);
	}

	for (pugi::xml_node enemyNode2 = configParameters.child("entities").child("enemies").child("murcielago1"); enemyNode2; enemyNode2 = enemyNode2.next_sibling("murcielago1"))
	{
		EnemyInClass* enemy1 = (EnemyInClass*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMYBFS);
		enemy1->SetParameters(enemyNode2);
		batEnemyList.push_back(enemy1);
	}

	for (pugi::xml_node enemyNode2 = configParameters.child("entities").child("enemies").child("BOO0"); enemyNode2; enemyNode2 = enemyNode2.next_sibling("BOO0"))
	{
		BOO* enemy1 = (BOO*)Engine::GetInstance().entityManager->CreateEntity(EntityType::BOO);
		enemy1->SetParameters(enemyNode2);

		booEnemyList.push_back(enemy1);
	}
	

	for (pugi::xml_node enemyNode3 = configParameters.child("entities").child("enemies").child("spring0"); enemyNode3; enemyNode3 = enemyNode3.next_sibling("spring0"))
	{
		Spring* springEnemy1 = (Spring*)Engine::GetInstance().entityManager->CreateEntity(EntityType::SPRINGENEMY);
		springEnemy1->SetParameters(enemyNode3);

		springEnemyList.push_back(springEnemy1);
	}
	

	for (pugi::xml_node bulletNode = configParameters.child("entities").child("items").child("bullet"); bulletNode; bulletNode = bulletNode.next_sibling("enemy"))
	{
		bulletParameters = bulletNode;
	}


	SDL_Rect btPos = { 520, 350, 120,20 };

	//guiBt = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "MyButton", btPos, this);
	SDL_Rect layoutBounds = { 0, 0, WWidth, WHeight }; 
	layout = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "Layout", layoutBounds, this);
	layout->isLayout = true; // Mark as layout

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	
	//L06 TODO 3: Call the function to load the map. 
	Engine::GetInstance().map->Load(configParameters.child("map").attribute("path").as_string(), configParameters.child("map").attribute("name").as_string());
	shootFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/shoot.ogg");

	// Texture to highligh mouse position 
	mouseTileTex = Engine::GetInstance().textures.get()->Load("Assets/Maps/MapMetadata.png");

	// Initalize the camera position
	int w, h;
	Engine::GetInstance().window.get()->GetWindowSize(w, h);
	Engine::GetInstance().render.get()->camera.x = 0;
	Engine::GetInstance().render.get()->camera.y = 0;
	WWidth = Engine::GetInstance().window.get()->width;
	WHeight = Engine::GetInstance().window.get()->height;

	Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/peak_music_but_fast.ogg", 1.0f);

	return true;
}



// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

float Scene::Slower(float ogPos, float goalPos, float time)
{
	float acceleration = goalPos - ogPos;
	float speed = ogPos + time * acceleration;
	return speed;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	//L03 TODO 3: Make the camera movement independent of framerate
	float camSpeed = 1;

	shootingTimer += dt / 1000;

	int mapLimitX = 7000;
	int mapLimitY = 1184;
	Engine::GetInstance().render.get()->camera.y = (-player->position.getY() * camSpeed) + WHeight / 2;
	layout->bounds.x = -Engine::GetInstance().render->camera.x;
	layout->bounds.y = -Engine::GetInstance().render->camera.y;
	layout->bounds.w = WWidth;
	layout->bounds.h = WHeight;

	/*if (player->position.getX() > WWidth / (camSpeed * 2) &&
		player->position.getX() < mapLimitX - WWidth / (camSpeed * 2))
	{*/
	Engine::GetInstance().render.get()->camera.x =
		Slower(Engine::GetInstance().render.get()->camera.x, (-player->position.getX() * camSpeed) + WWidth / 2 - offsetX, 0.2f);

	Engine::GetInstance().render.get()->camera.y =
		Slower(Engine::GetInstance().render.get()->camera.y, (-player->position.getY() * camSpeed) + WHeight / 2 - offsetY, 0.2f);
	/*}*/

	WindowManipulation(dt);

	SpringEnemyThings();

	SetCheckpoints();

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		Engine::GetInstance().render.get()->camera.y -= ceil(camSpeed * dt);

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		Engine::GetInstance().render.get()->camera.y += ceil(camSpeed * dt);

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		Engine::GetInstance().render.get()->camera.x -= ceil(camSpeed * dt);

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		Engine::GetInstance().render.get()->camera.x += ceil(camSpeed * dt);

	// L10 TODO 6: Implement a method that repositions the player in the map with a mouse click

	if (Engine::GetInstance().input.get()->GetKeyDown(SDL_SCANCODE_P) == KEY_DOWN && shootingTimer >= 2)
		Shoot();


	if (player->godMode) {
		//Get mouse position and obtain the map coordinate
		Vector2D mousePos = Engine::GetInstance().input.get()->GetMousePosition();
		Vector2D mouseTile = Engine::GetInstance().map.get()->WorldToMap(mousePos.getX() - Engine::GetInstance().render.get()->camera.x,
			mousePos.getY() - Engine::GetInstance().render.get()->camera.y);


		//Render a texture where the mouse is over to highlight the tile, use the texture 'mouseTileTex'
		Vector2D highlightTile = Engine::GetInstance().map.get()->MapToWorld(mouseTile.getX(), mouseTile.getY());
		SDL_Rect rect = { 0,0,32,32 };
		Engine::GetInstance().render.get()->DrawTexture(mouseTileTex,
			highlightTile.getX(),
			highlightTile.getY(),
			&rect);

		// saves the tile pos for debugging purposes
		if (mouseTile.getX() >= 0 && mouseTile.getY() >= 0 || once) {
			tilePosDebug = "[" + std::to_string((int)mouseTile.getX()) + "," + std::to_string((int)mouseTile.getY()) + "] ";
			once = true;
		}

		//If mouse button is pressed modify enemy position
		if (Engine::GetInstance().input.get()->GetMouseButtonDown(1) == KEY_DOWN) {
			player->SetPosition(Vector2D(highlightTile.getX(), highlightTile.getY()));

		}
	}
	if (player->isDead)
	{
		//make a cooldwon for the player to respawn
		player->respawnTimer += dt / 1000;
		if (player->respawnTimer >= 2)
		{
			LoadState();
			player->isDead = false;
			player->isJumping = false;
			player->canDJ = true;
			player->currentAnimation = &player->idleR;
			player->respawnTimer = 0;
		}


		

	}

	return true;
}

void Scene::SpringEnemyThings()
{
	for (Spring* spring : springEnemyList)
	{
		if (player->GetDirection() == Direction::RIGHT)	spring->movementDirection = 1; else spring->movementDirection = -1;
	}
}

void Scene::SetCheckpoints()
{
	if (!hasCheckpointsBeenSetted)
	{
		checkPoint->SetPosition(Vector2D(500, 500));
	}
	hasCheckpointsBeenSetted = true;

	if (player->setCheckPoint)
	{
		SaveState();
	}
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		LoadState();
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		SaveState();

	return ret;
}

// L15 TODO 1: Implement the Load function
void Scene::LoadState() {

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (!result) {
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	pugi::xml_node sceneNode = loadFile.child("config").child("scene");

	pugi::xml_node playerNode = sceneNode.child("entities").child("player");
	Vector2D playerPos = Vector2D(
		playerNode.attribute("x").as_int(),
		playerNode.attribute("y").as_int()
	);
	player->SetPosition(playerPos);

	pugi::xml_node enemiesNode = sceneNode.child("entities").child("enemies");
	for (int i = 0; i < enemyList.size(); i++) {
		std::string enemyNodeName = "enemy" + std::to_string(i);

		pugi::xml_node enemyNode = enemiesNode.child(enemyNodeName.c_str());
		if (enemyNode) {
			Vector2D enemyPos = Vector2D(
				enemyNode.attribute("x").as_int(),
				enemyNode.attribute("y").as_int()
			);
			if(!enemyList[i]->imDead) enemyList[i]->SetPosition(enemyPos);
		}
		else {
			LOG("Enemy node %s not found in config.xml", enemyNodeName.c_str());
		}
	}

	pugi::xml_node springEnemyNode = sceneNode.child("entities").child("enemies");
	for (int i = 0; i < enemyList.size(); i++) {
		std::string enemyNodeName = "spring" + std::to_string(i);

		pugi::xml_node enemyNode = springEnemyNode.child(enemyNodeName.c_str());
		if (enemyNode) {
			Vector2D enemyPos = Vector2D(
				enemyNode.attribute("x").as_int(),
				enemyNode.attribute("y").as_int()
			);
			if (!springEnemyList[i]->imDead) springEnemyList[i]->SetPosition(enemyPos);
		}
		else {
			LOG("Enemy node %s not found in config.xml", enemyNodeName.c_str());
		}
	}

	pugi::xml_node booEnemyNode = sceneNode.child("entities").child("enemies");
	for (int i = 0; i < enemyList.size(); i++) {
		std::string enemyNodeName = "BOO" + std::to_string(i);

		pugi::xml_node enemyNode = booEnemyNode.child(enemyNodeName.c_str());
		if (enemyNode) {
			Vector2D enemyPos = Vector2D(
				enemyNode.attribute("x").as_int(),
				enemyNode.attribute("y").as_int()
			);
			if (!booEnemyList[i]->imDead) booEnemyList[i]->SetPosition(enemyPos);
		}
		else {
			LOG("Enemy node %s not found in config.xml", enemyNodeName.c_str());
		}
	}
	pugi::xml_node murcielagoEnemyNode = sceneNode.child("entities").child("enemies");
	for (int i = 0; i < enemyList.size(); i++) {
		std::string enemyNodeName = "murcielago" + std::to_string(i);

		pugi::xml_node enemyNode = murcielagoEnemyNode.child(enemyNodeName.c_str());
		if (enemyNode) {
			Vector2D enemyPos = Vector2D(
				enemyNode.attribute("x").as_int(),
				enemyNode.attribute("y").as_int()
			);
			if (!batEnemyList[i]->imDead) batEnemyList[i]->SetPosition(enemyPos);
		}
		else {
			LOG("Enemy node %s not found in config.xml", enemyNodeName.c_str());
		}
	}
}// L15 TODO 2: Implement the Save function
void Scene::SaveState() {

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (!result) {
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	pugi::xml_node sceneNode = loadFile.child("config").child("scene");

	pugi::xml_node playerNode = sceneNode.child("entities").child("player");
	if (playerNode) {
		playerNode.attribute("x").set_value(player->GetPosition().getX() - player->texW / 2);
		playerNode.attribute("y").set_value(player->GetPosition().getY());
	}
	else {
		playerNode = sceneNode.child("entities").append_child("player");
		playerNode.append_attribute("x") = player->GetPosition().getX() - player->texW / 2;
		playerNode.append_attribute("y") = player->GetPosition().getY() - player->texH / 2;
	}

	pugi::xml_node enemiesNode = sceneNode.child("entities").child("enemies");

	for (int i = 0; i < enemyList.size(); i++) {
		std::string enemyNodeName = "enemy" + std::to_string(i);

		pugi::xml_node enemyNode = enemiesNode.child(enemyNodeName.c_str());
		if (!enemyNode) {
			enemyNode = enemiesNode.append_child(enemyNodeName.c_str());
		}
		if (!enemyList[i]->imDead)
		{
			enemyNode.attribute("x").set_value(enemyList[i]->GetPosition().getX() - player->texW / 2);
			enemyNode.attribute("y").set_value(enemyList[i]->GetPosition().getY() - player->texH / 2);
		}
		
	}


	pugi::xml_node springEnemiesNode = sceneNode.child("entities").child("enemies");

	for (int i = 0; i < springEnemyList.size(); i++) {
		std::string enemyNodeName = "spring" + std::to_string(i);

		pugi::xml_node enemyNode = springEnemiesNode.child(enemyNodeName.c_str());
		if (!enemyNode) {
			enemyNode = springEnemiesNode.append_child(enemyNodeName.c_str());
		}
		if (!springEnemyList[i]->imDead)
		{
			enemyNode.attribute("x").set_value(springEnemyList[i]->GetPosition().getX() - player->texW / 2);
			enemyNode.attribute("y").set_value(springEnemyList[i]->GetPosition().getY() - player->texH / 2);
		}
	}

	pugi::xml_node booEnemiesNode = sceneNode.child("entities").child("enemies");

	for (int i = 0; i < booEnemyList.size(); i++) {
		std::string enemyNodeName = "BOO" + std::to_string(i);

		pugi::xml_node enemyNode = booEnemiesNode.child(enemyNodeName.c_str());
		if (!enemyNode) {
			enemyNode = booEnemiesNode.append_child(enemyNodeName.c_str());
		}

		if (!booEnemyList[i]->imDead)
		{
			enemyNode.attribute("x").set_value(booEnemyList[i]->GetPosition().getX() - player->texW / 4);
			enemyNode.attribute("y").set_value(booEnemyList[i]->GetPosition().getY() - player->texH / 4);

		}
	}
	pugi::xml_node batEnemiesNode = sceneNode.child("entities").child("enemies");

	for (int i = 0; i < batEnemyList.size(); i++) {
		std::string enemyNodeName = "murcielago" + std::to_string(i);

		pugi::xml_node enemyNode = batEnemiesNode.child(enemyNodeName.c_str());
		if (!enemyNode) {
			enemyNode = batEnemiesNode.append_child(enemyNodeName.c_str());
		}

		if (!batEnemyList[i]->imDead)
		{
			enemyNode.attribute("x").set_value(batEnemyList[i]->GetPosition().getX() - player->texW / 4);
			enemyNode.attribute("y").set_value(batEnemyList[i]->GetPosition().getY() - player->texH / 4);
		}
	}


	if (!loadFile.save_file("config.xml")) {
		LOG("Failed to save game state to file.");
	}
	else {
		LOG("Game state saved successfully.");
	}
}
void Scene::Shoot()
{
	Vector2D playerPos = player->GetPosition();

	Bullet* bullet = (Bullet*)Engine::GetInstance().entityManager->CreateEntity(EntityType::BULLET);
	bullet->SetParameters(bulletParameters);
	bullet->Start();
	Vector2D Offset = { 65, 0 };
	Vector2D Offset2 = { -65, 0 };
	Engine::GetInstance().audio.get()->PlayFx(shootFxId);

	if (player->GetDirection() == Direction::RIGHT)
	{
		std::cout << "Bullet right";
		bullet->SetVelocity(Direction::RIGHT);

		bullet->SetPosition(playerPos + Offset);
	}
	else if(player->GetDirection() == Direction::LEFT)
	{
		std::cout << "Bullet left";

		bullet->leftBullet = true;
		bullet->SetPosition(playerPos + Offset2);
		bullet->SetVelocity(Direction::LEFT);
	}

	shootingTimer = 0;
}
// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");
	return true;
}
void Scene::WindowManipulation(float dt)
{
	int windowX, windowY;
	int screenWidth, screenHeight;
	int windowWidth, windowHeight;

	Engine::GetInstance().window.get()->GetWindowPosition(windowX, windowY);

	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);
	screenWidth = displayMode.w;
	screenHeight = displayMode.h;

	SDL_GetWindowSize(Engine::GetInstance().window.get()->GetSDLWindow(), &windowWidth, &windowHeight);

	int windowMoveSpeed = 2; 

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_I) == KEY_REPEAT)
		windowY -= windowMoveSpeed * dt;

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_K) == KEY_REPEAT)
		windowY += windowMoveSpeed * dt;

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_J) == KEY_REPEAT)
		windowX -= windowMoveSpeed * dt;

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_L) == KEY_REPEAT)
		windowX += windowMoveSpeed * dt;

	windowX = std::max(0, std::min(windowX, screenWidth - windowWidth));
	windowY = std::max(0, std::min(windowY, screenHeight - windowHeight));

	SDL_SetWindowPosition(Engine::GetInstance().window.get()->GetSDLWindow(), windowX, windowY);
}


// Return the player position
Vector2D Scene::GetPlayerPosition()
{
	return player->GetPosition();
}
