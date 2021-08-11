#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>

#include "World.h"
#include "State.h"
#include "Physics.h"
#include "Time.h"

#include "Collidable.h"

#include "ImGui.h"
#include "Props.h"

class Movable : public World::Component
{
public:
	Collidable* collider;
	World::Entity* collided = nullptr;

	glm::vec2 velocity = glm::vec2(0);
	glm::vec2 passive = glm::vec2(0);
	bool colliding = false;
	bool drifting = false;

	inline glm::vec2 getVelocity() { return velocity + passive; }

	inline Movable(Collidable* col = 0) : velocity(glm::vec2()), collider(col) { className = "Movable"; }
	
	inline void Update()
	{
		colliding = false;
		collided = nullptr;

		//X Collision & Movement
		Physics::CollisionInfo colInfo = Physics::Collides(owner->position + collider->position + glm::vec2(getVelocity().x, 0) - (collider->bounds/2.f), collider->bounds, collider->uid, collider->layer);
		if (!colInfo) owner->position.x += getVelocity().x * Time::timeScale;
		else
		{
			colliding = true;
			collided = colInfo.entt;

			//TODO: 2.f is your restitution
			if (drifting) velocity.x = -(velocity.x / 5.f);
		}

		//Y Collision & Movement
		colInfo = Physics::Collides(owner->position + collider->position + glm::vec2(0, getVelocity().y) - (collider->bounds / 2.f), collider->bounds, collider->uid, collider->layer);
		if (!colInfo) owner->position.y += getVelocity().y * Time::timeScale;
		else 
		{
			colliding = true;
			collided = colInfo.entt;

			//TODO: 2.f is your restitution
			if (drifting) velocity.y = -(velocity.y / 5.f);
		}

		//Passive Reset
		passive = glm::lerp(passive, glm::vec2(0), 0.1f);
	}

	inline void Draw() 
	{
		if (GameState::Mode == AppState::DebugMode) {
			Render::DrawQuad(glm::vec3(collider->owner->position + collider->position + velocity, 5), 0, collider->bounds, glm::vec4(1,1,1,0.5));
			Render::DrawQuad(glm::vec3(collider->owner->position + collider->position + velocity, 5), 0, collider->bounds, glm::vec4(1), true, camera->size / (glm::max(collider->bounds.x, collider->bounds.y) * 300));
		}
	}

	inline void Editor() 
	{
		Editor::CheckBox("Colliding", &colliding);
	}

	inline static World::Component* Construct(World::Entity* ent)
	{
		Movable* m = new Movable((Collidable*)ent->GetComponent("Collidable"));
		return m;
	}
};