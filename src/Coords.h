#pragma once

#include <glm/glm.hpp>

#include "Window.h"
#include "Camera.h"

extern Camera* camera;
extern Window* window;

// SCREEN SPACE: mouse_x and mouse_y are screen space
inline glm::vec3 viewToWorldCoordTransform(int mouse_x, int mouse_y)
{
	// NORMALISED DEVICE SPACE
	double x = 2.0 * mouse_x / window->width - 1;
	double y = 2.0 * mouse_y / window->height - 1;

	// HOMOGENEOUS SPACE
	glm::vec4 screenPos = glm::vec4(x, -y, -1.0f, 1.0f);

	// Projection/Eye Space
	glm::mat4 ProjectView = camera->projection * camera->view;
	glm::mat4 viewProjectionInverse = inverse(ProjectView);
	glm::vec4 worldPos = viewProjectionInverse * screenPos;

	return glm::vec3(worldPos);
}

// WORLD SPACE: 
inline glm::vec2 worldToViewCoordTransform(glm::vec3 p)
{
	glm::mat4 ProjectView = camera->projection * camera->view;
	glm::mat4 model = glm::translate(glm::mat4(1.0), p);

	//transform world to clipping coordinates
	p = ProjectView * model * glm::vec4(0, 0, 0, 1);
	p /= p.z;

	float x = ((p.x + 1) / 2.f) * window->width;
	float y = ((p.y + 1) / 2.f) * window->height;

	return glm::vec2(
		x,
		window->height - y
	);
}

inline bool PointOverlaps(glm::vec2 bpos, glm::vec2 bsize, glm::vec2 point)
{
	return (
		point.x > bpos.x && point.x < (bpos.x + bsize.x) &&
		point.y > bpos.y && point.y < (bpos.y + bsize.y)
	);
}

inline bool BoxOverlaps(glm::vec2 apos, glm::vec2 asize, glm::vec2 bpos, glm::vec2 bsize)
{
	return (
		apos.x + asize.x > bpos.x && apos.x < bpos.x + bsize.x &&
		apos.y + asize.y > bpos.y && apos.y < bpos.y + bsize.y
	);
}