#include "Bullet.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"

Bullet::Bullet() : Entity(EntityType::ITEM)
{
	name = "bullet";
}

Bullet::~Bullet() {}

bool Bullet::Awake() {
	
	return true;
}

bool Bullet::Start() {
	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	//position.setX(parameters.attribute("x").as_int());
	//position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();
	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	currentAnimation = &idle;
	
	// L08 TODO 4: Add a physics to an item - initialize the physics body
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->body->IsBullet();
	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::BULLET;
	pbody->body->SetGravityScale(0);
	if (leftBullet) pbody->body->SetLinearVelocity(b2Vec2(PIXEL_TO_METERS(-2000.0f), 0)); else pbody->body->SetLinearVelocity(b2Vec2(PIXEL_TO_METERS(2000.0f), 0));

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);
	timerToDie = new Timer();
	return true;
}

void Bullet::SetVelocity(Direction direction)
{
	switch (direction)
	{
	case Direction::LEFT:
		pbody->body->SetLinearVelocity(b2Vec2(PIXEL_TO_METERS(-2000.0f), 0));
		break;  
	case Direction::RIGHT:
		pbody->body->SetLinearVelocity(b2Vec2(PIXEL_TO_METERS(2000.0f), 0));
		break;
	}
}

bool Bullet::Update(float dt) {
	// Decrement timer

	// Update position
	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	currentAnimation->Update();



	return true;
}


bool Bullet::CleanUp()
{
	Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
	return true;
}

void Bullet::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{

	
	case ColliderType::PLATFORM:
		//Engine::GetInstance().entityManager.get()->DestroyEntity(this);

		break;

	}
}
//void Bullet::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
//{
//	switch (physB->ctype)
//	{
//	case ColliderType::PLAYER:
//		LOG("Collision player");
//		break;
//	case ColliderType::ENEMY:
//		//Engine::GetInstance().entityManager.get()->DestroyEntity(this);
//		break;
//	}
//}
