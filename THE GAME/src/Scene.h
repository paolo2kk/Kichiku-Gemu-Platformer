#pragma once

#include "Module.h"
#include "Player.h"
#include "Enemy.h"
#include "CheckPoint.h"
#include "Bullet.h"
#include <vector>
#include "GuiControlButton.h"
#include "Spring.h"
#include "BOO.h"
#include "Murcielago.h"

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

	void SpringEnemyThings();

	float Slower(float ogPos, float goalPos, float time);

	void SetCheckpoints();

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Return the player position
	Vector2D GetPlayerPosition();

	//L15 TODO 1: Implement the Load function
	void LoadState();
	//L15 TODO 2: Implement the Save function
	void SaveState();

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
	CheckPoint* checkPoint;
	CheckPoint* checkPoint2;
	bool hasCheckpointsBeenSetted = false;
	//L03: TODO 3b: Declare a Player attribute
	Player* player;
	std::vector<Enemy*> enemyList;
	std::vector<CheckPoint*> checkPointList;
	pugi::xml_node bulletParameters;
	std::vector<Spring*> springEnemyList;
	std::vector<BOO*> booEnemyList;
	std::vector<EnemyInClass*> batEnemyList;

	int WWidth;
	int WHeight;
	float offsetX = 350;
	float offsetY = 450;
	int shootFxId;

	GuiControlButton* guiBt;
	GuiControlButton* layout;

	float shootingTimer = 0;
	

};