#pragma once

#include "World.h"

class LazerPilot : public World::Entity
{
public:
	bool hasPlayer = false;

	float snapRadius = 3;

public:
	LazerPilot();

	void Awake();

	void Update();
	void Draw();

	void Editor();

	nlohmann::json Save();
	void Load(nlohmann::json data);

	static World::Entity* Construct() { return new LazerPilot(); }
};