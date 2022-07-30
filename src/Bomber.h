#pragma once

#include "World.h"

#include "glm/glm.hpp"

class Destructable;
class Collidable;
class Movable;

class Bomber : public World::Entity
{
public:
	bool firing = false;
	bool heatSeeking = false;

	float fireRate = 1.f;
	float restRate = 3.f;
	float viewDistance = 18;
	float fireDistance = 15;
	float speed = 0.1f;
	float accuracy = 0.1f;

	Time::Timer restTimer;
	Time::Timer fireTimer;

	Destructable* destruct;
	Collidable* collider;
	Movable* rigidbody;

public:
	Bomber();

	void Update();
	void Draw();

	void Editor();

	nlohmann::json Save();
	void Load(nlohmann::json data);

	static World::Entity* Construct() { return new Bomber(); }
};

