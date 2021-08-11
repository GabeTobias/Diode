#pragma once

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "nlohmann.hpp"

#include "Props.h"

#include "World.h"
#include "Coords.h"
#include "State.h"

#include "Renderer.h"
#include "GUI.h"

class Note : public World::Entity
{
public:
	std::string text = "Hello World";
	float textWidth = 64;
	glm::vec4 color = glm::vec4(1);

public:
	Note() { className = "Note"; editorBounds = glm::vec2(4); }


	inline void Draw()
	{
		if (GameState::Mode == EditMode) 
		{
			glm::vec2 screenPosition = worldToViewCoordTransform(glm::vec3(position,0));

			glm::ivec2 lineBounds = GUI::DrawWrappedTextUI(text, screenPosition + glm::vec2(10,20), 12.0f, textWidth, { 1,1,1,1 });
			GUI::DrawBoxUI(screenPosition, { (lineBounds.x+3) * 9.f, (lineBounds.y * 24) + 5}, { 0,0,0,0.5 }, 0, 2.25, color);
			GUI::DrawBoxUI(screenPosition - glm::vec2(5,5), { 10,10 }, color, 5);
		}
	}

	inline nlohmann::json Save()
	{
		nlohmann::json data;
		data["text"] = text.c_str();
		data["textWidth"] = textWidth;
		data["color"] = File::JsonVec4(color);
		return data;
	}

	inline void Load(nlohmann::json data)
	{
		if (data.contains("text")) text = data["text"].get<std::string>();
		if (data.contains("textWidth")) textWidth = data["textWidth"].get<float>();
		if (data.contains("color")) color = File::Vec4Json(data["color"]);
	}

	inline void Editor()
	{
		Editor::String("Text", &text);
		Editor::Float("TextWidth", &textWidth);
		Editor::Color4("color", &color[0]);
	}

	static World::Entity* Construct() { return new Note(); }
};