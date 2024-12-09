 #include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
	direction = Direction::RIGHT;
	isWalking = false;
	godMode = false;
}

Player::~Player() {

}

bool Player::Awake() {

	return true;
}

bool Player::Start() {

	//L03: TODO 2: Initialize Player parameters
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//Load animations
	idleR.LoadAnimations(parameters.child("animations").child("idleR"));
	currentAnimation = &idleR; 
	walk.LoadAnimations(parameters.child("animations").child("walk"));
	right.LoadAnimations(parameters.child("animations").child("right"));
	idleL.LoadAnimations(parameters.child("animations").child("idleL"));
	dead.LoadAnimations(parameters.child("animations").child("dead"));

	// L08 TODO 5: Add physics to the player - initialize physics body
	pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX(), (int)position.getY(), texW - texW/1.5 , texH - texH/4.5, bodyType::DYNAMIC);

	// L08 TODO 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;
	pbody->body->SetFixedRotation(true);
	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;
	pbody->body->IsBullet();
	SetMass(20);
	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	//initialize audio effect
	pickCoinFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");
	jumpFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/jump.wav");
	stepFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/step.ogg");
	shootFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/shoot.ogg");
	playerdieFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/playerdie.ogg");
	godMode = false;

	return true;
}

bool Player::Update(float dt)
{
	
	if (isDead) {
		
		if (!currentAnimation->HasFinished()) {
			currentAnimation->Update(dt);
		}

		Engine::GetInstance().render.get()->DrawTexture(texture,
			(int)position.getX(),
			(int)position.getY(),
			&currentAnimation->GetCurrentFrame());

		return true; 
	}

	if (walksoundTimer.ReadMSec() > 200) {
		isWalking = false;
		walksoundTimer.ResetTimer();
	}


	currentAnimation = (direction == Direction::LEFT) ? &idleL : &idleR;

	b2Vec2 velocity = b2Vec2(0, pbody->body->GetLinearVelocity().y);

	
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		velocity.x = -0.2f * 16.0f;
		direction = Direction::LEFT;
		currentAnimation = &right; 

		if (!isWalking && !isJumping) {
			Engine::GetInstance().audio.get()->PlayFx(stepFxId);
			walksoundTimer.Start();
			isWalking = true;
		}
	}

	
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		velocity.x = 0.2f * 16.0f;
		direction = Direction::RIGHT;
		currentAnimation = &walk; 

		if (!isWalking && !isJumping) {
			Engine::GetInstance().audio.get()->PlayFx(stepFxId);
			walksoundTimer.Start();
			isWalking = true;
		}
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) != KEY_REPEAT &&
		Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) != KEY_REPEAT) {
		isWalking = false;
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		if (!isJumping) {
			pbody->body->SetLinearVelocity(b2Vec2(velocity.x, 0));
			pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);
			Engine::GetInstance().audio.get()->PlayFx(jumpFxId);
			isJumping = true;
		}
		else if (canDJ) {
			pbody->body->SetLinearVelocity(b2Vec2(velocity.x, 0));
			pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);
			Engine::GetInstance().audio.get()->PlayFx(jumpFxId);
			canDJ = false;
		}
	}

	if (isJumping) {
		velocity.y = pbody->body->GetLinearVelocity().y;
	}

	pbody->body->SetLinearVelocity(velocity);

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2 - texH / 11);

	Engine::GetInstance().render.get()->DrawTexture(texture,
		(int)position.getX(),
		(int)position.getY(),
		&currentAnimation->GetCurrentFrame());
	currentAnimation->Update(dt);

	return true;
}




void Player::SetMass(float newMass) {
	if (pbody && pbody->body) {
		b2MassData massData;
		pbody->body->GetMassData(&massData);
		massData.mass = newMass;
		pbody->body->SetMassData(&massData);
	}
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		isJumping = false;
		canDJ = true;
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		Engine::GetInstance().audio.get()->PlayFx(pickCoinFxId);
		Engine::GetInstance().physics.get()->DeletePhysBody(physB); 
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	case ColliderType::CHECKPOINT:
		LOG("CheckPoint collision ON");
		setCheckPoint = true;
		break;
	case ColliderType::KILLER:
		LOG("Player Killed");
		if (!godMode && !isDead){
			Engine::GetInstance().audio.get()->PlayFx(playerdieFxId);
		currentAnimation = &dead;
		currentAnimation->Reset();
		isDead = true;
		}
		break;
	case ColliderType::ENEMY:
		if (!godMode && !isDead) {
			Engine::GetInstance().audio.get()->PlayFx(playerdieFxId);
			currentAnimation = &dead;
			currentAnimation->Reset();
			isDead = true;
		}
		break;
	case ColliderType::ENEMYBFS:
		if (!godMode && !isDead) {
			Engine::GetInstance().audio.get()->PlayFx(playerdieFxId);
			currentAnimation = &dead;
			currentAnimation->Reset();
			isDead = true;
		}
		break;

	default:
		break;
	}
}

void Player::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("End Collision PLATFORM");
		break;
	case ColliderType::ITEM:
		LOG("End Collision ITEM");
		break;
	case ColliderType::UNKNOWN:
		LOG("End Collision UNKNOWN");
		break;
	case ColliderType::CHECKPOINT:
		LOG("CheckPoint collision OFF");
		setCheckPoint = false;
		break;
	default:
		break;
	}
}

void Player::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos,0);
}

Vector2D Player::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

void Player::Bounce()
{
	
	b2Vec2 bounceImpulse = b2Vec2(0.0f, -120.0f); 
	pbody->body->ApplyLinearImpulseToCenter(bounceImpulse, true);
}