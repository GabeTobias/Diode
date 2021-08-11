#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>

#include "World.h"
#include "State.h"
#include "Physics.h"

#include "Collidable.h"

#include "ImGui.h"
#include "Props.h"
#include "Manip.h"

#include "Time.h"
#include "GUI.h"

class Pannable: public World::Component
{
public:
	int pointCount = 2;
	std::vector<glm::vec2> points;
	std::vector<glm::vec2> pointsWorld;

	int currentPoint = 0;
	float panSpeed = 0.1;
	float panInterval = 2;
	float intervalOffset = 2;

	Time::Timer panTimer;
	glm::vec2 startPosition;

	inline Pannable() 
	{ 
		className = "Pannable"; 

		//Add Initial Points
		for (int i = 0; i < pointCount; i++)
			points.push_back(glm::vec2(1));
	}

	inline void Start() 
	{
		pointsWorld.clear();
		for (int i = 0; i < pointCount; i++)
		{
			pointsWorld.push_back(points[i] + owner->position);
		}

		startPosition = owner->position;
		
		panTimer.reset();
		panTimer.previousTime += intervalOffset;
	}

	inline void Update()
	{
		//Loop threw points
		if (panTimer.getInterval(panInterval)) 
		{
			currentPoint = (currentPoint + 1) % pointCount;
		}

		//Lerp to Point
		owner->position = glm::lerp(owner->position, pointsWorld[currentPoint], panSpeed);
	}

	inline void Editor()
	{
		Editor::Float("Movement Speed", &panSpeed);
		Editor::Float("Pan Interval", &panInterval);
		Editor::Float("Interval Offset", &intervalOffset);
		Editor::IntSlider("Point Count", &pointCount, 2, 8);
		if (ImGui::IsItemEdited())
		{
			points.resize(pointCount, glm::vec2(1));
		}

		for (int i = 0; i < pointCount; i++)
		{
			points[i] = Editor::Manipulator(uid + i + 1, points[i] + owner->position) - owner->position;
			
			glm::vec3 pos = glm::vec3(points[i] + owner->position, 0);
			GUI::DrawTextUI(std::to_string(i), worldToViewCoordTransform(pos) + glm::vec2(-15, 15), 15, glm::vec4(1,1,1,0.25));
		}
	}

	inline nlohmann::json Save()
	{
		nlohmann::json data;

		data["pointCount"] = pointCount;
		data["panSpeed"] = panSpeed;
		data["panInterval"] = panInterval;
		data["intervalOffset"] = intervalOffset;

		for (int i = 0; i < pointCount; i++)
		{
			data[("point" + std::to_string(i)).c_str()] = File::JsonVec2(points[i]);
		}

		return data;
	}

	inline void Load(nlohmann::json data)
	{
		if (data.contains("panSpeed")) panSpeed = data["panSpeed"].get<float>();
		if (data.contains("panInterval")) panInterval = data["panInterval"].get<float>();
		if (data.contains("intervalOffset")) intervalOffset = data["intervalOffset"].get<float>();
		if (data.contains("pointCount")) pointCount = data["pointCount"].get<int>();

		points.resize(pointCount, glm::vec2(1));
		for (int i = 0; i < pointCount; i++)
		{
			if (data.contains(("point" + std::to_string(i)).c_str())) points[i] = File::Vec2Json(data[("point" + std::to_string(i)).c_str()]);
		}
	}


	inline static World::Component* Construct(World::Entity* ent)
	{
		return new Pannable();
	}
};