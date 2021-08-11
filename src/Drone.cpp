#include "Drone.h"

#include "ImGui.h"
#include "Props.h"

#include "Game.h"
#include "State.h"
#include "Physics.h"

#include "Bullet.h"

#include "Destructable.h"
#include "Collidable.h"
#include "Movable.h"

#include "Pathfinding.h"
#include "Room.h"

using namespace glm;

Drone::Drone(World::Entity* target, vec2 pos, int difficulty)
{
	this->className = "Drone";
	this->position = pos;
	this->target = target;

	this->damage = difficulty;
	this->acurracy = difficulty * 0.05f;

	Tag("Enemy");

	collider = new Collidable(vec2(2));
	AttachComponent(collider);

	rigidbody = new Movable(collider);
	AttachComponent(rigidbody);

	destruct = new Destructable();
	AttachComponent(destruct);

	destruct->maxDamage = 1;

	if (target->className == "Player") playerTarget = true;
}

void Drone::Update()
{
	//Update Player Target
	if (playerTarget && Game::player) target = Game::player;

	//Targetting Calculations
	bool inRange = distance(position, target->position) < shootDistance;
	float dist = distance(position, target->position);
	vec2 localDir = normalize(target->position - position);

	Physics::CollisionInfo info = Physics::Raycast(position, target->position, viewDistance * 2, collider->uid, viewDistance * 2);

	//Raycast to player
	if (distance(position, target->position) < viewDistance)
	{
		if (info)
		{
			if (info.component->owner->uid != target->uid)
			{
				//Find path to target
				Room* room = Room::getRoom(position);
				std::vector<vec2> path = Pathfinding::FindPath(room->tilemap->data, room->toLocalSpace(position), room->toLocalSpace(target->position), room->tilemap->width, room->tilemap->height, 0);

				for (int i = 0; i < path.size(); i++)
				{
					//Find node to move to
					if (distance(position, room->toWorldSpace(path[i])) >= 1)
					{
						localDir = normalize((room->toWorldSpace(path[i]) + vec2(0.5)) - position);
						break;
					}
				}
			}
		}
	}

	//Set Movement Direction
	Dir = lerp(Dir, localDir, (lazer && inRange) ? 0.15f:0.2f);

	//Disable if out of range
	if (distance(position, target->position) > viewDistance)
	{
		rigidbody->velocity = lerp(rigidbody->velocity, vec2(0), 0.1f);
		return;
	}

	//Set Velocity
	if (inRange)
		rigidbody->velocity = lerp(rigidbody->velocity, vec2(0), 0.1f);
	else 
		rigidbody->velocity = normalize(lerp(rigidbody->velocity, Dir, acurracy)) * 0.2f;

	//Shooting Bullet
	if (inRange && info && !lazer)
	{
		if (shootTimer.getInterval(rateOfFire)) 
		{
			Bullet* b = new Bullet(position + Dir * 2.5f, Dir * 0.3f, damage);
			b->shocking = shocking;
			World::AddEntity(b);
		}
	}
	 
	//Shooting Lazer
	if (inRange && info && lazer)
	{
		if (shootTimer.getInterval(rateOfFire))
			lazerShooting = !lazerShooting;

		//Raycast for hit position
		Physics::CollisionInfo visualHit = Physics::Raycast(position + (Dir * 2.f), position + (Dir * 56.f), 144.f, collider->uid);
		if (visualHit && lazerShooting)
		{
			lazerLocation = visualHit.position;

			//Do damage
			World::Component* destruct = visualHit.entt->GetComponent("Destructable");
			if (destruct)
			{
				//TODO: Make this variable
				//Try to find Destructable Component
				((Destructable*)destruct)->TakeDamage(0.02f);
			}
		}
	}
	else lazerShooting = false;
}

void Drone::Draw()
{
	bool inRange = distance(position, target->position) < viewDistance;

	static quat rotation = quat();
	if(inRange && active) rotation = glm::quatLookAt(vec3(Dir, 0), vec3(0, 0, 1));
	
	Render::DrawTri({ position.x, position.y, 10 }, eulerAngles(rotation).z, vec2(1), vec4((shocking)? 1.0:0.5, (lazer) ? 0.5 : 0.1, 1.0, (active) ? 1:0.5));

	if (GameState::Mode == EditMode)
		Render::DrawCircle(vec3(position, 2), vec2(viewDistance*2), vec4(1, 1, 1, 0.15), true, 0.01f);

	if (lazerShooting && lazer)
	{
		//Calculate Lazer quad props
		float opacity = (active) ? 1.0f : 0.5f;
		float dist = length(lazerLocation - position);
		vec2 direct = normalize(lazerLocation - position);
		quat rotation = glm::quatLookAt(vec3(direct, 0), vec3(0, 0, 1));

		//Draw Lazer Beam
		Render::DrawQuad(vec3(position + (direct * (dist / 2.f)), 13), eulerAngles(rotation).z, vec2(0.5, dist), vec4(1, 0.5, 0, 1));
	}
}

void Drone::Editor()
{
	ImGui::Text("Target UID: "); ImGui::SameLine();
	ImGui::TextColored(ImVec4(1,1,1,0.25),std::to_string(target->uid).c_str());

	Editor::Int("Damage", &damage);
	Editor::Float("Accuracy", &acurracy);
	Editor::Float("View Distance", &viewDistance);
	Editor::Float("Fire Distance", &shootDistance);
	Editor::Float("Fire Rate", &rateOfFire);

	Editor::CheckBox("Shocking", &shocking);
	Editor::CheckBox("Lazer", &lazer);
}

nlohmann::json Drone::Save()
{
	nlohmann::json data;

	data["damage"] = damage;
	data["acurracy"] = acurracy;
	data["shootDistance"] = shootDistance;
	data["viewDistance"] = viewDistance;
	data["fireRate"] = rateOfFire;
	data["shocking"] = shocking;
	data["lazer"] = lazer;

	return data;
}

void Drone::Load(nlohmann::json data)
{
	if (data.contains("damage")) damage = data["damage"].get<int>();
	if (data.contains("acurracy")) acurracy = data["acurracy"].get<float>();
	if (data.contains("shootDistance")) shootDistance = data["shootDistance"].get<float>();
	if (data.contains("viewDistance")) viewDistance = data["viewDistance"].get<float>();
	if (data.contains("fireRate")) rateOfFire = data["fireRate"].get<float>();
	if (data.contains("shocking")) shocking = data["shocking"].get<bool>();
	if (data.contains("lazer")) lazer = data["lazer"].get<bool>();
}
