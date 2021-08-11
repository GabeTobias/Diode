#pragma once

#include <glm/glm.hpp>

#include "Time.h"
#include "World.h"

class Shocked : public World::Component
{
public:
	float duration = 5;
	Time::Timer shockTimer;

	inline Shocked(float duration) 
	{ 
		className = "Shocked";
		this->duration = duration;
		shockTimer.reset();
	}

	inline void Update()
	{
		if (shockTimer.getInterval(duration)) 
		{
			owner->active = true;
			World::DeleteComponent(this);
		}
		else
		{
			owner->active = false;
		}
	}
};