#pragma once

#include <iomanip>
#include <fstream>
#include <string>
#include <map>

#include "nlohmann.hpp"
#include "World.h"

typedef World::Entity* (*ConstructEntt)(void); // function pointer type
typedef World::Component* (*ConstructComp)(World::Entity*); // function pointer type

namespace File 
{
	using nlohmann::json;

	inline std::map<std::string, ConstructEntt> EnttTable;
	inline std::map<std::string, ConstructComp> CompTable;

	//Map Component from function
	inline void MapConst(std::string name, ConstructEntt con)
	{
		EnttTable.emplace(name, con);
	}

	//Map Component from function
	inline void MapConst(std::string name, ConstructComp con)
	{
		CompTable.emplace(name, con);
	}

	inline void SaveJson(json data, std::string src)
	{
		//write prettified JSON to another file
		std::ofstream o(src);
		o << std::setw(4) << data << std::endl;
	}

	inline json LoadJson(std::string src)
	{
		// read a JSON file
		std::ifstream i(src.c_str());
		json j;
		i >> j;

		//Return Data
		return j;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	inline std::vector<float> JsonVec4(glm::vec4 v)
	{
		return { v[0],v[1],v[2],v[3] };
	}

	inline std::vector<float> JsonVec3(glm::vec3 v)
	{
		return { v[0],v[1],v[2] };
	}

	inline std::vector<float> JsonVec2(glm::vec2 v)
	{
		return { v[0],v[1] };
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	inline glm::vec4 Vec4Json(nlohmann::json val)
	{
		//JSON Namespace
		using nlohmann::json;

		std::vector<float> v = val.get < std::vector < float >>();
		return { v[0], v[1], v[2], v[3] };
	}

	inline glm::vec3 Vec3Json(nlohmann::json val)
	{
		//JSON Namespace
		using nlohmann::json;

		std::vector<float> v = val.get < std::vector < float >>();
		return { v[0], v[1], v[2] };
	}

	inline glm::vec2 Vec2Json(nlohmann::json val)
	{
		//JSON Namespace
		using nlohmann::json;

		std::vector<float> v = val.get < std::vector < float >>();
		return { v[0], v[1] };
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	inline nlohmann::json SerializeEntt(World::Entity* ent) 
	{
		using namespace World;

		nlohmann::json actor;

		//Bind Base Properties
		actor["uid"] = ent->uid;
		actor["className"] = ent->className.c_str();
		actor["name"] = ent->name.c_str();
		actor["active"] = ent->active;
		actor["visible"] = ent->visible;
		actor["position"] = JsonVec2(ent->position);

		//Get Actor Defined Data
		actor["data"] = ent->Save();

		//Save Components
		actor["comps"] = json::array();
		int compCount = 0;
		for (Component* comp : components)
		{
			//Skip components not attached
			if (comp->owner->uid != ent->uid) continue;
			if (CompTable.find(comp->className) == CompTable.end()) continue;

			json compData = json::object();
			compData["className"] = comp->className;
			compData["active"] = comp->active;
			compData["visible"] = comp->visible;
			compData["data"] = comp->Save();

			//Get Save Data
			actor["comps"][compCount] = compData;

			compCount++;
		}

		return actor;
	}

	inline World::Entity* DeserializeEntt(World::Entity* ent, nlohmann::json data)
	{
		using namespace World;

		if (ent == nullptr) 
		{
			//Search for Construct function
			std::map<std::string, ConstructEntt>::iterator find = EnttTable.find(data["className"].get<std::string>());
			if (find == EnttTable.end()) return nullptr;

			//Create Actor Object
			ent = find->second();
		}

		//Load Base Data
		ent->uid = data["uid"].get<unsigned int>();
		ent->className = data["className"].get<std::string>();
		ent->name = data["name"].get<std::string>();
		ent->active = data["active"].get<bool>();
		ent->visible = data["visible"].get<bool>();
		ent->position = Vec2Json(data["position"]);

		//Pass Data to Actor Loader
		ent->Load(data["data"]);

		for (json comp : data["comps"])
		{
			if (comp.is_null()) continue;

			World::Component* compon = ent->GetComponent(comp["className"]);
			if (compon == nullptr) 
			{
				compon = CompTable[comp["className"]](ent);
				ent->AttachComponent(compon);
			}

			compon->active = comp["active"].get<bool>();
			compon->visible = comp["visible"].get<bool>();
			compon->Load(comp["data"]);
		}

		return ent;
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	inline nlohmann::json SerializeScene() 
	{
		using namespace World;

		nlohmann::json fileData;
		nlohmann::json actors = nlohmann::json::array();

		for (Entity* ent : entities)
		{
			//Add Serialized Actor to data
			actors.push_back(SerializeEntt(ent));
		}

		//Pass Actors
		fileData["Entities"] = actors;

		return fileData;
	}

	inline void DeserializeScene(nlohmann::json j) 
	{
		//Clear Scene
		World::ClearEntities();
		World::ClearComponents();

		for (json data : j["Entities"])
		{
			World::AddEntity(DeserializeEntt(nullptr, data));
		}
	}

	inline void SaveScene(std::string src) 
	{
		//Convert scene to json
		nlohmann::json fileData = SerializeScene();

		//Save to file
		SaveJson(fileData, src);
	}

	inline void LoadScene(std::string src) 
	{
		// read a JSON file
		json j = LoadJson(src);

		//Create Scene from json
		DeserializeScene(j);
	}
}