#include "Window.h"

#include <iostream>

extern Window* window;

void Resize(GLFWwindow* win, int width, int height)
{
	window->width = width;
	window->height = height;
}

Window::Window(int width, int height, const char* name) 
{
	//Initialize GLFW
	glfwInit();

	//Set Window Hints
	glfwWindowHint(GLFW_SAMPLES, 1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);

	//Get Monitor
	int count;
	GLFWmonitor* monitor = glfwGetMonitors(&count)[0];

	//Get Monitor Mode
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	this->width = (isFullScreen) ? mode->width : width;
	this->height = (isFullScreen) ? mode->height : height;

	//Generate Window
	if(isFullScreen) this->window = glfwCreateWindow(mode->width, mode->height, name, glfwGetPrimaryMonitor(), nullptr);
	else this->window = glfwCreateWindow(width, height, name, nullptr , nullptr);

	if (this->window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}

	//Log Window State
	printf("Window Created \n");

	//Console Spacing for clearity
	printf("--------------------------------------------------- \n");

	glfwMakeContextCurrent(this->window);

	glfwSetWindowSizeCallback(this->window, Resize);

	//TODO: WHY DON'T I HAVE A PROPER PLATFORM LAYER AHHHH
	if (glewInit() != GLEW_OK) printf("Glew init fail \n");
}

void Window::SetTitle(std::string name)
{
	std::string newString = "Diode v0.0.1 | " + name;
	glfwSetWindowTitle(this->window, newString.c_str());
}

void Window::Swap() 
{
	//check and call events
	glfwSwapBuffers(this->window);
	glfwPollEvents();
}

void Window::Close() 
{
	//Destroy window context
	glfwDestroyWindow(this->window);

	//Terminate GLFW
	glfwTerminate();
}

bool Window::isRunning() { return !glfwWindowShouldClose(this->window) && !isClosing; }
