#include "Texture.h"
#include "Window.h"

#define STB_IMAGE_IMPLEMENTATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"

#include <iostream>
#include <memory>
#include <fstream>

extern Window* window;

void Texture::LoadTexture(std::string filename)
{
	if (filename == "") return;

	glGenTextures(1, &texture_ptr);
	glBindTexture(GL_TEXTURE_2D, texture_ptr);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// load and generate the texture
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		//Scale to matrix
		glm::vec3 scale = glm::vec3(width / window->width, height / window->height, 1);
		TextureScale = glm::scale(glm::mat4(1.0f), (scale * 5.0f));

		name = filename;
	}
	else
	{
		std::cout << "Failed to load texture " << filename << std::endl;
	}
	stbi_image_free(data);
}

void Texture::LoadSprite(std::string filename, int w, int h) 
{
	glGenTextures(1, &texture_ptr);
	glBindTexture(GL_TEXTURE_2D, texture_ptr);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// load and generate the texture
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		//Scale to matrix
		//TODO: Calculate on a per tile level for non-uniform sprite sheets
		glm::vec3 scale = glm::vec3((width/w) / window->width, (height/h) / window->height, 1);
		TextureScale = glm::scale(glm::mat4(1.0f), scale * 5.0f);

		isSprite = true;
		tsWidth = w;
		tsHeight = h;

		name = filename;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}