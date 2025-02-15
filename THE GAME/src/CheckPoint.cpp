#include "CheckPoint.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"

CheckPoint::CheckPoint() : Entity(EntityType::ITEM)
{
	name = "checkpoint";
}

CheckPoint::~CheckPoint() {}

bool CheckPoint::Awake() {
	return true;
}

bool CheckPoint::Start() {

	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	currentAnimation = &idle;
	
	// L08 TODO 4: Add a physics to an item - initialize the physics body
	pbody = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texW / 2 + 128, texH / 2 + 128, bodyType::STATIC);

	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::CHECKPOINT;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	return true;
}

bool CheckPoint::Update(float dt)
{
	// L08 TODO 4: Add a physics to an item - update the position of the object from the physics.  

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2); 

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	currentAnimation->Update(dt);

	return true;
}

bool CheckPoint::CleanUp()
{
	if (pbody != nullptr) {
		Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
	}

	return true;
}