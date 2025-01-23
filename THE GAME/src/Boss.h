#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"
#include "Player.h"
#include "Bullet.h"

struct SDL_Texture;

class Boss : public Entity {
public:
    Boss();
    virtual ~Boss();

    bool Awake();

    bool Start();
    bool Update(float dt);

	bool CleanUp();

    void OnCollision(PhysBody* physA, PhysBody* physB);
    void Die();

    void Activate(Vector2D spawnPosition);

    void SetPosition(Vector2D pos);

    void SetParameters(pugi::xml_node parameters) {
        this->parameters = parameters;
    }

public:
    int health;
    bool isActive;
    float fadeInProgress;
    float fadeOutProgress;

    int texW, texH;

    Vector2D position;
    PhysBody* pbody;
    Animation idleR;
    Animation idleL;
    Animation* currentAnimation;
    SDL_Texture* texture;

    pugi::xml_node parameters;

};

