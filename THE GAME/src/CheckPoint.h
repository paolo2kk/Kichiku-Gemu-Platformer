#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Physics.h"

struct SDL_Texture;

class CheckPoint : public Entity
{
public:

	CheckPoint();
	virtual ~CheckPoint();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void SetPosition(Vector2D newPosition) {
		position = newPosition;
		pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(newPosition.getX()), PIXEL_TO_METERS(newPosition.getY())), 0);
	}

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

public:

	bool isPicked = false;
	PhysBody* pbody;

private:

	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;
	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation idle;

	//L08 TODO 4: Add a physics to an item
};
