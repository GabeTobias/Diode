#pragma once

#include <glm/glm.hpp>

#include "File.h"
#include "nlohmann.hpp"

namespace File 
{
	using namespace nlohmann;

	inline void SaveInt(std::string name, json* object, int data) 
	{
		(*object)[name] = data;
	}

	inline void LoadInt(std::string name, json object, int* data) 
	{
		if (object.contains(name)) *data = object[name].get<int>();
	}

	////////////////////////////////////////////////////////////////////

	inline void SaveFloat(std::string name, json* object, float data)
	{
		(*object)[name] = data;
	}

	inline void SaveFloat2(std::string name, json* object, glm::vec2 data)
	{
		(*object)[name] = File::JsonVec2(data);
	}

	inline void SaveFloat3(std::string name, json* object, glm::vec3 data)
	{
		(*object)[name] = File::JsonVec3(data);
	}


	inline void LoadFloat(std::string name, json object, float* data)
	{
		if (object.contains(name)) *data = object[name].get<float>();
	}

	inline void LoadFloat2(std::string name, json object, glm::vec2* data)
	{
		if (object.contains(name)) *data = File::Vec2Json(object[name]);
	}

	inline void LoadFloat3(std::string name, json object, glm::vec3* data)
	{
		if (object.contains(name)) *data = File::Vec3Json(object[name]);
	}

	////////////////////////////////////////////////////////////////////

	inline void SaveBool(std::string name, json* object, bool data)
	{
		(*object)[name] = data;
	}

	inline void LoadBool(std::string name, json object, bool* data)
	{
		if (object.contains(name)) *data = object[name].get<bool>();
	}

	////////////////////////////////////////////////////////////////////

	inline void SaveString(std::string name, json* object, std::string data)
	{
		(*object)[name] = data;
	}

	inline void LoadString(std::string name, json object, std::string* data)
	{
		if (object.contains(name)) *data = object[name].get<float>();
	}

}