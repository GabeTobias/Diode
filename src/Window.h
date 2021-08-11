#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

#ifndef WINDOW_H
#define WINDOW_H

class Window
{
public:
	GLFWwindow* window;

	int width = 1200;    // Width of the OpenGL viewport
	int height = 1200;   // Height of the OpenGL viewport

	bool isFullScreen = false;
	bool isClosing = false;

	Window(int width, int height, const char* name);

	void SetTitle(std::string name);

	void Swap();
	void Close();
	
	bool isRunning();
};

#endif