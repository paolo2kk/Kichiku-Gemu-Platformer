#pragma once

#include "Input.h"
#include "Render.h"

enum class EntityType
{
	PLAYER,
	ITEM,
	ENEMY,
	ENEMYBFS,
	UNKNOWN,
	BULLET,
	SPRINGENEMY,
	BOO,
	CHECKPOINT,
	HP,
	BOSS
};

enum class States
{
	IDLE,
	WALKING_R,
	WALKING_L,
	JUMPING,
	FALLING,
	DEAD
};

class PhysBody;

class Entity
{
public:

	Entity(EntityType type) : type(type), active(true) {}

	virtual bool Awake()
	{
		return true;
	}

	virtual bool Start()
	{
		return true;
	}

	virtual bool Update(float dt)
	{
		return true;
	}

	virtual bool CleanUp()
	{
		return true;
	}

	void Enable()
	{
		if (!active)
		{
			active = true;
			Start();
		}
	}

	void Disable()
	{
		if (active)
		{
			active = false;
			CleanUp();
		}
	}

	virtual void OnCollision(PhysBody* physA, PhysBody* physB) {
	};

	virtual void OnCollisionEnd(PhysBody* physA, PhysBody* physB) {

	};

public:
	bool stop = false;

	std::string name;
	EntityType type;
	bool active = true;
	bool imDead = false;
	// Possible properties, it depends on how generic we
	// want our Entity class, maybe it's not renderable...
	Vector2D position;       
	bool renderable = true;
	bool toDestroy = false;

};