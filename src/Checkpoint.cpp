#include "Checkpoint.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "nlohmann.hpp"
#include "Props.h"

#include "Game.h"
#include "World.h"
#include "Player.h"

#include "Renderer.h"

#include "Random.h"

Checkpoint::Checkpoint() 
{ 
	className = "Checkpoint"; 
	editorBounds = glm::vec2(2); 

	pointID = RANDOM::UID();
}

void Checkpoint::Update()
{
	//Activate Checkpoint players in range
	if (glm::distance(Game::player->position, position) < 5)
	{
		Game::player->currentCheckpoint = pointID;
	}
}

void Checkpoint::Draw()
{
	color = glm::lerp(color, (Game::player->currentCheckpoint == pointID) ? glm::vec4(1, 1, 1, 0.5) : glm::vec4(1, 1, 1, 0.2), 0.2f);

	Render::DrawCircle(glm::vec3(position, 11), glm::vec2(2), color, true, 0.05f);

	if (Game::player->currentCheckpoint == pointID)
		Render::DrawCircle(glm::vec3(position, 11), glm::vec2(0.75), color);
}

nlohmann::json Checkpoint::Save()
{
	nlohmann::json data;
	data["index"] = pointID;
	return data;
}

void Checkpoint::Load(nlohmann::json data)
{
	if (data.contains("index")) pointID = data["index"].get<int>();
}

void Checkpoint::Editor()
{
	Editor::Int("Index", &pointID);
}