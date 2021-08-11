#pragma once

#include <glm/glm.hpp>

#include "World.h"
#include "Time.h"

class Shield;
class Movable;
class Collidable;

class Stock : public World::Entity
{
public:
	Collidable* collider;
	Movable* rigidbody;
	Shield* shield;

public:
	Stock(glm::vec2 pos, glm::vec2 velocity);

	void Awake();

	void Update();
	void Draw();

	static World::Entity* Construct() { return new Stock(glm::vec2(), glm::vec2()); }
};