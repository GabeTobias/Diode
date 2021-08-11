#pragma once

#include <glm/glm.hpp>

#include "World.h"
#include "Time.h"

class Movable;
class Collidable;

class Bullet : public World::Entity
{
public:
	Collidable* collider;
	Movable* rigidbody;

	int damage = 1;
	int lifetime = 3;

	bool shocking = false;
	Time::Timer deathTime;
public:
	Bullet(glm::vec2 pos, glm::vec2 velocity, int damage = 1, float lifetime = 1);

	void Awake();
	
	void Update();
	void Draw();
};