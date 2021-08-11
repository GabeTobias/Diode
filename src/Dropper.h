#pragma once

#include "World.h"
#include "Time.h"

class Destructable;
class Collidable;

class Dropper : public World::Entity
{
public:
	Destructable* destruct;
	Collidable* collider;

	glm::vec2 size = glm::vec2(2);

	float moveSpeed = 0.025f;
	float dropTime = 1;
	float sitTime = 1;
	float damage = 1;

	bool inAir = false;
	Time::Timer dropTimer;
	Time::Timer sitTimer;

public:
	Dropper(glm::vec2 size);

	void Awake();
	
	void Update();
	void Draw();

	void Editor();

	nlohmann::json Save();
	void Load(nlohmann::json data);

	static World::Entity* Construct() { return new Dropper(glm::vec2(2)); }
};

// Future gabe, this guy just jumps up and the air and falls down on top of the
// player. If you need reference look at Hades bombing behavior. IDK if I want 
// them to chase you or just pick a destination when they jump. Chasing is more
// stressful but predetermined landings is more chaotic.