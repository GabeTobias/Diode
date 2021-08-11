#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "World.h"
#include "Time.h"

#include "nlohmann.hpp"

class Destructable;
class Collidable;
class Movable;

class Player : public World::Entity
{
public:
	float angularSpeed = 0.2f;
	float angle = 0.f;
	glm::quat rotation;

	float sensitivity = 0.1f;
	float acceleration = 0.2f;
	float drag = 0.015f;

	bool dashing = false;
	bool targeting = false;

	int dashCount = 0;
	int dashMax = 3;
	float dash = 1.0;
	
	int currentCheckpoint = 0;

	float energy = 100;
	float energyMax = 100;
	float energyRegen = 1;

	glm::vec2 target;
	World::Entity* missileTarget = nullptr;

	Destructable* destruct;
	Collidable* collider;
	Movable* movement;

	Time::Timer bulletTimer;
	Time::Timer dashTimer;
	Time::Timer dashResetTimer;
public:
	Player(glm::vec2 position);

	void Awake();			//Fires On AddActor
	void Start();			//Fires On Scene Start

	void Destroy();

	void Draw();
	void Update();

	void Editor();
	void GUI();

	nlohmann::json Save();
	void Load(nlohmann::json data);

	static World::Entity* Construct() { return new Player(glm::vec2(0)); }
};

/*
	Collidable
	  - Bounds
	  - Collision
	Movable
	  - Linear Velocity
*/
