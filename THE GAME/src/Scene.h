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
#include "GuiCheckBox.h"
#include "GuiSlider.h"

struct SDL_Texture;

enum UIStates
{
	MAINMENU,
	PLAYING,
	PAUSED
};

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

	void StateManagement(UIStates uiStates);

	void MainMenu();

	void PauseMenu(float dt);

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

	void SetPlayerCheckpointPos();

	void Shoot();

	void CreateEnemies2();

	void WindowManipulation(float dt);

	void ResetWholeGame();

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

	GuiControlButton* resumeBt;
	GuiControlButton* backtotitleBt;
	GuiControlButton* settingsBt;
	GuiControlButton* quitgameBT2;
	GuiControlButton* layout;

	std::vector<GuiControlButton*> guiButtons;
	GuiControlButton* menuLayout;


	GuiControlButton* playBt;
	GuiControlButton* playGameBT;
	GuiControlButton* creditsButton;
	GuiControlButton* settingsButton;
	GuiControlButton* exitGameBT;
	GuiCheckBox* myCheckBox;
	GuiSlider* musicSlider;
	GuiSlider* fxSlider;
	GuiControlButton* newGameBT;

	std::vector<GuiControlButton*> guiButtonsMM;

	GuiControlButton* returnSTBT;

	std::vector<GuiControlButton*> guiButtonsSettings;

	int musicVolume;
	int fxVolume;

	UIStates uiState = MAINMENU;
	UIStates currentState;

	float shootingTimer = 0;
	bool help = false;
	bool settings = false;
	bool credits = false;
	bool isPaused = false;
	bool fullScreen = false;

	
	template<typename T>
	void ResetEnemyList(std::vector<T*>& enemyList, pugi::xml_node& sceneNode, const std::string& baseName, EntityType entityType);

	template<typename T>
	T* CreateNewEnemy();

};