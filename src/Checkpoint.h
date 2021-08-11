#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "nlohmann.hpp"
#include "Props.h"

#include "Game.h"
#include "World.h"
#include "Player.h"

#include "Renderer.h"

class Checkpoint : public World::Entity
{
public:
	int index = 0;
	glm::vec4 color = glm::vec4(1);

public:
	Checkpoint() { className = "Checkpoint"; editorBounds = glm::vec2(2); }

	inline void Update() 
	{
		//Activate Checkpoint players in range
		if (glm::distance(Game::player->position, position) < 5)
		{
			Game::player->currentCheckpoint = index;
		}
	}

	inline void Draw() 
	{
		color = glm::lerp(color, (Game::player->currentCheckpoint == index) ? glm::vec4(1,1,1,0.5): glm::vec4(1,1,1,0.2), 0.2f);

		Render::DrawCircle(glm::vec3(position,11), glm::vec2(2), color, true, 0.05f);
		
		if(Game::player->currentCheckpoint == index)
			Render::DrawCircle(glm::vec3(position,11), glm::vec2(0.75), color);
	}


	inline nlohmann::json Save()
	{
		nlohmann::json data;
		data["index"] = index;
		return data;
	}

	inline void Load(nlohmann::json data)
	{
		if (data.contains("index")) index = data["index"].get<int>();
	}


	inline void Editor()
	{
		Editor::Int("Index", &index);
	}

	static World::Entity* Construct() { return new Checkpoint(); }
};