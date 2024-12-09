#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Physics.h"
#include "Player.h".
#include "Timer.h"

struct SDL_Texture;

class Bullet : public Entity
{
public:

	Bullet();
	virtual ~Bullet();

	bool Awake();

	bool Start();

	void SetVelocity(Direction direction);


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

	Vector2D GetPosition() {
		b2Vec2 bodyPos = pbody->body->GetTransform().p;
		Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
		return pos;
	}


	virtual void OnCollision(PhysBody* physA, PhysBody* physB) override;


	//virtual void OnCollisionEnd(PhysBody* physA, PhysBody* physB) override;

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

	Timer* timerToDie;
	PhysBody* pbody;
	Vector2D velocity; 
};
