#pragma once

#include <glm/glm.hpp>
#include <map>

namespace Editor 
{
	//Manipulator Properties
	inline int manipAxis = -1;
	inline int manipSelected = -1;
	inline bool manipDragging = false;
	inline bool manipHovering = false;
	inline glm::vec2 manipStartPos = glm::vec2(0);
	inline std::map<int, glm::vec2> manipObjStartPos;
	
	inline bool marqueeDragging = false;
	inline glm::vec2 marqueeStartPos = glm::vec2(0);

	glm::vec2 Manipulator(int uid, glm::vec2 position);
	void Marquee();
}