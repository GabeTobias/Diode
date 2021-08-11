#include "Camera.h"
#include "Window.h"

using namespace glm;

extern Window* window;

Camera::Camera(vec2 location, float size)
{
	this->position = location;
	this->size = size;
	this->delta = location;
}

void Camera::Update() 
{
	//Lerp delta to position
	delta = lerp(delta, vec2(0), 0.1f);

	//Update Camera View
	view = glm::lookAt(
		glm::vec3(position + delta, viewDistance),
		glm::vec3(position + delta, 0),
		glm::vec3(0, 1, 0)
	);

	//Update Camera Projection
	float aspectRatio = float(window->width) / float(window->height);
	projection = glm::ortho<float>(-size * aspectRatio, size * aspectRatio, -size, size, -viewDistance, viewDistance);
}
