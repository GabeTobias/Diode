#pragma once

#include <glm/glm.hpp>

#include "World.h"

class Destructable;
class Collidable;
class Movable;

class Charger : public World::Entity
{
public:
	Destructable* destruct;
	Collidable* collider;
	Movable* rigidbody;

	Time::Timer cooldownTimer;
	Time::Timer trailTimer;
	float rateOfAttack = 4;
	float chargeSpeed = 0.5;
	int damage = 4;

	float attackRadius = 16;
	bool engaged = false;
	bool attacking = false;

	bool explosive = false;
	bool trailing = false;

	int dashCount;

	glm::vec2 chargeDirection;
	glm::vec2 chargeStartPosition;

	glm::vec2 offset = glm::vec2(0,0.5);
	glm::quat rotation = glm::quat();

public:
	Charger(glm::vec2 pos);

	void Update();
	void Draw();

	void Editor();

	nlohmann::json Save();
	void Load(nlohmann::json data);

	static World::Entity* Construct() { return new Charger(glm::vec2(0)); }
};

