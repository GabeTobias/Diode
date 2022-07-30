#include "Bullet.h"

#include "Physics.h"
#include "Killpad.h"

#include "Destructable.h"
#include "Collidable.h"
#include "Movable.h"

using namespace glm;

Bullet::Bullet(glm::vec2 pos, glm::vec2 velocity, int damage, float lifetime)
{
	this->className = "Bullet";
	this->position = pos;
	this->damage = damage;
	this->lifetime = lifetime;

	Tag("Roomless");

	collider = new Collidable(vec2(0.5));
	AttachComponent(collider);

	rigidbody = new Movable(collider);
	AttachComponent(rigidbody);

	rigidbody->velocity = velocity;
	collider->layer = 10;
}

void Bullet::Awake() 
{
	deathTime.reset();
}

void Bullet::Update()
{
	if (rigidbody->collided)
	{
		World::Component* destruct = rigidbody->collided->GetComponent("Destructable");
		if (destruct)
		{
			//Try to find Destructable Component
			((Destructable*)destruct)->TakeDamage(damage);
		}

		if (shocking)
		{
			vec2 snapPos = vec2(
				int(((position.x) / 5.f) * 5),
				int(((position.y) / 5.f) * 5)
			);

			Killpad* pad = new Killpad(snapPos);
			pad->bounds = vec2(8);
			pad->dying = true;
			pad->hitTime = 2;
			pad->coolDownTime = 0.25f;
			World::AddEntity(pad);
		}

		//Delete Actor
		World::DeleteEntity(this);
	}

	if (deathTime.getInterval(lifetime)) 
	{
		if (shocking)
		{
			vec2 snapPos = vec2(
				int(((position.x) / 5.f) * 5),
				int(((position.y) / 5.f) * 5)
			);

			//Drop KillPad
			Killpad* pad = new Killpad(snapPos);
			pad->bounds = vec2(8);
			pad->dying = true;
			pad->hitTime = 2;
			pad->coolDownTime = 0.25f;
			World::AddEntity(pad);
		}

		//Delete Actor
		World::DeleteEntity(this);
	}
}

void Bullet::Draw()
{
	quat rotation = glm::quatLookAt(vec3(rigidbody->velocity, 0), vec3(0,0,1));
	Render::DrawQuad({ position.x, position.y, 10 }, eulerAngles(rotation).z, vec2(0.5), vec4(1, 1, 1, 1));
}