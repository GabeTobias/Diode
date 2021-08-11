#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "World.h"
#include "Bitmap.h"

#define TILESIZE 64
#define TILESCALE 2.f

class Tilemap : public World::Component 
{
public:
	std::vector<std::vector<short int>> data;

	int width = 16;
	int height = 16;

	unsigned int geo;
	unsigned int geoCount;

	glm::vec2 position;

	bool editing = false;

public:
	Tilemap(glm::vec2 size = glm::vec2(32));

	void GenGeo();

	void Update();
	void Draw();

	void Delete();

	void Editor();
};