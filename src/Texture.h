#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifndef TEXTURE_H
#define TEXTURE_H

class Texture
{
public:
	int width, height, nrChannels;
	int tsWidth = 1, tsHeight = 1, tileX = 0, tileY = 0;

	unsigned int texture_ptr;

	bool isSprite = false;

	std::string name;
	glm::mat4 TextureScale;

	void LoadTexture(std::string filename);
	void LoadSprite(std::string filename, int w, int h);
};

#endif