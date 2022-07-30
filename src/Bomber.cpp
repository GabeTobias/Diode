#include "Bomber.h"

#include "Renderer.h"
#include "Game.h"
#include "Bomb.h"

#include "Movable.h"
#include "Collidable.h"
#include "Destructable.h"

using namespace glm;

Bomber::Bomber()
{
	className = "Bomber";

	Tag("Enemy");
	Tag("NotBombable");

	collider = new Collidable(vec2(2));
	AttachComponent(collider);

	rigidbody = new Movable(collider);
	AttachComponent(rigidbody);

	destruct = new Destructable();
	AttachComponent(destruct);
}

void Bomber::Update()
{
	if (distance(position, Game::player->position) > viewDistance)
		return;

	if (distance(position, Game::player->position) > fireDistance)
	{
		static vec2 direction;
		direction = lerp(direction, Game::player->position - position, 0.2f);

		rigidbody->velocity = normalize(lerp(rigidbody->velocity, direction, accuracy)) * speed;
		return;
	}
	else 
		rigidbody->velocity = lerp(rigidbody->velocity, vec2(0), 0.2f);

	if (restTimer.getInterval(restRate))
		firing = !firing;

	if (firing && fireTimer.getInterval(fireRate))
	{
		Bomb* bomb;

		if(!heatSeeking)
			bomb = new Bomb(position, Game::player->position, 1, 5, 2);
		else
			bomb = new Bomb(position, Game::player, 1, 5, 2);

		World::AddEntity(bomb);
	}
}

void Bomber::Draw()
{
	static vec2 direction;
	direction = lerp(direction, Game::player->position - position, 0.2f);
	float ang = atan(direction.y / direction.x);

	Render::DrawQuad(vec3(position, 13), ang, vec2(2), vec4(1,1,0,1));

	if (GameState::Mode == EditMode)
		Render::DrawCircle(vec3(position, 2), vec2(viewDistance * 2), vec4(1, 1, 1, 0.15), true, 0.01f);
}

void Bomber::Editor()
{
	Editor::Float("Accuracy", &accuracy);
	Editor::Float("View Distance", &viewDistance);
	Editor::Float("Fire Distance", &fireDistance);
	Editor::Float("Fire Rate", &fireRate);
	Editor::Float("speed", &speed);
	Editor::Float("accuracy", &accuracy);

	Editor::CheckBox("heatSeeking", &heatSeeking);
}

nlohmann::json Bomber::Save()
{
	nlohmann::json data;

	data["fireRate"] = fireRate;
	data["restRate"] = restRate;
	data["viewDistance"] = viewDistance;
	data["fireDistance"] = fireDistance;
	data["speed"] = speed;
	data["accuracy"] = accuracy;
	data["heatSeeking"] = heatSeeking;

	return data;
}

void Bomber::Load(nlohmann::json data)
{
	if (data.contains("fireRate")) fireRate = data["fireRate"].get<float>();
	if (data.contains("restRate")) restRate = data["restRate"].get<float>();
	if (data.contains("viewDistance")) viewDistance= data["viewDistance"].get<float>();
	if (data.contains("fireDistance")) fireDistance = data["fireDistance"].get<float>();
	if (data.contains("speed")) speed = data["speed"].get<float>();
	if (data.contains("accuracy")) accuracy = data["accuracy"].get<float>();
	if (data.contains("heatSeeking")) heatSeeking = data["heatSeeking"].get<bool>();
}
