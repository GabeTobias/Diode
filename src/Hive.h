#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "World.h"
#include "Time.h"

class Drone;
class Player;
class Destructable;
class Collidable;

class Hive : public World::Entity
{
public:
	Destructable* destruct;
	Collidable* collider;

	Player* player;

	std::vector<Drone*> children;

	int initWaveCount = 4;
	int spawnerCount = 4;
	int offset = 0;

	int waveCount = 4;
	int spawnCount = 2;
	float spawnRate = 0.05;

	float viewDistance = 32;

	glm::vec2* spawnPoints;
	Time::Timer spawnTimer;
public:
	Hive(int waveCount, int spawnCount);

	void Awake();

	void Update();
	void Draw();

	void Destroy();

	void Editor();

	nlohmann::json Save();
	void Load(nlohmann::json data);

	static World::Entity* Construct() { return new Hive(4,2); }
};