#include "Dropper.h"

#include "Game.h"

#include "File.h"
#include "Props.h"

#include "Movable.h"
#include "Collidable.h"
#include "Destructable.h"

using namespace glm;

Dropper::Dropper(vec2 size)
{
	className = "Dropper";

	this->size = size;

	collider = new Collidable(vec2(size));
	AttachComponent(collider);

	destruct = new Destructable();
	AttachComponent(destruct);
}

void Dropper::Awake() 
{
	collider->bounds = size;
}

void Dropper::Update()
{
	static int colliderDelay = 0;

	if (inAir) 
	{
		collider->active = false;

		//Chase the Player
		position = lerp(position, Game::player->position, moveSpeed);

		if (dropTimer.getInterval(dropTime) || distance(Game::player->position, position) < 2)
		{
			inAir = false;
			sitTimer.reset();

			//Player Hit
			std::vector<World::Component*> movables = World::GetComponentsOfClass("Movable");
			for (World::Component* comp : movables) 
			{
				if (distance(position, comp->owner->position) < size.x)
				{
					Movable* move = (Movable*)comp;
					vec2 dir = Game::player->position - position;

					move->passive = normalize(dir) * 0.75f;
					colliderDelay = 15;

					//Player Damage
					Destructable* pHealth = (Destructable*)comp->owner->GetComponent("Destructable");
					if (pHealth) pHealth->TakeDamage(damage);
				}
			}
		}
	}
	else 
	{
		//Delays collider activity to allow objects undernieth to move
		if (colliderDelay == 0)		collider->active = true;
		else						colliderDelay--;

		if (sitTimer.getInterval(sitTime))
		{
			inAir = true;
			dropTimer.reset();
		}
	}
}

void Dropper::Draw()
{
	static vec2 visibleSize;
	vec2 targetSize = (inAir) ? vec2(0) : size;
	visibleSize = lerp(visibleSize, targetSize, 0.1f);

	Render::DrawQuad(vec3(position,10), 0, visibleSize, vec4(1,0.5,0,0.6));		//TODO: Z should change by state

	//Draw Hover Indicator
	Render::DrawCircle(vec3(position,11), (size-visibleSize)+vec2(3), vec4(0,0,0,0.8), true, 0.03);	
	Render::DrawCircle(vec3(position,11), (size-visibleSize)+vec2(3), vec4(0,0,0,0.25));	
}

void Dropper::Editor()
{
	Editor::Float2("Size", &size.x);
	if (ImGui::IsItemEdited) collider->bounds = size;

	Editor::Float("MoveSpeed", &moveSpeed);
	Editor::Float("Drop Time", &dropTime);
	Editor::Float("Sit Time", &sitTime);
	Editor::Float("HitDamage", &damage);
	
	Editor::CheckBox("In Air", &inAir);
}

nlohmann::json Dropper::Save()
{
	nlohmann::json data;

	data["size"] = File::JsonVec2(size);

	data["moveSpeed"] = moveSpeed;
	data["dropTime"] = dropTime;
	data["sitTime"] = sitTime;
	data["damage"] = damage;

	return data;
}

void Dropper::Load(nlohmann::json data)
{
	if (data.contains("size")) size = File::Vec2Json(data["size"]);

	if (data.contains("moveSpeed")) moveSpeed = data["moveSpeed"].get<float>();
	if (data.contains("dropTime")) dropTime = data["dropTime"].get<float>();
	if (data.contains("sitTime")) sitTime = data["sitTime"].get<float>();
	if (data.contains("damage")) damage = data["damage"].get<float>();
}
