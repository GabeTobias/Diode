#pragma once
#include <glm/glm.hpp>

#include "World.h"
#include "Audio.h"

class Listenable : public World::Component
{
public:
	inline Listenable()
	{
		className = "Listenable";
	}

	inline void Update() 
	{
		Audio::SetListener(owner->position);
	}
};