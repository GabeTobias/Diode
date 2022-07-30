#pragma once

#include "World.h"
#include "Time.h"

#include "glm/glm.hpp"

class Collidable;
class Movable;

class Bomb : public World::Entity
{
public:
	glm::vec2 targetPos;
	World::Entity* target = nullptr;

	int damage = 1;
	int lifetime = 3;
	float radius = 5;
	bool shocking = false;
	bool heatSeeking = false;

	bool explode;
	float visualRadius = 0;
	float spawnTime;

public:
	Bomb(glm::vec2 pos, glm::vec2 target, int damage = 1, float radius = 1, float lifetime = 1, bool shocking = false);
	Bomb(glm::vec2 pos, World::Entity* target, int damage = 1, float radius = 1, float lifetime = 1, bool shocking = false);

	void Awake();

	void Update();
	void Draw();
};

