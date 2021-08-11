#pragma once

#include <cstdlib>
#include <glm/glm.hpp>

namespace RANDOM
{
	using namespace glm;

	// Base Random
	inline float Float(float min = 0, float max = 1)
	{
		return min + (((float)rand() / (RAND_MAX)) * (max - min));
	}

	// Int Random
	inline int Int(int min = 0, int max = RAND_MAX) { return Float(min, max); }

	// Unsigned Int Random
	inline int UID() { return rand(); };

	/*	Vec2 Randoms */
	inline vec2 Vec2() { return { Float(), Float() }; }
	inline vec2 Vec2(vec2 min, vec2 max) { return { Float(min.x, max.x), Float(min.y, max.y) }; }
	inline vec2 Vec2Linear(vec2 min, vec2 max) { return min + (vec2(max - min) * Float()); }

	/*	Vec3 Randoms */
	inline vec3 Vec3() { return { Float(), Float(), Float() }; }
	inline vec3 Vec3(vec3 min, vec3 max) { return { Float(min.x, max.x), Float(min.y, max.y), Float(min.z, max.z) }; }
	inline vec3 Vec3Linear(vec3 min, vec3 max) { return min + (vec3(max - min) * Float()); }

	/*	Vec4 Randoms */
	inline vec4 Vec4() { return { Float(), Float(), Float(), Float() }; }
	inline vec4 Vec4(vec4 min, vec4 max) { return { Float(min.x, max.x), Float(min.y, max.y), Float(min.y, max.y), Float(min.y, max.y) }; }
	inline vec4 Vec4Linear(vec4 min, vec4 max) { return min + (vec4(max - min) * Float()); }
}