#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "nlohmann.hpp"

#include "World.h"
#include "Time.h"

class Collidable;
class Movable;
class Pannable;

class Breaker : public World::Entity
{
public:
	Collidable* collider;
	Movable* rigidbody;
	Pannable* pan;

	glm::vec2 size;
	
	Time::Timer hitTimer;

public:
	Breaker();

	void Update();
	void Draw();

	void Editor();

	nlohmann::json Save();
	void Load(nlohmann::json data);

	static World::Entity* Construct() { return new Breaker(); }
};