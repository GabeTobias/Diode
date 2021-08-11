#pragma once

#include "World.h"

class LazerPilot : public World::Entity
{
public:
	bool hasPlayer = false;

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