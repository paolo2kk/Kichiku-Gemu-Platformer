#include "BOO.h"
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
#include <tracy/Tracy.hpp>

BOO::BOO() : Entity(EntityType::ENEMYBFS)
{
}

BOO::~BOO() {
	delete pathfinding;
}

bool BOO::Awake() {
	return true;
}

bool BOO::Start() {
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	idleL.LoadAnimations(parameters.child("animations").child("idleL"));
	idleR.LoadAnimations(parameters.child("animations").child("idleR"));
	scaredL.LoadAnimations(parameters.child("animations").child("scaredL"));
	scaredR.LoadAnimations(parameters.child("animations").child("scaredR"));
	lookDirection = Direction::LEFT;
	currentAnimation = &idleL;

	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);
	pbody->ctype = ColliderType::ENEMYBFS;
	pbody->body->SetGravityScale(0);
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	pathfinding = new Pathfinding();
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
	pathfinding->ResetPath(tilePos);

	return true;
}

bool BOO::Update(float dt)
{
	ZoneScoped;

	Player* player = Engine::GetInstance().scene.get()->player;

	if (!player) return true;

	Vector2D playerPos = player->GetPosition();
	Vector2D enemyPos = GetPosition();

	float distanceX = abs(playerPos.getX() - enemyPos.getX());
	float distanceY = abs(playerPos.getY() - enemyPos.getY());

	const int blockSize = 32;
	float maxDistance = 25 * blockSize;

	if (distanceX <= maxDistance && distanceY <= maxDistance) {
		if (buscando <= 40) {
			pathfinding->PropagateAStar(MANHATTAN);
			buscando++;
		}
		else {
			Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(enemyPos.getX(), enemyPos.getY());
			pathfinding->ResetPath(tilePos);
			buscando = 0;
		}
		if (player->GetPosition().getX() >= this->GetPosition().getX()) lookDirection = Direction::RIGHT; else lookDirection = Direction::LEFT;

		if (pathfinding->pathTiles.size() > 0) {
			Vector2D nextTile = pathfinding->pathTiles.front();
			Vector2D nextPos = Engine::GetInstance().map->MapToWorld(nextTile.getX(), nextTile.getY());
			Vector2D dir = nextPos - enemyPos;
			dir.normalized();

			if (player->GetDirection() == lookDirection)
			{
				float speed = 0.02f;
				b2Vec2 velocity = b2Vec2(dir.getX() * speed, dir.getY() * speed);
				pbody->body->SetLinearVelocity(velocity);
			}
			else {
				pbody->body->SetLinearVelocity(b2Vec2(0, 0));

				switch (lookDirection)
				{
				case Direction::LEFT:
					currentAnimation = &idleL;
					break;
				case Direction::RIGHT:
					currentAnimation = &idleR;
					break;
				}
			}
		}
	}
	else {
		pbody->body->SetLinearVelocity(b2Vec2(0, 0));
	}

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	currentAnimation->Update(dt);

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
	{
		if (showPath)
		{
			showPath = false;
		}
		else
			showPath = true;
	}
	if (showPath)
	{
		pathfinding->DrawPath();
	}

	return true;
}

bool BOO::CleanUp()
{
	Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
	return true;
}

void BOO::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D BOO::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

void BOO::ResetPath() {
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
	pathfinding->ResetPath(tilePos);
}

void BOO::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype) {
	case ColliderType::PLAYER:
	{
		break;
	}
	default:
		break;
	}
}

void BOO::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		break;
	}
}
