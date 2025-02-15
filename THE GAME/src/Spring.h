#pragma once
#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"
#include "Player.h"

struct SDL_Texture;
class Spring : public Entity
{
public:
	Spring();
	virtual ~Spring();
	bool Awake();
	bool Start();
	bool Update(float dt);
	int CheckDirection(Player* player);
	bool CleanUp();
	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}
	void SetPosition(Vector2D pos);
	Vector2D GetPosition();
	void ResetPath();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);
	int movementDirection = -1;

public:
private:

	float timeSinceLastJump = 0.0f;
	int jumpForce = 20;
	int movementDirectionCoefficient = 10;

	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;
	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation idle;
	PhysBody* pbody;
	Pathfinding* pathfinding;

	int buscando = 0;
	bool showPath = false;
	float jumpForceMax = 10.0f; 
	float jumpTimeFactor = 1.0f; 
	float progressionFactor = 0.0f;
	bool isDead = false;
	States stat = States::WALKING_R;

};