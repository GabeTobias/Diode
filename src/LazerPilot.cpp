#include "LazerPilot.h"

#include "Renderer.h"
#include "Game.h"

using namespace glm;

LazerPilot::LazerPilot()
{
	className = "LazerPilot";
}

void LazerPilot::Awake()
{
}

void LazerPilot::Update()
{
	// For now I'm just allowing the player to dash into the lazer but in the future 
	// it would be better to have a direction interact button for clearity
	
	float playerDist = distance(position, Game::player->position);
	static bool entering = true;

	//Determine if player is dashing into me
	if (Game::player->dashing && playerDist < snapRadius && !hasPlayer && entering)
	{
		hasPlayer = true;
		Game::player->megaLazer = true;
		Game::player->dashing = false;
		((Movable*)Game::player->GetComponent("Movable"))->velocity = vec2(0);
		entering = false;
	}

	//Move player to target position
	if (hasPlayer)
	{
		Game::player->position = lerp(Game::player->position, position, 0.2f);

		if (Game::player->dashing) 
		{
			Game::player->megaLazer = false;
			hasPlayer = false;
		}
	}

	// Prevent player from snapping back onto the lazer when trying to get off
	if (!hasPlayer && !Game::player->dashing) 
	{
		entering = true;
	}
}

void LazerPilot::Draw()
{
	vec4 color = vec4(0.5, 0.1, 1, hasPlayer ? 1 : 0.5);

	Render::DrawQuad(vec3(position, 12), 3.14/4.0f, vec2(1.0), color, true, 0.1f);
	Render::DrawCircle(vec3(position, 12), vec2(2.0f), color, true, 0.05f);

	/*
		Particle::KirbyDot(position, {minSize, maxSize}, color, duration, count(default = 1));
		Particle::ActionLine(start, stop, color, duration);
		Particle::Matrix(position, size, color[], duration, count(default = 6));
	*/
}

void LazerPilot::Editor()
{
}

nlohmann::json LazerPilot::Save()
{
	return nlohmann::json();
}

void LazerPilot::Load(nlohmann::json data){}
