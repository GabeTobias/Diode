#pragma once

#include <glm/glm.hpp>

#include "Renderer.h"
#include "State.h"

#include "World.h"

#include "imgui.h"
#include "Props.h"

class Collidable : public World::Component 
{
public:
	//Collision Box
	glm::vec2 position = glm::vec2(0);
	glm::vec2 bounds;
	int layer = 0;

	//Bind Bounds & Set class name
	inline Collidable(glm::vec2 size) : bounds(size) 
	{
		className = "Collidable";
	}
	
	//Collider Debug View
	inline void Draw() 
	{
		if (GameState::ShowOverlaps) 
		{
			Render::DrawQuad(glm::vec3(owner->position + position, 5), 0, bounds, glm::vec4(1), true, camera->size / (glm::max(bounds.x, bounds.y) * 300));
		}
	}

	inline void Editor() 
	{
		Editor::Float2("Bounds",&bounds[0]);
		Render::DrawQuad(glm::vec3(owner->position + position, 1), 0, bounds, glm::vec4(1), true, camera->size / (glm::max(bounds.x, bounds.y)* 300));
	}

	inline static World::Component* Construct(World::Entity* ent) 
	{
		return new Collidable(glm::vec2(1));
	}
};