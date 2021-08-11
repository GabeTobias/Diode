#pragma once

#include <glm/glm.hpp>

#include "Input.h"
#include "Coords.h"
#include "Renderer.h"
#include "Undo.h"

namespace Editor 
{
	//Manipulator Properties
	inline int manipAxis = -1;
	inline int manipSelected = -1;
	inline bool manipDragging = false;
	inline bool manipHovering = false;
	inline glm::vec2 manipStartPos = glm::vec2(0);
	inline glm::vec2 manipObjStartPos = glm::vec2(0);

	inline glm::vec2 Manipulator(int uid, glm::vec2 position)
	{
		using namespace glm;

		//Calculate Zoom Manip Scaling
		float manipScale = camera->size / 20;

		//Calculate Manip overlap
		bool hoveringX = PointOverlaps(position + vec2(2 * manipScale, 0) - vec2(0.25 * manipScale), vec2(0.5) * manipScale, Input::GetMouseWorldPosition());
		bool hoveringY = PointOverlaps(position + vec2(0, 2 * manipScale) - vec2(0.25 * manipScale), vec2(0.5) * manipScale, Input::GetMouseWorldPosition());
		bool hoveringXY = PointOverlaps(position + vec2(0.5 * manipScale), vec2(0.5) * manipScale, Input::GetMouseWorldPosition());

		//Draw Manipulator
		{
			//Draw Handle Rods
			Render::DrawQuad(vec3(position + vec2(1 * manipScale, 0), 1), 0, vec2(2, 0.1f) * manipScale, vec4(1, 0, 0, (hoveringX) ? 1.0 : 0.75));
			Render::DrawQuad(vec3(position + vec2(0, 1 * manipScale), 1), 0, vec2(0.1f, 2) * manipScale, vec4(0, 1, 0, (hoveringY) ? 1.0 : 0.75));

			//Draw Arrow Handles
			Render::DrawTri(vec3(position + vec2(2 * manipScale, 0), 1), -3.14f / 2.0, vec2(0.5) * manipScale, vec4(1, 0, 0, (hoveringX) ? 1.0 : 0.75));
			Render::DrawTri(vec3(position + vec2(0, 2 * manipScale), 1), 0, vec2(0.5) * manipScale, vec4(0, 1, 0, (hoveringY) ? 1.0 : 0.75));

			//Draw Free Manip Handle
			Render::DrawQuad(vec3(position + vec2(0.75 * manipScale), 1), 0, vec2(0.5) * manipScale, vec4(1, 1, 0, (hoveringXY) ? 1.0 : 0.75));
		}

		//Hover UI Flag
		{
			manipHovering |= (hoveringX || hoveringY || hoveringXY);
		}

		//Begin Dragging
		{
			if ((hoveringX || hoveringY || hoveringXY) && Input::GetMousebutton(0) && !manipDragging)
			{
				//Begin Manipulation
				manipDragging = true;
				manipSelected = uid;
				manipStartPos = Input::GetMouseWorldPosition();
				manipObjStartPos = position;

				//Store Manipulation Axis
				if (hoveringX)  manipAxis = 0;
				if (hoveringY)  manipAxis = 1;
				if (hoveringXY) manipAxis = 2;
			}

			//Check for Manipulation End
			if (manipDragging && !Input::GetMousebutton(0))
			{
				//Reset Manipulation
				manipDragging = false;
				manipSelected = -1;

				//TODO: this shouldn't store the entire scene on object move
				//Store Action
				Undo::Store();
			}
		}

		//Drag Object
		{
			//THIS IS THE ONLY REASON WHY I AM ALLOWED TO USE MULTIPLE MANIPULATORS
			//EVERY MANIPULATOR MUST HAVE A UNIQUE ID IN ORDER FOR THIS TO WORK

			//Check if you are selected manipulator
			if (manipDragging && manipSelected == uid)
			{
				//Calculate Change in axis
				float xOffset = Input::GetMouseWorldPosition().x - manipStartPos.x;
				float yOffset = Input::GetMouseWorldPosition().y - manipStartPos.y;

				//Check if for snap
				bool free = Input::GetKey(GLFW_KEY_LEFT_SHIFT);

				//SNAP
				if (round(xOffset) != xOffset && round(yOffset) != yOffset || free)
				{
					//Move position based on change in axis & snap
					if (manipAxis == 0 || manipAxis == 2) position.x = manipObjStartPos.x + ((free) ? xOffset : round(xOffset));
					if (manipAxis == 1 || manipAxis == 2) position.y = manipObjStartPos.y + ((free) ? yOffset : round(yOffset));
				}
			}
		}

		return position;
	}
}