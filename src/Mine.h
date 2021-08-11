#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "nlohmann.hpp"

#include "World.h"
#include "Time.h"

class Collidable;
class Destructable;

class Mine: public World::Entity
{
public:
	Collidable* collider;
	Destructable* destruct;

	int hits = 0;
	int maxHits = 3;
	float range = 15;

	Time::Timer hitTimer;

public:
	Mine();

	void Update();
	void Draw();

	void Editor();

	void Destroy();

	nlohmann::json Save();
	void Load(nlohmann::json data);

	static World::Entity* Construct() { return new Mine(); }
};