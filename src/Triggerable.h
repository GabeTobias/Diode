#pragma once

#include <glm/glm.hpp>

#include "World.h"
#include "State.h"
#include "Physics.h"

#include "nlohmann.hpp"

#include "ImGui.h"
#include "Props.h"
#include "File.h"

class Triggerable : public World::Component
{
public:
	bool triggered = false;

	glm::vec2 position;
	glm::vec2 bounds;

public:
	Triggerable(glm::vec2 pos, glm::vec2 bounds) : position(pos), bounds(bounds) { className = "Triggerable"; }

	inline void Update()
	{
		triggered = (bool)Physics::Collides(owner->position + position - (bounds/2.f), bounds);
	}

	//Debug View
	inline void Draw()
	{
		if (GameState::ShowOverlaps || GameState::Mode == AppState::EditMode)
			Render::DrawQuad(glm::vec3(owner->position + position, 5), 0, bounds, glm::vec4(0.5f, 1, 0.5f, 1), true, camera->size / (glm::max(bounds.x, bounds.y) * 300));
	}

	inline void Editor()
	{
		Editor::Float2("Postiion", &position[0]);
		Editor::Float2("Bounds", &bounds[0]);
	}

	inline nlohmann::json Save() 
	{
		nlohmann::json data;

		data["offset"] = File::JsonVec2(position);
		data["bounds"] = File::JsonVec2(bounds);

		return data;
	}

	inline void Load(nlohmann::json data) 
	{
		if (data.contains("offset")) position = File::Vec2Json(data["offset"]);
		if (data.contains("bounds")) bounds = File::Vec2Json(data["bounds"]);
	};


	inline static World::Component* Construct(World::Entity* ent)
	{
		return new Triggerable(glm::vec2(0), glm::vec2(1));
	}
};