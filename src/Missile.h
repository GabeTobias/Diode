#pragma once

#include <glm/glm.hpp>

#include "World.h"

class Player;
class Movable;
class Collidable;

class Missile : public World::Entity
{
public:
	Collidable* collider;
	Movable* rigidbody;

	World::Entity* target;

	int damage = 1;
	float acurracy = 0.05;

public:
	Missile(glm::vec2 pos, glm::vec2 velocity, int damage = 2, World::Entity* target = nullptr);

	void Update();
	void Draw();
};