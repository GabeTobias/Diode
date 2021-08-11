#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "nlohmann.hpp"
#include "Props.h"

#include "World.h"
#include "Renderer.h"

#include "Bullet.h"
#include "Missile.h"
#include "Movable.h"

class Shield : public World::Component
{
public:
	float size = 3;
	float regen = 1;
	float damage = 0;
	float maxDamage = 3;

	Time::Timer regenTimer;

	inline Shield() { className = "Shield"; }

	inline void Editor()
	{
		Editor::Float("Regen", &regen);
		Editor::Float("Size", &size);
		Editor::Float("Hits", &damage);
		Editor::Float("Strength", &maxDamage);
	}

	inline void Update()
	{
		//Bullets
		std::vector<World::Entity*> bullets = World::GetEntitiesOfClass("Bullet");
		for (World::Entity* ent : bullets) 
		{
			Movable* rig = (Movable*)ent->GetComponent("Movable");

			if (rig) 
			{
				float distCurrent = glm::distance(ent->position, owner->position);
				float distNext = glm::distance(ent->position + rig->velocity, owner->position);
			
				if (distCurrent > (size / 2.f) && distNext < (size / 2.f)) 
				{
					World::DeleteEntity(ent);
					damage += ((Bullet*)ent)->damage;

				}
			}
		}

		//Missiles
		std::vector<World::Entity*> missiles = World::GetEntitiesOfClass("Missile");
		for (World::Entity* ent : missiles)
		{
			Movable* rig = (Movable*)ent->GetComponent("Movable");

			if (rig)
			{
				float distCurrent = glm::distance(ent->position, owner->position);
				float distNext = glm::distance(ent->position + rig->velocity, owner->position);

				if (distCurrent > (size / 2.f) && distNext < (size / 2.f))
				{
					World::DeleteEntity(ent);
					damage += ((Bullet*)ent)->damage;

				}
			}
		}

		//Damage Regen
		if (regenTimer.getInterval(regen) && damage > 0) 
			damage -= 1;

		//Damage Kill
		if (damage >= maxDamage)
			World::DeleteComponent(this);
	}

	inline void Draw() 
	{
		float percentDmg = (maxDamage - damage) / maxDamage;
		Render::DrawCircle(glm::vec3(owner->position,11), glm::vec2(size), glm::vec4(1,1,1, percentDmg * 0.75), true, camera->size / 1200);
	}

	inline nlohmann::json Save()
	{
		nlohmann::json data;
		data["size"] = size;
		data["regen"] = regen;
		data["maxDamage"] = maxDamage;
		return data;
	}

	inline void Load(nlohmann::json data)
	{
		if (data.contains("size")) size = data["size"].get<float>();
		if (data.contains("regen")) regen = data["regen"].get<float>();
		if (data.contains("maxDamage")) maxDamage = data["maxDamage"].get<float>();
	}

	inline static World::Component* Construct(World::Entity* ent)
	{
		return new Shield();
	}
};