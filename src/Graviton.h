#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "World.h"


class Graviton : public World::Entity
{
public:
	glm::vec2 bounds = glm::vec2(1);
	glm::vec2 gravity = glm::vec2(1);

public:
	Graviton(glm::vec2 pos);

	void Awake();

	void Update();
	void Draw();

	void Editor();

	nlohmann::json Save();
	void Load(nlohmann::json data);

	static World::Entity* Construct() { return new Graviton(glm::vec2(0)); }
};

