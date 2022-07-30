#include "Bomb.h"

#include "Game.h"
#include "Time.h"

#include "Collidable.h"
#include "Movable.h"

using namespace glm;

Bomb::Bomb(glm::vec2 pos, glm::vec2 target, int damage, float radius, float lifetime, bool shocking)
{
	position = pos;
	this->targetPos = target;
	this->damage = damage;
	this->radius = radius;
	this->lifetime = lifetime;
	this->shocking = shocking;
	this->heatSeeking = heatSeeking;

	visualRadius = 1;

	Tag("Roomless");
}

Bomb::Bomb(glm::vec2 pos, World::Entity* target, int damage, float radius, float lifetime, bool shocking)
{
	position = pos;
	this->target = target;
	this->damage = damage;
	this->radius = radius;
	this->lifetime = lifetime;
	this->shocking = shocking;
	this->heatSeeking = heatSeeking;

	visualRadius = 1;

	Tag("Roomless");
}


void Bomb::Awake()
{
	spawnTime = Time::getTime();
}

void Bomb::Update()
{
	float dist = 0;

	if (target == nullptr)
	{
		position = lerp(position, targetPos, 0.025f);
		dist = distance(position, targetPos);
	}
	else 
	{
		position = lerp(position, target->position, 0.025f);
		dist = distance(position, target->position);
	}

	if ( (dist < 2 && target == nullptr) || (Time::getTime() - spawnTime > lifetime && target != nullptr) )
	{
		if (!explode)
		{
			//Player Hit
			std::vector<World::Component*> movables = World::GetComponentsOfClass("Movable");
			for (World::Component* comp : movables)
			{
				if (distance(position, comp->owner->position) < radius)
				{
					Movable* move = (Movable*)comp;
					vec2 dir = Game::player->position - position;

					move->passive = normalize(dir) * 0.75f;
					
					if (comp->owner->hasTag("NotBombable"))
						continue;

					//Player Damage
					Destructable* pHealth = (Destructable*)comp->owner->GetComponent("Destructable");
					if (pHealth) pHealth->TakeDamage(damage);
				}
			}

			explode = true;
		}

		visualRadius = lerp(visualRadius, radius*2, 0.1f);
		if ((radius*2) - visualRadius < 1.f) World::DeleteEntity(this);
	}
}

void Bomb::Draw()
{
	//Draw Hover Indicator
	Render::DrawCircle(vec3(position, 11), (visualRadius) + vec2(1), vec4(0, 0, 0, 1), true, 0.03);
	Render::DrawCircle(vec3(position, 11), (visualRadius) + vec2(1), vec4(0, 0, 0, 0.5));
}
