#pragma once

#include <glm/glm.hpp>

#include "World.h"

class Destructable;
class Collidable;
class Hearable;
class Movable;

class Phazer: public World::Entity
{
public:
	Destructable* destruct;
	Collidable* collider;
	Hearable* aSource;
	Movable* rigidbody;

	Time::Timer cooldownTimer;
	float fireTime = 4;
	float coolDownTime = 4;
	float damage = 0.025;
	float turnSpeed = 0.02;
	float viewDistance = 16;

	glm::vec2 direction;
	glm::vec2 hitPosition;
	glm::vec2 shootDirection;
	bool shooting = false;
	bool inRange = false;	
	bool ranged = false;

public:
	Phazer(glm::vec2 pos);

	void Update();
	void Draw();

	void Editor();

	nlohmann::json Save();
	void Load(nlohmann::json data);

	static World::Entity* Construct() { return new Phazer(glm::vec2(0)); }
};

