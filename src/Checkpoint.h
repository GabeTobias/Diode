#pragma once

#include "World.h"

class Checkpoint : public World::Entity
{
public:
	int pointID;
	glm::vec4 color = glm::vec4(1);

public:
	Checkpoint();

	void Update();
	void Draw();

	nlohmann::json Save();
	void Load(nlohmann::json data);

	void Editor();
	
	static World::Entity* Construct() { return new Checkpoint(); }
};