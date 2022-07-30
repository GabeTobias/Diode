#pragma once

#include "World.h"

#include "Collidable.h"
#include "Destructable.h"

class BulletHell : World::Entity
{
public:
	int phase = 0;

	float rotation = 0;
	float rotationSpeed = 0.01f;
	
	float phaseInterval = 8;
	float waveInterval = 1;
	float spinInterval = 1;
	
	float bulletSpeed = 0.3f;

	Destructable* destruct;
	Collidable* collider;


	Time::Timer phaseTimer;
	Time::Timer waveTimer;

public:
	BulletHell();

	void Awake();

	void Update();
	void Draw();

	void SpawnBullets(int count, float interval);

	nlohmann::json Save();
	void Load(nlohmann::json data);

	void Editor();

	static World::Entity* Construct() { return new BulletHell(); }
};

