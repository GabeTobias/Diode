#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "nlohmann.hpp"

#include "World.h"
#include "Time.h"

class Collidable;
class Destructable;

class Lazer : public World::Entity
{
public:
	Collidable* collider;
	Destructable* destruct;

	glm::vec2 beamTarget;

	float flipInterval = 1;
	float damageInterval = 0.5f;
	int damage = 1;

	bool active = false;

	Time::Timer flipTimer;
	Time::Timer damageTimer;

public:
	Lazer();

	void Awake();

	void Update();
	void Draw();

	void Editor();

	nlohmann::json Save();
	void Load(nlohmann::json data);

	static World::Entity* Construct() { return new Lazer(); }
};