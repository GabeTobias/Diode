#include "Missile.h"

#include "Physics.h"
#include "Pathfinding.h"

#include "Player.h"

#include "Destructable.h"
#include "Collidable.h"
#include "Movable.h"

#include "Room.h"

using namespace glm;

Missile::Missile(glm::vec2 pos, glm::vec2 velocity, int damage, World::Entity* target)
{
	this->className = "Missile";
	this->position = pos;
	this->damage = damage;

	Tag("Roomless");

	collider = new Collidable(vec2(0.1));
	AttachComponent(collider);

	rigidbody = new Movable(collider);
	AttachComponent(rigidbody);

	rigidbody->velocity = velocity;
	collider->layer = 10;

	this->target = target;
	if (target == nullptr) 
		this->target = World::GetEntityOfClass("Player");
}

void Missile::Update()
{
	//Steering
	vec2 Dir = normalize(target->position - position);

	//Path Finding
	{
		Physics::CollisionInfo info = Physics::Raycast(position, target->position, 24 * 2, collider->uid, 100);
		if (info)
		{
			//Check if target is visible
			if (info.component->owner->uid != target->uid)
			{
				//Get Path from room data
				Room* room = Room::getRoom(position);
				std::vector<vec2> path = Pathfinding::FindPath(room->tilemap->data, room->toLocalSpace(position), room->toLocalSpace(target->position), room->tilemap->width, room->tilemap->height, 0);

				//Find Nearest Node
				for (int i = 0; i < path.size(); i++)
				{
					if (distance(position, room->toWorldSpace(path[i])) >= 1)
					{
						Dir = normalize((room->toWorldSpace(path[i]) + vec2(0.5)) - position);
						break;
					}
				}
			}
		}
	}

	//Final Velocity
	rigidbody->velocity = normalize(lerp(normalize(rigidbody->velocity), Dir, acurracy)) * length(rigidbody->velocity);

	//Collision Detection
	if (rigidbody->collided)
	{
		World::Component* destruct = rigidbody->collided->GetComponent("Destructable");
		if (destruct)
		{
			//Try to find Destructable Component
			((Destructable*)destruct)->TakeDamage(damage);
		}

		//Delete Actor
		World::DeleteEntity(this);
	}
}

void Missile::Draw()
{
	quat rotation = glm::quatLookAt(vec3(rigidbody->velocity, 0), vec3(0, 0, 1));
	Render::DrawQuad({ position.x, position.y, 10 }, eulerAngles(rotation).z, vec2(0.5,1), vec4(1, 1, 1, 1));
}
