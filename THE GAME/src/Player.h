#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Box2D/Box2D.h"
#include "Animation.h"
#include "Timer.h"

struct SDL_Texture;

enum class Direction
{
	LEFT,
	RIGHT
};

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	void SetMass(float newMass);

	bool CleanUp();

	// L08 TODO 6: Define OnCollision function for the player. 
	void OnCollision(PhysBody* physA, PhysBody* physB);

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	void SetPosition(Vector2D pos);


	Direction GetDirection() const
	{
		return direction;
	}

	Vector2D GetPosition();

	void Bounce();

	void Respaw();

public:

	//Declare player parameters
	float speed = 5.0f;
	SDL_Texture* texture = NULL;
	int texW, texH;
	float holdTime = 0.0f; // Tiempo que se mantiene presionada la tecla
	float maxHoldTime = 1.0f;
	//Audio fx
	int pickCoinFxId;
	int jumpFxId;     
	int stepFxId;     
	int shootFxId;   
	int playerdieFxId;
	float respawnTimer = 0;
	
	bool stop = false;

	// L08 TODO 5: Add physics to the player - declare a Physics body
	PhysBody* pbody;
	float jumpForce = 100; // The force to apply when jumping
	bool isJumping = false; // Flag to check if the player is currently jumping

	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation idleR;
	Animation idleL;
	Animation walk;
	Animation right;
	Animation dead;
	Direction direction;

	bool isDead = false;
	bool canDJ = true;
	bool setCheckPoint = false;
	bool isWalking = false;

	bool godMode = false;

	int lives = 3;

	int gameOverFxId;

	bool checkpointActivated = false;
	Vector2D initialPosition;

	Timer walksoundTimer;

	bool loadLevel2 = false;
	
};