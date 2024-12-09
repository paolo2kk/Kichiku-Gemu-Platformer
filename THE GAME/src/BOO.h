#pragma once
#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"
#include "Player.h"
struct SDL_Texture;
class BOO : public Entity
{
public:
	BOO();
	virtual ~BOO();
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

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

public:
private:
	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;
	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation idleL;
	Animation idleR;
	Animation scaredR;
	Animation scaredL;

	PhysBody* pbody;
	Pathfinding* pathfinding;

	int buscando = 0;
	bool showPath = false;
	bool lookingPlayer = false;
	bool isDead = false;
	

	Direction lookDirection;
	States stat = States::WALKING_R;

};