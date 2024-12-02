#pragma once
#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"
struct SDL_Texture;
class EnemyInClass : public Entity
{
public:
	EnemyInClass();
	virtual ~EnemyInClass();
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

public:
private:
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
	
	States stat = States::WALKING_R;

};