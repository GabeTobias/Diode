#include "Pickup.h"

#include "Physics.h"
#include "Renderer.h"
#include "Editor.h"

#include "Game.h"

#include "Destructable.h"

using namespace glm;

Health::Health()
{
	className = "Health";
	Tag("Roomless");

	amount = 1;
}

void Health::Draw()
{
	Render::DrawCircle(vec3(position, 11), vec2(1), vec4(1, 0.3, 0.3, 1));
}

void Health::Update() 
{
	//Pickup if in range
	if (distance(Game::player->position,position) < 2) 
	{
		Destructable* destruct = (Destructable*)Game::player->GetComponent("Destructable");
		destruct->damage -= amount;
	
		World::DeleteEntity(this);
	}
}

nlohmann::json Health::Save()
{
	nlohmann::json data;
	data["amount"] = amount;
	return data;
}

void Health::Load(nlohmann::json data)
{
	if (data.contains("amount")) amount = data["amount"].get<int>();
}

void Health::Editor()
{
	Editor::Int("Amount", &amount);
}
