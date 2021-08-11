#pragma once

#define GLM_ENABLE_EXPERIMENTAL

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera 
{
public:
	glm::mat4 view;
	glm::mat4 projection;

	//Position & Size
	glm::vec2 position;
	glm::vec2 delta;
	float size;

	//Depth
	float viewDistance = 25;

public:
	Camera(glm::vec2 location, float size);
	void Update();
};