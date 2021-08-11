#pragma once

#include "Renderer.h"
#include "Audio.h"
#include "nlohmann.hpp"

#include "Window.h"
#include "Camera.h"

#include "Debug.h"
#include "Player.h"
#include "Room.h"

#include "State.h"
#include "Editor.h"
#include "Undo.h"
#include "File.h"

#include "Pathfinding.h"

extern Camera* camera;
extern Window* window;

namespace Game 
{
	using namespace World;

	inline Player* player;
	inline nlohmann::json prePlayState;

	inline void Init()
	{
		Editor::Init();
		File::LoadScene("resources/scenes/test.json");
		Undo::Store();

		//Store Game State
		prePlayState = File::SerializeScene();

		//Start Game Scene
		World::Start();
	}

	inline void Render()
	{
		Render::Clear();
		Render::DrawCheck(1024);

		World::Draw();
		
		if (GameState::Mode != AppState::EditMode)	World::GUI();
		if (GameState::Mode == AppState::DebugMode) DEBUG::Stats();
		if (GameState::Mode == AppState::EditMode)	Editor::UI();
	}

	inline void Update()
	{
		if (Input::GetKeyDown(GLFW_KEY_F4))
			GameState::ShowOverlaps = !GameState::ShowOverlaps;
		
		if (Input::GetKeyDown(GLFW_KEY_F1))
		{
			if (GameState::Mode == AppState::EditMode) 
			{
				//Store Game state
				prePlayState = File::SerializeScene();

				//Start Game Scene
				World::Start();

				GameState::Mode = AppState::GameMode;
			}
			else 
			{
				File::DeserializeScene(prePlayState);
				GameState::Mode = AppState::EditMode;
			}

			//Clear Selection
			Editor::selectedEntt = nullptr;
		}

		if (Input::GetKeyDown(GLFW_KEY_F5))
			GameState::Mode = (GameState::Mode != AppState::GameMode) ? AppState::GameMode : AppState::EditMode;

		if (Input::GetKeyDown(GLFW_KEY_F3))
			GameState::Mode = (GameState::Mode != AppState::DebugMode) ? AppState::DebugMode : AppState::GameMode;

		if (Input::GetKeyDown(GLFW_KEY_F9))
			window->isClosing = true;

		if (GameState::Mode != AppState::EditMode) World::Update();
	}
}