#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Physics.h"

struct SDL_Texture;

class Bullet : public Entity
{
public:

	Bullet();
	virtual ~Bullet();

	bool Awake();

	bool Start();

	void SetVelocity(Vector2D newVelocity);

	bool Update(float dt);

	bool CleanUp();

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
		Start();
	}

	void SetPosition(Vector2D newPosition) {
		position = newPosition;
		pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(newPosition.getX()), PIXEL_TO_METERS(newPosition.getY())), 0);
	}

	void OnCollision(PhysBody* physA, PhysBody* physB);

public:

	bool isPicked = false;
	bool leftBullet = false;

private:

	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;
	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation idle;

	//L08 TODO 4: Add a physics to an item
	PhysBody* pbody;
	Vector2D velocity; 
};
