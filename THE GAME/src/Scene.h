#pragma once

#include "Module.h"
#include "Player.h"
#include "Enemy.h"
#include "CheckPoint.h"
#include "Bullet.h"
#include <vector>

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Return the player position
	Vector2D GetPlayerPosition();

	void Shoot();

	void WindowManipulation(float dt);

public:
	// Get tilePosDebug value
	std::string GetTilePosDebug() {
		return tilePosDebug;

	}

public:
	SDL_Texture* mouseTileTex = nullptr;
	std::string tilePosDebug = "[0,0]";
	bool once = false;

	//L03: TODO 3b: Declare a Player attribute
	Player* player;
	std::vector<Enemy*> enemyList;
	std::vector<CheckPoint*> checkPointList;
	std::vector<Bullet*> bulletList;
	pugi::xml_node bulletParameters;
};