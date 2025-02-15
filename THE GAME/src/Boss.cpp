#include "Boss.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "Map.h"
#include "EntityManager.h"

Boss::Boss() : Entity(EntityType::BOSS) {
    health = 4;
    isActive = false;
    fadeInProgress = 0.0f;
    fadeOutProgress = 0.0f;
    texture = nullptr;
    currentAnimation = nullptr;
}

Boss::~Boss() {
    if (pbody != nullptr) {
        Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
    }
    if (texture != nullptr) {
        Engine::GetInstance().textures.get()->UnLoad(texture);
    }
}

bool Boss::Awake() {
    return true;
}

bool Boss::Start() {
    // Hardcoded values for the boss
    texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/Boss.png");

    position.setX(5432);
    position.setY(1031);

    // Initialize dimensions
    texW = 120;
    texH = 120;
    
    currentAnimation = nullptr;
    if (pbody == NULL)
    {
        pbody = Engine::GetInstance().physics.get()->CreateCircle(
            (int)position.getX() + texW / 2,
            (int)position.getY() + texH / 2,
            texW / 2, // Radius based on texture width
            bodyType::DYNAMIC
        );
        pbody->ctype = ColliderType::BOSS;
        pbody->listener = this;

        // No gravity for the boss
        pbody->body->SetGravityScale(0);
    }
    return true;
}


bool Boss::Update(float dt) {

    if (!isActive) {
        return true;
    }

    if (fadeInProgress < 1.0f) {
        fadeInProgress += dt * 0.5f;
        Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), nullptr, fadeInProgress);
        return true;
    }

    Player* player = Engine::GetInstance().scene.get()->player;
    Vector2D playerPos = player->GetPosition();
    Vector2D dir = playerPos - position;

    dir.normalized();

    float speed = 0.01f;
    b2Vec2 velocity = b2Vec2(dir.getX() * speed, dir.getY() * speed);
    pbody->body->SetLinearVelocity(velocity);

    b2Transform pbodyPos = pbody->body->GetTransform();
    position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - 60);
    position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - 60);

    // Draw the boss
    Engine::GetInstance().render.get()->DrawTexture(
        texture, (int)position.getX(), (int)position.getY()
    );

    return true;
}

bool Boss::CleanUp()
{
    if (pbody != nullptr) {
        Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
    }
    if (texture != nullptr) {
        Engine::GetInstance().textures.get()->UnLoad(texture);
    }
}

void Boss::OnCollision(PhysBody* physA, PhysBody* physB) {
    if (physB->ctype == ColliderType::BULLET) {
        health--;
        if (health <= 0) {
            Die();
        }
    }
}

void Boss::Die() {
    fadeOutProgress = 1.0f;
    isActive = false;
    Engine::GetInstance().entityManager.get()->DestroyEntity(this);
}

void Boss::Activate(Vector2D spawnPosition) {
    isActive = true;
    position = spawnPosition;
    if (pbody != nullptr) {
        SetPosition(spawnPosition);
    }
    fadeInProgress = 0.0f;
}

void Boss::SetPosition(Vector2D pos) {
    pos.setX(pos.getX() + texW / 2);
    pos.setY(pos.getY() + texH / 2);
    b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
    pbody->body->SetTransform(bodyPos, 0);
}
