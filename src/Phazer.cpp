#include "Phazer.h"

#include "Hearable.h"

#include "Physics.h"
#include "Renderer.h"
#include "Editor.h"
#include "Game.h"

using namespace glm;

Phazer::Phazer(glm::vec2 pos)
{
	className = "Phazer";
	Tag("Enemy");

	position = pos;

	collider = new Collidable(vec2(1.5));
	AttachComponent(collider);

	rigidbody = new Movable(collider);
	AttachComponent(rigidbody);

	destruct = new Destructable();
	AttachComponent(destruct);

	aSource = new Hearable();
	AttachComponent(aSource);

	destruct->maxDamage = 2;
	shootDirection = vec2(0,1);
	direction = vec2(0,1);
	hitPosition = vec2(0,100);
}

void Phazer::Update()
{
	//Check Range
	inRange = (distance(position, Game::player->position) < 16);

	//Look in Direction
	shootDirection = lerp(shootDirection, direction, turnSpeed);

	//Raycast to Hit
	{
		//Raycast in direction
		Physics::CollisionInfo visualHit = Physics::Raycast(position + (shootDirection * 2.f), position + (shootDirection * 250.f), 500.f, collider->uid);
		if (visualHit)
		{
			hitPosition = visualHit.position;

			//Do damage
			World::Component* destruct = visualHit.entt->GetComponent("Destructable");
			if (destruct && shooting)
			{
				//TODO: Make this variable
				//Try to find Destructable Component
				((Destructable*)destruct)->TakeDamage(0.02f);
			}
		}
	}

	//Ranged Behaviour
	if(inRange)
	{
		//Update Direction
		direction = normalize(Game::player->position - position);

		//Flip Shooting State
		if (cooldownTimer.getInterval((shooting) ? fireTime:coolDownTime)) 
		{
			shooting = !shooting;
		}
	}
}

void Phazer::Draw()
{
	//Calculate Lazer quad props
	float opacity = (active) ? 1.0f : 0.5f;
	float dist = length(hitPosition - position);
	vec2 direct = normalize(hitPosition - position);
	quat rotation = glm::quatLookAt(vec3(direct, 0), vec3(0, 0, 1));

	//Draw Enemy
	Render::DrawQuad(vec3(position,11), eulerAngles(rotation).z, vec2(1), vec4(0,1,0.25, opacity));

	//Draw Range Radius
	if (GameState::Mode == EditMode)
		Render::DrawCircle(vec3(position, 2), vec2(viewDistance * 2), vec4(1, 1, 1, 0.15), true, 0.01f);

	//Draw Lazer
	if (shooting && active)
		Render::DrawQuad(vec3(position + (direct * (dist / 2.f)), 13), eulerAngles(rotation).z, vec2(0.5, dist), vec4(1, 0.5, 0, 1));
}

void Phazer::Editor()
{
	//TODO: Make it so that 2 items with the same name in different 
	//class types don't interfere with each other
	
	Editor::Float("Fire Time", &fireTime);
	Editor::Float("CoolDown Time", &coolDownTime);
	Editor::Float("Lazer Damage", &damage);
	Editor::Float("Turn Rate", &turnSpeed);
	Editor::Float("View Distance", &viewDistance);
	Editor::CheckBox("Ranged", &ranged);
}

nlohmann::json Phazer::Save()
{
	nlohmann::json data;

	data["fireTime"] = fireTime;
	data["coolDownTime"] = coolDownTime;
	data["damage"] = damage;
	data["turnRate"] = turnSpeed;
	data["viewDistance"] = viewDistance;
	data["ranged"] = ranged;

	return data;
}

void Phazer::Load(nlohmann::json data)
{
	if (data.contains("fireTime")) fireTime = data["fireTime"].get<float>();
	if (data.contains("coolDownTime")) coolDownTime = data["coolDownTime"].get<float>();
	if (data.contains("damage")) damage = data["damage"].get<float>();
	if (data.contains("turnRate")) turnSpeed = data["turnRate"].get<float>();
	if (data.contains("viewDistance")) viewDistance = data["viewDistance"].get<float>();
	if (data.contains("ranged")) ranged = data["ranged"].get<bool>();
}
