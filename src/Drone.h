#pragma once

#include <glm/glm.hpp>

#include "nlohmann.hpp"

#include "World.h"
#include "Time.h"

class Player;
class Movable;
class Collidable;
class Destructable;

class Drone : public World::Entity
{
public:
	Destructable* destruct;
	Collidable* collider;
	Movable* rigidbody;

	World::Entity* target;
	bool playerTarget = false;
	bool shocking = false;
	bool lazer = false;

	glm::vec2 Dir;

	int damage = 1;
	float acurracy = 0.01;
	float shootDistance = 6;
	float viewDistance = 32;

	Time::Timer shootTimer;
	float rateOfFire = 2;

	bool lazerShooting = false;
	glm::vec2 lazerLocation;
public:
	Drone(World::Entity* target, glm::vec2 pos, int difficulty = 2);

	void Update();
	void Draw();

	void Editor();

	nlohmann::json Save();
	void Load(nlohmann::json data);

	static World::Entity* Construct() { return new Drone(World::GetEntityOfClass("Player"), glm::vec2(0)); }
};