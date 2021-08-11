#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "World.h"
#include "Tilemap.h"

#include "nlohmann.hpp"

class Room : public World::Entity 
{
public:
	Tilemap* tilemap;
	bool roomActive = false;

	std::vector<nlohmann::json> state;

public:
	Room(glm::vec2 position, glm::vec2 size = glm::vec2(24));

	void Start();

	void Update();
	void Draw();

	void Editor();

	nlohmann::json Save();
	void Load(nlohmann::json data);

	bool Contains(glm::vec2 pos);

	glm::vec2 toLocalSpace(glm::vec2 pos);
	glm::vec2 toWorldSpace(glm::vec2 pos);

	void StoreState();
	void ReloadState();

	void SetEnttActive(bool state);

	static Room* getRoom(glm::vec2 position);

	static World::Entity* Construct() { return new Room(glm::vec2()); }
};