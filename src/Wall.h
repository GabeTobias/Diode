#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "nlohmann.hpp"
#include "File.h"

#include "World.h"
#include "Renderer.h"

#include "Destructable.h"
#include "Triggerable.h"
#include "Collidable.h"


class Wall : public World::Entity
{
public:
	glm::vec2 Target;

	Collidable* collider;
	Destructable* destruct;
	Triggerable* trigger;

	bool triggerable = false;
	bool open = false;

public:
	Wall();

	void Awake();

	void Draw();
	void Update();
	
	nlohmann::json Save();
	void Load(nlohmann::json data);

	void Editor();

	static World::Entity* Construct() { return new Wall(); }
};