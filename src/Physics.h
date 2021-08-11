#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "World.h"
#include "Tilemap.h"
#include "Collidable.h"

namespace Physics
{
	struct CollisionInfo
	{
		glm::vec2 position;
		glm::vec2 bounds;
		World::Component* component = nullptr;
		World::Entity* entt = nullptr;
		explicit operator bool() { return component != nullptr; }
	};

	inline CollisionInfo Collides(glm::vec2 position, glm::vec2 bounds, unsigned int uid = 0, int layer = -1)
	{
		using namespace World;

		//Checks Collision Component
		for (Component* comp : components)  
		{
			if (!comp->active) continue;

			//Collider Component
			if (comp->className == "Collidable") 
			{
				Collidable* collide = (Collidable*)comp;

				if (collide->layer > layer) continue;
				if (collide->bounds == glm::vec2(0)) continue;

				if (BoxOverlaps(position, bounds, (collide->owner->position + collide->position) - (collide->bounds / 2.f), collide->bounds))
				{
					if (uid != collide->uid)
						return { collide->owner->position, collide->bounds, comp, comp->owner };
				}
			}

			//Tilemap Component
			if (comp->className == "Tilemap") 
			{
				Tilemap* map = (Tilemap*)comp;

				//Loop threw tiles
				for (int x0 = 0; x0 < map->width; x0++)
				{
					for (int y0 = 0; y0 < map->height; y0++)
					{
						if (map->data[x0][y0] == 0) continue;

						if (BoxOverlaps(position, bounds, map->position + map->owner->position + glm::vec2(x0*TILESCALE, y0*TILESCALE) - glm::vec2(TILESCALE/4.f), glm::vec2(TILESCALE)))
							return { map->position + map->owner->position + glm::vec2(x0 * TILESCALE, y0 * TILESCALE), glm::vec2(TILESCALE), comp, comp->owner };
					}
				}
			}

		}

		return { glm::vec2(NAN, NAN), glm::vec2(NAN, NAN), nullptr};
	}

	// RAYCAST ONLY RETURNS THE FIRST POINT IT COLLIDES WITH
	// WHILE RAYTRACES RETURN ALL COLLISIONS ALONG A GIVEN PATH

	inline CollisionInfo Raycast(glm::vec2 start, glm::vec2 stop, float iter, int uid = 0, int layer = 100)
	{
		using namespace glm;

		float dist = distance(start, stop);
		vec2 direction = normalize(stop - start);

		//Iterate over positions along ray
		for (float i = 0; i < iter; i++)
		{
			//Calculate ray position
			float percent = (i / iter);
			float offset = dist * percent;
			vec2 position = start + (direction * offset);

			//Collsion Check
			CollisionInfo info = Collides(position, vec2(0.1), uid, layer);
			if (info) return { position, info.bounds, info.component, info.component->owner };
		}

		return { glm::vec2(NAN, NAN), glm::vec2(NAN, NAN), nullptr, nullptr };
	}


	inline std::vector<CollisionInfo> RayTrace(glm::vec2 start, glm::vec2 stop, int iter, int uid = 0, int layer = 100)
	{
		using namespace glm;

		std::vector<CollisionInfo> hits;

		float dist = distance(start, stop);
		vec2 direction = normalize(stop - start);

		//Iterate over positions along ray
		for (float i = 0; i < iter; i++)
		{
			//Calculate Position
			float percent = (i / iter);
			float offset = dist * percent;
			vec2 position = start + (direction * offset);

			//Collision check
			CollisionInfo info = Collides(position, vec2(0.1), uid, layer);
			if (info) hits.push_back( { position, info.bounds, info.component, info.component->owner });
		}

		return hits;
	}

}