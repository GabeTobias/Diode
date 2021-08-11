#include "DashTarget.h"

#include "Renderer.h"
#include "Game.h"
#include "Props.h"

using namespace glm;

DashTarget::DashTarget()
{
	className = "DashTarget";
}

void DashTarget::Awake(){}

void DashTarget::Update()
{
	directionDot = dot(
		normalize(position - Game::player->position),
		normalize(Game::player->movement->velocity)
	);

	float directionZone = 0.2;
	float playerDist = distance(position, Game::player->position);
	bool inDirection = (directionDot > (1.0-directionZone));

	//Determine if player is dashing into me
	if (Game::player->dashing && playerDist < snapRadius && !hasPlayer && entering && inDirection)
	{
		hasPlayer = true;
		Game::player->dashing = false;
		entering = false;
	}


	if (hasPlayer)
	{
		if (Game::player->dashing)
		{
			hasPlayer = false;
		
			std::vector<World::Entity*> otherTargets = World::GetEntitiesOfClass("DashTarget");

			float nearestDistance = 0;
			DashTarget* target = nullptr;

			//Iterate over all instances of DashTarget
			for (World::Entity* entt : otherTargets) 
			{
				DashTarget* current = (DashTarget*)entt;
				float dist = distance(Game::player->position, entt->position);

				//Skip myself
				if (entt->uid == uid) continue;

				//Determine if dash is targeting another DashTarget
				if (current->directionDot > (1.0-directionZone) && (target == nullptr || dist < nearestDistance))
				{
					target = current;
					nearestDistance = dist;
				}
			}

			//Pass player to new DashTarget
			if (target != nullptr)
			{
				target->hasPlayer = true;
				target->entering = true;
				Game::player->dashing = false;
			}
		}

		//Move player to target position
		Game::player->position = lerp(Game::player->position, position, 0.2f);
	}

	if (playerDist > snapRadius) entering = true;
}

void DashTarget::Draw()
{
	bool inDirection = (directionDot > 0.8);
	vec4 color = vec4(0.5, 0.1, 1, inDirection ? 1 : 0.5 );

	Render::DrawQuad(vec3(position, 12), 0, vec2(0.7), color);

	Render::DrawQuad(vec3(position + vec2( 0.75, 1), 12), 0, vec2(0.58, 0.15), color);
	Render::DrawQuad(vec3(position + vec2(-0.75, 1), 12), 0, vec2(0.58, 0.15), color);

	Render::DrawQuad(vec3(position + vec2( 1, 0.75), 12), 0, vec2(0.15, 0.58), color);
	Render::DrawQuad(vec3(position + vec2(-1, 0.75), 12), 0, vec2(0.15, 0.58), color);

	Render::DrawQuad(vec3(position + vec2( 0.75,-1), 12), 0, vec2(0.58, 0.15), color);
	Render::DrawQuad(vec3(position + vec2(-0.75,-1), 12), 0, vec2(0.58, 0.15), color);

	Render::DrawQuad(vec3(position + vec2( 1, -0.75), 12), 0, vec2(0.15, 0.58), color);
	Render::DrawQuad(vec3(position + vec2(-1, -0.75), 12), 0, vec2(0.15, 0.58), color);
}

void DashTarget::Editor()
{
	Editor::Float("Snap Radius", &snapRadius);
}

nlohmann::json DashTarget::Save()
{
	nlohmann::json data;

	data["snapRadius"] = snapRadius;

	return data;
}

void DashTarget::Load(nlohmann::json data)
{
	if (data.contains("snapRadius")) snapRadius = data["snapRadius"].get<float>();
}
