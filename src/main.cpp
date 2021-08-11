#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "Window.h"
#include "Camera.h"

#include "Renderer.h"
#include "Audio.h"

#include "Time.h"
#include "Input.h"
#include "Game.h"

Camera* camera;
Window* window;

int main(void)
{
	window = new Window(16 * 96, 9 * 96, "Diode v0.7.3");
	camera = new Camera(glm::vec2(-0.5,0.5), 17);

	Render::Init();
	Audio::Init();
	Game::Init();
	Input::Bind();

	/* Loop until the user closes the window */
	while (window->isRunning())
	{
		Time::StartFrame();
		Game::Update();
		Input::Update();

		camera->Update();

		Game::Render();
		
		window->Swap();

		Time::EndFrame();
	}

	window->Close();

	glfwTerminate();
	return 0;
}