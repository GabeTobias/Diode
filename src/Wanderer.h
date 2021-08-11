#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "nlohmann.hpp"

#include "World.h"
#include "Time.h"

class Movable;
class Collidable;
class Room;

class Wanderer: public World::Entity
{
public:
	Collidable* collider;
	Movable* rigidbody;
	Room* room = nullptr;

	std::vector<glm::vec2> path;

	Time::Timer searchTimer;
	float searchRate = 60;
	float acurracy = 0.01;
public:
	Wanderer(glm::vec2 pos);

	void Start();

	void Update();
	void Draw();

	void Editor();

	nlohmann::json Save();
	void Load(nlohmann::json data);

	static World::Entity* Construct() { return new Wanderer(glm::vec2(0)); }
};