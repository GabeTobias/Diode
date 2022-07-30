#include "Manip.h"

#include "Input.h"
#include "Coords.h"
#include "Renderer.h"
#include "Undo.h"

#include "Physics.h"

#include "Editor.h"

namespace Editor 
{
	bool dragStart = false;

	void Marquee() 
	{
		using namespace glm;
		using namespace World;

		vec2 mpos = Input::GetMouseWorldPosition();

		if (Input::GetMousebutton(0) && !dragStart && !manipDragging && !manipHovering && !TileEditing)
		{
			marqueeStartPos = mpos;
			dragStart = true;
		}

		if (!Input::GetMousebutton(0))
			dragStart = false;

		if (distance(marqueeStartPos, mpos) > 0.5f && dragStart)
			marqueeDragging = true;

		if (marqueeDragging)
		{
			vec2 size = {abs(marqueeStartPos.x - mpos.x), abs(marqueeStartPos.y - mpos.y) };
			vec2 offset = size / 2.f;

			if (mpos.x < marqueeStartPos.x) offset.x *= -1;
			if (mpos.y < marqueeStartPos.y) offset.y *= -1;

			Render::DrawQuad
			(
				vec3(marqueeStartPos + offset, 1),
				0, size,
				vec4(1,1,1,0.15)
			);

			if (!Input::GetMousebutton(0)) 
			{
				marqueeDragging = false;

				selectedEntt.clear();

				for (Entity* entt : entities) 
				{
					if (entt->className == "Room") continue;

					vec2 start = 
					{
						(mpos.x < marqueeStartPos.x) ? mpos.x : marqueeStartPos.x,
						(mpos.y < marqueeStartPos.y) ? mpos.y : marqueeStartPos.y
					};

					bool selected = BoxOverlaps(start, size, entt->position - (entt->editorBounds / 2.f), entt->editorBounds);

					if (selected)
						selectedEntt.push_back(entt);
				}
			}
		}
	}

	glm::vec2 Manipulator(int uid, glm::vec2 position)
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
			if (!manipDragging)
				manipObjStartPos[uid] = position;

			if ((hoveringX || hoveringY || hoveringXY) && Input::GetMousebutton(0) && !manipDragging)
			{
				//Begin Manipulation
				manipDragging = true;
				manipSelected = uid;
				manipStartPos = Input::GetMouseWorldPosition();
				manipObjStartPos[uid] = position;

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
			if (manipDragging && (manipSelected == uid || Input::GetKey(GLFW_KEY_LEFT_CONTROL)))
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
					if (manipAxis == 0 || manipAxis == 2) position.x = manipObjStartPos[uid].x + ((free) ? xOffset : round(xOffset));
					if (manipAxis == 1 || manipAxis == 2) position.y = manipObjStartPos[uid].y + ((free) ? yOffset : round(yOffset));
				}
			}
		}

		return position;
	}
}