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


class Health : public World::Entity
{
public:
	int amount;

public:
	Health();

	void Draw();
	void Update();

	nlohmann::json Save();
	void Load(nlohmann::json data);

	void Editor();

	static World::Entity* Construct() { return new Health(); }
};