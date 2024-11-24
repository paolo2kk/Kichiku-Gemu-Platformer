#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"
#include "Player.h"

struct SDL_Texture;

class Enemy : public Entity
{
public:

	Enemy();
	virtual ~Enemy();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	void SetPosition(Vector2D pos);

	Vector2D GetPosition();

	void ResetPath();

	void CheckCollisionWithPlayer(Player* player);

public:

private:

	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;
	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation idle;
	PhysBody* pbody;
	PhysBody* pbody2;
	Pathfinding* pathfinding;

	float timeSinceDirectionChange = 0.0f;
	float directionChangeInterval = 4500.0f; 
	int movementDirection = 1;

	float timeSinceLastJump = 0.0f;    
	float jumpInterval = 2000.0f;         
	float jumpForce = -2.5f;

	bool isDead = false;
};
