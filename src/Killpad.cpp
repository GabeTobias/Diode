#include "Killpad.h"
#include "Renderer.h"
#include "Editor.h"
#include "File.h"

#include "Coords.h"
#include "Physics.h"

using namespace glm;

Killpad::Killpad(glm::vec2 pos)
{
	className = "Killpad";
	Tag("Obstacle");

	position = pos;
}

void Killpad::Awake() { 
	editorBounds = bounds; 
	deathTimer.reset();
}

void Killpad::Update()
{
	//Flip state
	Physics::CollisionInfo info = Physics::Collides(position - (bounds / 2.f), bounds, 0, 100);
	if (info)
	{
		//Check if its a bullet or missile
		if (info.component->owner->className != "Bullet" && info.component->owner->className != "Missile" && info.component->owner->className != "Room" && info.component->owner->className != "Killpad")
		{
			if (triggered == false) 
			{
				hitTimer.reset();
				triggered = true;
			}
			
			if (hitTimer.getInterval((hitting) ? hitTime : coolDownTime))
			{
				hitting = !hitting;

				if (!hitting)
				{
					//Reactivate hitlist
					for (World::Component* c : frozen)
					{
						c->active = true;
					}

					//Clear hitlist
					frozen.clear();
				}
			}
		}
	}
	else triggered = false;

	if(hitting)
	{
		//TODO: This is really slow but i don't have a better solution yet
		//		maybe something to do with rooms?

		//Freeze Movement
		for (World::Entity* e : World::entities) 
		{
			if (PointOverlaps(position - (bounds / 2.f), bounds, e->position)) 
			{
				//Check if its a bullet or missile
				if (e->className == "Bullet" || e->className == "Missile" || e->className == "Killpad") continue;

				//Check if it has a movable component
				World::Component* comp = e->GetComponent("Movable");
				if (!comp) continue;

				//check if its alread on the hitlist
				auto find = std::find(frozen.begin(), frozen.end(), comp);
				if (find == frozen.end())
				{
					//Skip Shocking Drones
					if (e->className == "Drone" && ((Drone*)e)->shocking) continue;

					//Skip Shocking Chargers
					if (e->className == "Charger" && ((Charger*)e)->trailing) continue;

					comp->active = false;
					frozen.push_back(comp);
				}
			}
		}

		//Do Damage
		if (doDamage) 
		{
			for (World::Component* c : frozen)
			{
				World::Component* comp = c->owner->GetComponent("Destructable");
				if (comp)
				{
					((Destructable*)comp)->TakeDamage(damage);
				}
			}
		}
	}

	if (dying && deathTimer.getInterval(deathTime)) 
	{
		if (hitting) 
		{
			//Reactivate hitlist
			for (World::Component* c : frozen)
				c->active = true;

			//Clear hitlist
			frozen.clear();
		}

		World::DeleteEntity(this);
	}
}

void Killpad::Draw()
{
	vec4 color = vec4(0,0,1, (hitting && active) ? 0.3:0.1);
	Render::DrawQuad(vec3(position,11), 0, bounds, color);
}

void Killpad::Editor()
{
	Editor::Float("Hit Time", &hitTime);
	Editor::Float("Cool Down", &coolDownTime);
	
	Editor::CheckBox("Hitting", &hitting);
	Editor::CheckBox("Do Damage", &doDamage);
	if(doDamage)
		Editor::Float("Damage", &damage);

	Editor::Float2("Bounds", &bounds.x);
	editorBounds = bounds;
}

nlohmann::json Killpad::Save()
{
	nlohmann::json data;

	data["hitTime"] = hitTime;
	data["coolDownTime"] = coolDownTime;
	data["doDamage"] = doDamage;
	data["damage"] = damage;
	data["bounds"] = File::JsonVec2(bounds);

	return data;
}

void Killpad::Load(nlohmann::json data)
{
	if (data.contains("hitTime")) hitTime = data["hitTime"].get<float>();
	if (data.contains("coolDownTime")) coolDownTime = data["coolDownTime"].get<float>();
	if (data.contains("doDamage")) doDamage = data["doDamage"].get<bool>();
	if (data.contains("damage")) damage = data["damage"].get<float>();
	if (data.contains("bounds")) bounds = File::Vec2Json(data["bounds"]);
}
