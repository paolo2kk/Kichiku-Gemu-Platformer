#include "Enemy.h"
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

Enemy::Enemy() : Entity(EntityType::ENEMY)
{

}

Enemy::~Enemy() {
	
	delete pathfinding;
}

bool Enemy::Awake() {
	return true;
}

bool Enemy::Start() {

	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	currentAnimation = &idle;
	
	//Add a physics to an item - initialize the physics body
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);


	//Assign collider type
	pbody->ctype = ColliderType::ENEMY;
	pbody->listener = this;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	// Initialize pathfinding
	pathfinding = new Pathfinding();
	ResetPath();


	return true;
}

bool Enemy::Update(float dt)
{


	Player* player = Engine::GetInstance().scene.get()->player;

	if (player != nullptr) {
		CheckCollisionWithPlayer(player);
	}

	if (isDead) {
		Engine::GetInstance().entityManager.get()->DestroyEntity(this);
		return false;
	}



	/*
	timeSinceDirectionChange += dt;


	if (timeSinceDirectionChange >= directionChangeInterval) {
		movementDirection *= -1;
		timeSinceDirectionChange = 0.0f;
	}

	timeSinceLastJump += dt;


	if (timeSinceLastJump >= jumpInterval) {

		b2Vec2 jumpImpulse = b2Vec2(0.0f, jumpForce);
		pbody->body->ApplyLinearImpulseToCenter(jumpImpulse, true);


		timeSinceLastJump = 0.0f;
	}*/

	if (!isDead) {

		b2Vec2 velocity = b2Vec2(0, 0);
		if (buscando <= 20)
		{
			pathfinding->PropagateAStar(EUCLIDEAN);
			buscando++;
		}
		else
		{
			Vector2D pos = GetPosition();
			Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
			pathfinding->ResetPath(tilePos);
			buscando = 0;
		}

		Vector2D PosInMap = Engine::GetInstance().map->WorldToMap(position.getX(), position.getY());

		//if pathfinding is done, move the next tile, make it slower
		if (pathfinding->pathTiles.size() > 0)
		{
			Vector2D nextTile = pathfinding->pathTiles.front();
			Vector2D nextPos = Engine::GetInstance().map->MapToWorld(nextTile.getX(), nextTile.getY());
			Vector2D dir = nextPos - position;
			dir.normalized();


			float velocidad = 0.03f;
			velocity = b2Vec2(dir.getX() * velocidad, 0);

			pbody->body->SetLinearVelocity(velocity);
		}


		b2Vec2 bodyPos = pbody->body->GetTransform().p;
		bodyPos.x += PIXEL_TO_METERS(2.0f * movementDirection);
		pbody->body->SetTransform(bodyPos, pbody->body->GetAngle());


		b2Transform pbodyPos = pbody->body->GetTransform();
		position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
		position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);


		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
		currentAnimation->Update();

		//haz que si pulsas F1 se muestre el pathfinding en el mapa y si esta activado y le das al F1 se desactive
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
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
	
}


bool Enemy::CleanUp()
{
	Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
	return true;
}

void Enemy::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Enemy::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

void Enemy::ResetPath() {
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
	pathfinding->ResetPath(tilePos);
}

void Enemy::CheckCollisionWithPlayer(Player* player)
{
	
	Vector2D playerPos = player->GetPosition();
	int playerWidth = player->texW;
	int playerHeight = player->texH;

	Vector2D enemyPos = this->GetPosition();
	int enemyWidth = this->texW;
	int enemyHeight = this->texH;

	
	bool isCollidingHorizontally = (playerPos.getX() + playerWidth/2 > enemyPos.getX() &&
		playerPos.getX() < enemyPos.getX() + enemyWidth);

	
	bool isCollidingVertically = (playerPos.getY() + playerHeight/2 >= enemyPos.getY() &&
                              playerPos.getY() < enemyPos.getY() + enemyHeight);


	if (isCollidingHorizontally && isCollidingVertically) {
		
	this->isDead = true;
		
		player->Bounce();
	}
}



void Enemy::Die()
{
	if (toDestroy)
	{
		isDead = true;
	}
}

void Enemy::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		LOG("Collided with player - DESTROY");
		//Engine::GetInstance().entityManager.get()->DestroyEntity(this);
		break;
	case ColliderType::BULLET:
		LOG("Collided with Bullet");
		Engine::GetInstance().entityManager.get()->DestroyEntity(this);
		break;
	}
}
void Enemy::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		LOG("Collision player");
		break;
	}
}

