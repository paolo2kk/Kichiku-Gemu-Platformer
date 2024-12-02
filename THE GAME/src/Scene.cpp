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
	for (pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy"))
	{
		Enemy* enemy = (Enemy*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY);
		enemy->SetParameters(enemyNode);
	 	enemyList.push_back(enemy);
	}

	for (pugi::xml_node CheckPointNode = configParameters.child("entities").child("items").child("checkpoint"); CheckPointNode; CheckPointNode = CheckPointNode.next_sibling("CheckPoint"))
	{
		CheckPoint* checkPoint = (CheckPoint*)Engine::GetInstance().entityManager->CreateEntity(EntityType::CHECKPOINT);
		checkPoint->SetParameters(CheckPointNode);
		
	}

	for (pugi::xml_node enemyNode1 = configParameters.child("entities").child("enemies").child("murcielago"); enemyNode1; enemyNode1 = enemyNode1.next_sibling("murcielago"))
	{
		EnemyInClass* enemy1 = (EnemyInClass*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMYBFS);
		enemy1->SetParameters(enemyNode1);
	}

	for (pugi::xml_node bulletNode = configParameters.child("entities").child("items").child("bullet"); bulletNode; bulletNode = bulletNode.next_sibling("enemy"))
	{
		bulletParameters = bulletNode;
	}

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	//L06 TODO 3: Call the function to load the map. 
	Engine::GetInstance().map->Load(configParameters.child("map").attribute("path").as_string(), configParameters.child("map").attribute("name").as_string());

	// Texture to highligh mouse position 
	mouseTileTex = Engine::GetInstance().textures.get()->Load("Assets/Maps/MapMetadata.png");

	// Initalize the camera position
	int w, h;
	Engine::GetInstance().window.get()->GetWindowSize(w, h);
	Engine::GetInstance().render.get()->camera.x = 0;
	Engine::GetInstance().render.get()->camera.y = 0;

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	//L03 TODO 3: Make the camera movement independent of framerate
	float camSpeed = 1;

	WindowManipulation(dt);

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		Engine::GetInstance().render.get()->camera.y -= ceil(camSpeed * dt);

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		Engine::GetInstance().render.get()->camera.y += ceil(camSpeed * dt);

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		Engine::GetInstance().render.get()->camera.x -= ceil(camSpeed * dt);

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		Engine::GetInstance().render.get()->camera.x += ceil(camSpeed * dt);

	// L10 TODO 6: Implement a method that repositions the player in the map with a mouse click
	
	if (Engine::GetInstance().input.get()->GetKeyDown(SDL_SCANCODE_P) == KEY_DOWN)
		Shoot();



	//Get mouse position and obtain the map coordinate
	Vector2D mousePos = Engine::GetInstance().input.get()->GetMousePosition();
	Vector2D mouseTile = Engine::GetInstance().map.get()->WorldToMap(mousePos.getX() - Engine::GetInstance().render.get()->camera.x,
																     mousePos.getY() - Engine::GetInstance().render.get()->camera.y);


	//Render a texture where the mouse is over to highlight the tile, use the texture 'mouseTileTex'
	Vector2D highlightTile = Engine::GetInstance().map.get()->MapToWorld(mouseTile.getX(),mouseTile.getY());
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
		enemyList[0]->SetPosition(Vector2D(highlightTile.getX(), highlightTile.getY()));
		enemyList[0]->ResetPath();
	}

	CheckEntitesErase();

	return true;
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

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	pugi::xml_node sceneNode = loadFile.child("config").child("scene");

	//Read XML and restore information

	//Player position
	Vector2D playerPos = Vector2D(sceneNode.child("entities").child("player").attribute("x").as_int(),
		sceneNode.child("entities").child("player").attribute("y").as_int());
	player->SetPosition(playerPos);

	//enemies
	// ...

}

// L15 TODO 2: Implement the Save function
void Scene::SaveState() {

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	pugi::xml_node sceneNode = loadFile.child("config").child("scene");

	//Save info to XML 

	//Player position
	sceneNode.child("entities").child("player").attribute("x").set_value(player->GetPosition().getX());
	sceneNode.child("entities").child("player").attribute("y").set_value(player->GetPosition().getY());

	//enemies
	// ...

	//Saves the modifications to the XML 
	loadFile.save_file("config.xml");
}

void Scene::Shoot()
{
	Vector2D playerPos = player->GetPosition();

	Bullet* bullet = (Bullet*)Engine::GetInstance().entityManager->CreateEntity(EntityType::BULLET);
	bullet->SetParameters(bulletParameters);
	bullet->Start();
	Vector2D Offset = { 65, 32 };

	if (player->GetDirection() == Direction::RIGHT)
	{
		bullet->SetPosition(playerPos + Offset);
	}	
	else 
	{
		bullet->leftBullet = true;
		bullet->SetPosition(playerPos - Offset);
	}
	bulletList.push_back(bullet);
}

void Scene::CheckEntitesErase()
{
	for (Enemy* enemy : enemyList)
	{
		if (enemy->toDestroy == true)
		{
			Engine::GetInstance().entityManager->DestroyEntity(enemy);
		}
	}
	for (Bullet* bullet : bulletList)
	{
		if (bullet->toDestroy == true)
		{
			Engine::GetInstance().entityManager->DestroyEntity(bullet);
		}
	}
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
