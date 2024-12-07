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

	showPath = true;


	return true;
}

bool Enemy::Update(float dt)
{
	
	Player* player = Engine::GetInstance().scene.get()->player;

	if (isDead) {
		Engine::GetInstance().entityManager.get()->DestroyEntity(this);
		return false;
	}

	
	if (!isDead) {
		
		Vector2D playerPos = player->GetPosition();
		Vector2D enemyPos = GetPosition();

		float distanceX = abs(playerPos.getX() - enemyPos.getX());
		float distanceY = abs(playerPos.getY() - enemyPos.getY());

		
		const int blockSize = 32; 
		float maxDistance = 10 * blockSize; 

		if (distanceX <= maxDistance && distanceY <= maxDistance) {
			
			if (buscando <= 20) {
				pathfinding->PropagateAStar(EUCLIDEAN);
				buscando++;
			}
			else {
				Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(enemyPos.getX(), enemyPos.getY());
				pathfinding->ResetPath(tilePos);
				buscando = 0;
			}

			
			if (pathfinding->pathTiles.size() > 0) {
				Vector2D nextTile = pathfinding->pathTiles.front();
				Vector2D nextPos = Engine::GetInstance().map->MapToWorld(nextTile.getX(), nextTile.getY());
				Vector2D dir = nextPos - enemyPos;
				dir.normalized();

				float velocidad = 0.03f; 
				b2Vec2 velocity = b2Vec2(dir.getX() * velocidad, 0);

				pbody->body->SetLinearVelocity(velocity);
			}
			else {
				pbody->body->SetLinearVelocity(b2Vec2(0, 0));
			}
		}
		else {
			pbody->body->SetLinearVelocity(b2Vec2(0, 0));
		}

		b2Transform pbodyPos = pbody->body->GetTransform();
		position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
		position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
		currentAnimation->Update();

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
	}

	return true;
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

/*void Enemy::CheckCollisionWithPlayer(Player* player)
{
	
	Vector2D playerPos = player->GetPosition();
	int playerWidth = player->texW;
	int playerHeight = player->texH;

	Vector2D enemyPos = this->GetPosition();
	int enemyWidth = this->texW;
	int enemyHeight = this->texH;

	
	bool isCollidingHorizontally = (playerPos.getX() + playerWidth > enemyPos.getX() &&
		playerPos.getX() < enemyPos.getX() + enemyWidth);

	
	bool isCollidingVertically = (playerPos.getY() + playerHeight/2 >= enemyPos.getY() &&
                              playerPos.getY() < enemyPos.getY() + enemyHeight);


	if (isCollidingHorizontally && isCollidingVertically) {
		
	this->isDead = true;
		
		player->Bounce();
	}
}
*/


void Enemy::Die()
{
	if (toDestroy)
	{
		isDead = true;
	}
}

void Enemy::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype) {
	case ColliderType::PLAYER:
	{
		Player* player = dynamic_cast<Player*>(physB->listener);
		if (player != nullptr) {
			Vector2D playerPos = player->GetPosition();
			Vector2D enemyPos = GetPosition();

			if (playerPos.getY() + player->texH / 2 <= enemyPos.getY()) {
				LOG("Player is on top of the enemy. Enemy will be destroyed.");
				this->isDead = true;
				player->Bounce();   
			}
		}
		break;
	}
	case ColliderType::BULLET:
		LOG("Collided with Bullet");
		Engine::GetInstance().entityManager.get()->DestroyEntity(this);
		break;

	default:
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

