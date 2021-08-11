#pragma once

#include <glm/glm.hpp>

#include "nlohmann.hpp"
#include "imgui.h"
#include "Props.h"

#include "World.h"

class Destructable : public World::Component
{
public:
	float damage = 0;
	float maxDamage = 3;
	
	inline Destructable() { className = "Destructable"; }

	inline void Editor()
	{
		Editor::Float("Damage", &damage);
		Editor::Float("Max Damage", &maxDamage);
	}

	inline void Update()
	{		
		if (damage >= maxDamage)
			World::DeleteEntity(owner);
	}

	inline void TakeDamage(float amnt) 
	{ 
		damage += amnt; 
	}

	inline nlohmann::json Save() 
	{
		nlohmann::json data;
		data["maxDamage"] = maxDamage;
		return data;
	}

	inline void Load(nlohmann::json data) 
	{
		if(data.contains("maxDamage"))maxDamage = data["maxDamage"].get<float>();
	}

	inline static World::Component* Construct(World::Entity* ent)
	{
		return new Destructable();
	}
};