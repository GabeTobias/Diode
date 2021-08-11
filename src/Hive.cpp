#include "Hive.h"
#include "State.h"

#include "File.h"
#include "Game.h"
#include "Editor.h"
#include "Props.h"

#include "Drone.h"

#include "Collidable.h"
#include "Destructable.h"
#include "Shocked.h"

using namespace glm;

Hive::Hive(int waveCount, int spawnCount) : waveCount(waveCount), spawnCount(spawnCount)
{
	className = "Hive";
	Tag("Enemy");

	collider = new Collidable(vec2(4));
	AttachComponent(collider);

	destruct = new Destructable();
	AttachComponent(destruct);

	destruct->maxDamage = 15;

	spawnPoints = new glm::vec2[spawnerCount];
	for (int i = 0; i < spawnerCount; i++) spawnPoints[i] = vec2(0);
}

void Hive::Awake()
{
	initWaveCount = waveCount;
}

void Hive::Update()
{
	//TODO: Replace this with some sort of in room function
	bool inRange = distance(position, Game::player->position) < viewDistance;

	if (inRange) 
	{
		//Spawn Wave
		if (spawnTimer.getInterval(spawnRate) && waveCount > 0) 
		{
			//TODO: Maybe create spawn direction handle
			for (int i = 0; i < spawnCount; i++)
			{
				//Spawn Enemy
				Drone* drone = new Drone(Game::player, position + spawnPoints[(i+offset)%spawnerCount], 1);
				World::AddEntity(drone);

				//Add to Children
				children.push_back(drone);
			}

			offset += spawnCount;
			
			waveCount--;
		}
	}
}

void Hive::Draw()
{
	float opacity = (active) ? 1.0f : 0.5f;

	Render::DrawQuad(vec3(position, 10), 0, vec2(2.8), vec4(1,1,1,0.1));
	Render::DrawQuad(vec3(position, 10), 0, vec2(4.2f), vec4(1,1,1,opacity), true, 0.05f);

	for (int i = 0; i < initWaveCount; i++) 
	{
		Render::DrawQuad(vec3(position + spawnPoints[i], 10), 0, vec2(0.4), vec4(0.5, 0, 1, 0.5));
	}

	if (initWaveCount == 4)
	{
		if (waveCount >= 4) Render::DrawQuad(vec3(position + vec2(-0.8, -0.8), 9), 0, vec2(1.2),vec4(1, 1, 1, opacity));
		if (waveCount >= 3) Render::DrawQuad(vec3(position + vec2(0.8, -0.8), 9), 0, vec2(1.2), vec4(1, 1, 1, opacity));
		if (waveCount >= 2) Render::DrawQuad(vec3(position + vec2(-0.8, 0.8), 9), 0, vec2(1.2), vec4(1, 1, 1, opacity));
		if (waveCount >= 1) Render::DrawQuad(vec3(position + vec2(0.8, 0.8), 9), 0, vec2(1.2),	vec4(1, 1, 1, opacity));
	}

	if (initWaveCount == 3)
	{
		if (waveCount >= 3) Render::DrawQuad(vec3(position + vec2(0, -0.8), 9), 0, vec2(1.2),	vec4(1, 1, 1, opacity));
		if (waveCount >= 2) Render::DrawQuad(vec3(position + vec2(-0.8, 0.8), 9), 0, vec2(1.2), vec4(1, 1, 1, opacity));
		if (waveCount >= 1) Render::DrawQuad(vec3(position + vec2(0.8, 0.8), 9), 0, vec2(1.2),	vec4(1, 1, 1, opacity));
	}

	if (initWaveCount == 2)
	{
		if (waveCount >= 2) Render::DrawQuad(vec3(position + vec2(-0.8, 0), 9), 0, vec2(1.2,2.6),	vec4(1, 1, 1, opacity));
		if (waveCount >= 1) Render::DrawQuad(vec3(position + vec2(0.8, 0), 9), 0, vec2(1.2,2.6),	vec4(1, 1, 1, opacity));
	}

	if (initWaveCount == 1)
	{
		if (waveCount >= 1) Render::DrawQuad(vec3(position, 9), 0, vec2(2.8), vec4(1,1,1, opacity));
	}

	if (GameState::Mode == EditMode)
		Render::DrawCircle(vec3(position, 2), vec2(viewDistance * 2), vec4(1, 1, 1, 0.15), true, 0.01f);
}

void Hive::Editor()
{
	Editor::Int("Spawn Offset", &offset);
	Editor::Int("Wave Count", &waveCount);
	Editor::Int("Spawn Count", &spawnCount);
	Editor::Float("Spawn Rate", &spawnRate);
	Editor::Float("View Distance", &viewDistance);

	Editor::Int("Spawner Count", &spawnerCount);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		spawnPoints = new glm::vec2[spawnerCount];
		for (int i = 0; i < spawnerCount; i++)
			spawnPoints[i] = vec2(1);
	}

	for (int i = 0; i < spawnerCount; i++)
	{
		spawnPoints[i] = Editor::Manipulator(uid + i + 1, spawnPoints[i] + position) - position;
	}

	if (ImGui::TreeNode("Spawn Points")) 
	{
		for (int i = 0; i < spawnerCount; i++)
		{
			Editor::Float2(("Spawn " + std::to_string(i)).c_str(), &spawnPoints[i][0]);
		}

		ImGui::TreePop();
	}
}

nlohmann::json Hive::Save()
{
	nlohmann::json data;

	data["waveCount"] = waveCount;
	data["spawnCount"] = spawnCount;
	data["spawnerCount"] = spawnerCount;
	data["spawnRate"] = spawnRate;
	data["viewDistance"] = viewDistance;

	for (int i = 0; i < spawnerCount; i++)
	{
		data[("spawn" + std::to_string(i)).c_str()] = File::JsonVec2(spawnPoints[i]);
	}

	return data;
}

void Hive::Load(nlohmann::json data)
{
	if (data.contains("waveCount")) waveCount = data["waveCount"].get<int>();
	if (data.contains("spawnCount")) spawnCount = data["spawnCount"].get<int>();
	if (data.contains("spawnerCount")) spawnerCount = data["spawnerCount"].get<int>();
	if (data.contains("spawnRate")) spawnRate = data["spawnRate"].get<float>();
	if (data.contains("viewDistance")) viewDistance = data["viewDistance"].get<float>();
	
	spawnPoints = new glm::vec2[spawnerCount];
	for (int i = 0; i < spawnerCount; i++)
	{
		if (data.contains(("spawn" + std::to_string(i)).c_str())) spawnPoints[i] = File::Vec2Json(data[("spawn" + std::to_string(i)).c_str()]);
	}
}

void Hive::Destroy()
{
	for (int i = 0; i < children.size(); i++) 
	{
		if (children[i]) 
		{
			//Delete shocked component on disable
			Shocked* shock = (Shocked*)children[i]->GetComponent("Shocked");
			if (shock) World::DeleteComponent(shock);

			children[i]->active = false;
			children[i]->rigidbody->drifting = true;
		}
	}
}
