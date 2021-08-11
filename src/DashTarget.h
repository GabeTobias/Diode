#pragma once

#include "World.h"

class DashTarget : public World::Entity
{
public:
	bool hasPlayer = false;	
	bool entering = true;

	float snapRadius = 6;
	float directionDot = 0;

public:
	DashTarget();

	void Awake();

	void Update();
	void Draw();

	void Editor();

	nlohmann::json Save();
	void Load(nlohmann::json data);

	static World::Entity* Construct() { return new DashTarget(); }
};

