#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>

//TODO: GABE WHY THE FUCK
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

//Utility inluces
#include "Window.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Tilemap.h"
#include "FrameBuffer.h"

#include "GUI.h"

extern Window* window;
extern Camera* camera;

namespace Render
{
	using namespace glm;

	inline Shader defaultShader;
	inline Shader checkerShader;

	inline int triGeo;
	inline int quadGeo;

	inline float shadowScale = 1.0f;

	inline vec2 gridOffset = vec2(0.5);
	inline vec2 shadowDistance = vec2(-0.25f);
	inline vec4 shadowColor = vec4(0,0,0,1);

	inline bool useShadow = true;
	inline bool addShadow = false;

	int GenerateTri();
	int GenerateQuad();
	int GenerateTilemapGeo(std::vector<std::vector<short int>> map, int width, int height);

	void DrawCircle(vec3 position = vec3(), vec2 size = vec2(1), vec4 color = glm::vec4(1), bool outline = false, float weight = 0.05f);
	void DrawQuad(vec3 position = vec3(), float rotation = 0, vec2 size = vec2(1), vec4 color = glm::vec4(1), bool outline = false, float weight = 0.05f);
	void DrawTri(vec3 position = vec3(), float rotation = 0, vec2 size = vec2(1), vec4 color = glm::vec4(1));

	void DrawSprite(vec3 position, Texture* texture, vec4 UV, float rotation = 0, vec2 size = vec2(1), vec4 color = vec4(1));
	void DrawFrame(vec3 position, FrameBuffer* frame, vec2 size);
	void DrawMap(unsigned int geo, int count, vec3 position = vec3(), float rotation = 0, vec2 size = vec2(1), vec4 color = glm::vec4(1), bool outline = false, float weight = 0.05f);

	void DrawCheck(int size);
	void Clear();
	void Init();

	void ReleadShaders();
}