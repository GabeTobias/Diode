#pragma once

#include <glm/glm.hpp>

#include "Input.h"
#include "Time.h"
#include "GUI.h"

namespace DEBUG 
{
	inline void Stats()
	{
		glm::vec2 mousePosition = Input::GetMouseScreenPosition();

		GUI::DrawTextUI("FrameRate: " + std::to_string(Time::frameRate), { 20, 30 }, 12.0f, { 1,1,1,1 });
		GUI::DrawTextUI("Mouse Position X:" + std::to_string(int(mousePosition.x)) + " Y:" + std::to_string(int(mousePosition.y)), { 20, 50 }, 12.0f, { 1,1,1,1 });
		GUI::DrawTextUI("Delta Time: " + std::to_string(Time::DeltaTime), { 20, 70 }, 12.0f, { 1,1,1,1 });
		GUI::DrawBoxUI({ 10,10 }, { 270,70 }, { 0,0,0,0.7 }, 0);
	}
}