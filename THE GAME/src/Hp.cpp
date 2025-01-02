#include "Hp.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "Player.h"
#include "EntityManager.h"

Hp::Hp() : Entity(EntityType::HP) 
{
    name = "hp";
}

Hp::~Hp() {}

bool Hp::Awake() {
    return true;
}

bool Hp::Start() {

    texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
    position.setX(parameters.attribute("x").as_int());
    position.setY(parameters.attribute("y").as_int());
    texW = parameters.attribute("w").as_int();
    texH = parameters.attribute("h").as_int();

    idle.LoadAnimations(parameters.child("animations").child("idle"));
    currentAnimation = &idle;

    pbody = Engine::GetInstance().physics.get()->CreateCircle(
        (int)position.getX() + texH / 2,
        (int)position.getY() + texH / 2,
        texH / 2,
        bodyType::STATIC
    );

    pbody->ctype = ColliderType::HP;
    pbody->listener = this;

    if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

    return true;
}

bool Hp::Update(float dt)
{
    b2Transform pbodyPos = pbody->body->GetTransform();
    position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
    position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

    Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
    currentAnimation->Update(dt);

    return true;
}

bool Hp::CleanUp()
{
    LOG("Cleaning up Hp entity");

    
    if (texture) {
        Engine::GetInstance().textures.get()->UnLoad(texture);
        texture = nullptr; 
    }

   
    if (pbody) {
        Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
        pbody = nullptr;
    }

    return true;
}

void Hp::OnCollision(PhysBody* physA, PhysBody* physB) {
     if (physB->ctype == ColliderType::PLAYER) {
        Player* player = (Player*)physB->listener; 

        if (player->lives < 3) {
            player->lives++; 
          
			
            Engine::GetInstance().physics.get()->DeletePhysBody(pbody); 
			Engine::GetInstance().entityManager.get()->DestroyEntity(this);

            LOG("Player's HP restored!");
        }
        else {
            LOG("Player already has max HP!");
        }
    }
}

//crea una funcion para que aparezca el hp en el mapa (ui) y que se actualice dependiendo de las vidas del player

void DrawHp() {
	int x = 10;
	int y = 10;
	int w = 50;
	int h = 50;
	for (int i = 0; i < Engine::GetInstance().scene.get()->player->lives; i++) {
		Engine::GetInstance().render.get()->DrawTexture(Engine::GetInstance().textures.get()->Load("Assets/Textures/goldCoin.png"), x, y, nullptr, 0.0f);
		x += w;
	}
}




