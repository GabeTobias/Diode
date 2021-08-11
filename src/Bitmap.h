#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "FrameBuffer.h"

class Shader;

class Bitmap : public FrameBuffer
{
public:
	inline static int quadGeo = 0;
	inline static Shader* bitmapShader = nullptr;

public:
	Bitmap(int width, int height);

	void Initialize();

	void Unbind();

	int GenerateQuad();
	void DrawQuad(glm::vec2 pos, glm::vec2 size, glm::vec4 col, float radius);
	glm::mat4 getProjection();
};