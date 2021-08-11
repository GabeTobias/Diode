#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "nlohmann.hpp"

#include "World.h"
#include "Time.h"

class Collidable;
class Destructable;

class Turret : public World::Entity 
{
public:
	Collidable* collider;
	Destructable* destruct;

	glm::vec2 barrelDir;
	glm::quat rotation;
	float viewDistance = 20;
	float fireRate = 2;

	int bulletType = 0;

	Time::Timer bulletTimer;
	Time::Timer panTimer;

	glm::vec2 root;

	float panSpeed = 0.1;
	float panInterval = 2;

	bool panning = false;
	bool panTarget = false;
	glm::vec2 offsetA = glm::vec2(-2,0);
	glm::vec2 offsetB = glm::vec2( 2,0);
public:
	Turret();

	void Awake();

	void Update();
	void Draw();

	void Editor();

	nlohmann::json Save();
	void Load(nlohmann::json data);

	static World::Entity* Construct() { return new Turret(); }
};