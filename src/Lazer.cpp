#include "Lazer.h"

#include <set>

#include "File.h"
#include "State.h"

#include "Props.h"
#include "Manip.h"
#include "Physics.h"

#include "Collidable.h"
#include "Destructable.h"

using namespace glm;

Lazer::Lazer()
{
	className = "Lazer";
	Tag("Obstacle");

	collider = new Collidable(vec2(2));
	AttachComponent(collider);

	destruct = new Destructable();
	AttachComponent(destruct);
}

void Lazer::Awake(){}

void Lazer::Update()
{
	if (flipTimer.getInterval(flipInterval)) active = !active;

	float dist = glm::length(beamTarget);
	vec2 direction = normalize(beamTarget);

	//TODO: Rework to use new Lazer system similar to Phazer

	//List of all Collidables that have been hit
	std::set<int> hitHistory;
	
	if (active && damageTimer.getInterval(damageInterval))
	{
		//Iterate over distance of lazer
		for (float i = 0; i < dist; i++) 
		{
			vec2 point = position + (direction * i);

			//Collision check
			Physics::CollisionInfo info = Physics::Collides(point, vec2(1), uid);
			if (info && info.component->owner->uid != uid)
			{
				World::Component* comp = info.component->owner->GetComponent("Destructable");
			
				//Check if collider has already been hit
				if (comp && hitHistory.find(comp->uid) == hitHistory.end()) 
				{
					((Destructable*)comp)->TakeDamage(damage);
					hitHistory.insert(comp->uid);
				}
			}
		}
	}
}

void Lazer::Draw()
{
	float opacity = (Entity::active) ? 1.0f : 0.5f;
	float dist = glm::length(beamTarget);
	vec2 direction = normalize(beamTarget);
	quat rotation = glm::quatLookAt(vec3(direction, 0), vec3(0, 0, 1));

	//Draw Lazer
	Render::DrawQuad(vec3(position, 12), 3.14/4.0, vec2(1), vec4(1, 0.2, 0.6, opacity));
	if(active && Entity::active) Render::DrawQuad(vec3(position + (direction * (dist/2.f)), 13), eulerAngles(rotation).z, vec2(0.5, dist), vec4(1, 0.5, 0, 1));

	if (GameState::Mode == EditMode)
	{
		//Draw Beam Target
		Render::DrawQuad(vec3(beamTarget + position, 2), 3.14 / 4.0, vec2(0.75), vec4(1, 0.2, 0.6, 0.75));
		Render::DrawQuad(vec3(position + (direction * (dist / 2.f)), 13), eulerAngles(rotation).z, vec2(0.5, dist), vec4(1, 0.5, 0, 0.25));
	}
}

void Lazer::Editor()
{
	Editor::Float("Flip Interval", &flipInterval);
	Editor::Float("Damage Interval", &damageInterval);
	Editor::Int("Hit Damage", &damage);

	beamTarget = Editor::Manipulator(uid + 1, beamTarget + position) - position;
	Editor::Float2("Beam Target", &beamTarget[0]);
}

nlohmann::json Lazer::Save()
{
	nlohmann::json data;

	data["flipInterval"] = flipInterval;
	data["damageInterval"] = damageInterval;
	data["damage"] = damage;

	data["target"] = File::JsonVec2(beamTarget);

	return data;
}

void Lazer::Load(nlohmann::json data)
{
	if (data.contains("flipInterval")) flipInterval = data["flipInterval"].get<float>();
	if (data.contains("damageInterval")) damageInterval = data["damageInterval"].get<float>();
	if (data.contains("damage")) damage = data["damage"].get<int>();

	if (data.contains("target")) beamTarget = File::Vec2Json(data["target"]);
}
