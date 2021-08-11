#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "World.h"


class Killpad : public World::Entity
{
public:
	bool hitting = false;
	bool doDamage = false;
	bool triggered = false;
	bool dying = false;

	Time::Timer hitTimer;
	Time::Timer deathTimer;

	glm::vec2 bounds = glm::vec2(1);

	float deathTime = 8;
	float hitTime = 4;
	float coolDownTime = 4;
	float damage = 0.01;

	std::vector<World::Component*> frozen;

public:
	Killpad(glm::vec2 pos);

	void Awake();
	
	void Update();
	void Draw();

	void Editor();

	nlohmann::json Save();
	void Load(nlohmann::json data);

	static World::Entity* Construct() { return new Killpad(glm::vec2(0)); }
};

