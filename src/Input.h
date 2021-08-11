#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

//TODO: Maybe I shoudl get rid of this?
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>

#include <map>

#include "Window.h"
#include "Coords.h"

extern Window* window;

struct GamePad 
{
	bool active = false;
	bool buttons[15];
	float axis[6];
};

namespace Input 
{
	inline glm::vec2 scrollDelta;
	inline GamePad gamePads[4];

	inline bool GetMousebutton(int btn) 
	{
		return glfwGetMouseButton(window->window, btn) == GLFW_PRESS;
	}

	inline bool GetKey(int key)
	{
		return glfwGetKey(window->window, key) == GLFW_PRESS;
	}

	inline std::map<int, bool> prevState;
	inline bool GetKeyDown(int key)
	{
		if (prevState.find(key) == prevState.end()) prevState[key] = false;

		if (Input::GetKey(key) && !prevState[key])
		{
			//Key Down
			prevState[key] = Input::GetKey(key);
			return true;
		}

		//Key Up
		prevState[key] = Input::GetKey(key);
		return false;
	}

	inline int isController(int controller) 
	{
		return glfwJoystickIsGamepad(controller);
	}

	inline glm::ivec2 GetMouseScreenPosition()
	{
		//Get Coord from glfw
		glm::dvec2 val;
		glfwGetCursorPos(window->window, &val.x, &val.y);
		
		//return vector
		return val;
	}

	inline glm::vec2 GetMouseWorldPosition()
	{
		double x, y;
		glfwGetCursorPos(window->window, &x, &y);

		return viewToWorldCoordTransform(x, y);
	}

	inline void Scroll(GLFWwindow* window, double xoffset, double yoffset)
	{
		scrollDelta = glm::ivec2(xoffset, yoffset);
	}


	inline void Bind()
	{
		//Bind Scroll Function
		glfwSetScrollCallback(window->window, Scroll);
	}

	inline void Update()
	{
		Input::scrollDelta = glm::lerp(glm::vec2(scrollDelta), glm::vec2(0, 0), 0.3f);

		//Update GamePads
		for (int i = 0; i < 4; i++) 
		{
			GLFWgamepadstate padState;
			if (glfwGetGamepadState(GLFW_JOYSTICK_1 + i, &padState))
			{
				gamePads[i].active = true;

				for (int j = 0; j < 6; j++)
					gamePads[i].axis[j] = padState.axes[j];

				for (int j = 0; j < 15; j++)
					gamePads[i].buttons[j] = padState.buttons[j];
			}
			else { gamePads[i].active = false; }
		}
	}
}