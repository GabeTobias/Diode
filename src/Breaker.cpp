#include "Breaker.h"

#include "Renderer.h"
#include "State.h"

#include "Pannable.h"
#include "Destructable.h"
#include "Collidable.h"
#include "Movable.h"

using namespace glm;

Breaker::Breaker()
{
	className = "Breaker";
	Tag("Obstacle");
	Tag("Roomless");

	size = vec2(5);

	pan = new Pannable();
	AttachComponent(pan);

	collider = new Collidable(size);
	AttachComponent(collider);

	rigidbody = new Movable(collider);
	AttachComponent(rigidbody);
}

void Breaker::Update()
{
	//Kill player if colliding
	if (rigidbody->colliding && rigidbody->collided->className == "Player") 
	{
		Destructable* destruct = (Destructable*)rigidbody->collided->GetComponent("Destructable");
		if (destruct != nullptr) 
			destruct->TakeDamage(100);
	}
}

void Breaker::Draw()
{
	Render::DrawQuad(vec3(position,9), 0, size, vec4(1));

	//Draw pannable targets if in editor
	if (GameState::Mode == AppState::EditMode) 
	{
		for (int i = 0; i < pan->points.size(); i++)
		{
			vec2 pos = position + pan->points[i];
			Render::DrawQuad(vec3(pos,11), 0, size, vec4(1,1,1,0.15), true, 0.025);
		}
	}
}

void Breaker::Editor(){}

nlohmann::json Breaker::Save()
{
	return nlohmann::json();
}

void Breaker::Load(nlohmann::json data){}
