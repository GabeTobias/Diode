#pragma once

#include <map>
#include <vector>
#include <string>

#include <glm/glm.hpp>

#include "Coords.h"
#include "Random.h"
#include "Time.h"

#include "nlohmann.hpp"

namespace World
{
	class Component;
	class Entity;

	inline std::vector<Entity*> entities;
	inline std::vector<Component*> components;

	inline unsigned int GenerateUID() { return RANDOM::UID(); }

	class Component
	{
	public:
		//Unique Actor Identifier
		unsigned int uid = -1;

		//Unique Serializable Class Name
		std::string className;

		//Pointer to attached actor
		Entity* owner;

		//Update Flag
		bool active = true;

		//Render Flag
		bool visible = true;

	public:
		virtual void Awake() {};			//Fires On Attach
		virtual void Start() {};			//Fire On Scene Start

		virtual void Destroy() {};
		virtual void Delete() {};

		virtual void Draw() {};
		virtual void Update() {};

		virtual void Editor() {};

		virtual void GUI() {};

		virtual nlohmann::json Save() { return NULL; };
		virtual void Load(nlohmann::json data) {};
	};
	class Entity
	{
	public:
		//Unique Actor Identifier
		unsigned int uid = -1;

		//Unique Serializable Class Name
		std::string className;

		//Semi-Unique Object Name
		std::string name = "";

		//WorldSpace Positition
		glm::vec2 position = glm::vec2(0);

		//Editor Bounds
		glm::vec2 editorBounds = glm::vec2(1);

		//Update Flag
		bool active = true;

		//Render Flag
		bool visible = true;

		//Tag System
		std::vector<std::string> tags;

	public:
		Component* AttachComponent(Component* c) { c->uid = GenerateUID(); c->owner = this; components.push_back(c); c->Awake(); return c; }
		void DetachComponent(Component* c) { c->owner = nullptr; }

		Component* GetComponent(std::string className)
		{			
			for (Component* comp: components) 
			{
				if (comp->owner->uid != uid) continue;

				if (comp->className == className) 
				{
					return comp;
				}
			}

			return nullptr;
		}

		void Tag(std::string str) { if(!hasTag(str)) tags.push_back(str); }
		void RemoveTag(std::string str) 
		{
			for (int i = 0; i < tags.size(); i++)
			{
				if (str == tags[i])
					tags.erase(tags.begin()+i);
			}
		}

		bool hasTag(std::string str) 
		{
			for (std::string tag : tags)
				if (str == tag) return true;

			return false;
		}

	public:
		virtual void Awake() {};			//Fires On AddActor

		virtual void Start() {};			//Fires On Scene Start

		virtual void Destroy() {};
		virtual void Delete() {};

		virtual void Draw() {};
		virtual void Update() {};

		virtual void Editor() {};
		
		virtual void GUI() {};
		
		virtual nlohmann::json Save() { return NULL; };
		virtual void Load(nlohmann::json data) {};
	};


	inline void AddEntity(Entity* entity)
	{
		if(entity->uid == -1)
			entity->uid = GenerateUID();
		
		entity->name = entity->className + " " + std::to_string(entities.size());
		entities.push_back(entity);
		entity->Awake();
	}

	inline Entity* GetEntityOfUID(unsigned int uid)
	{
		for (int i = entities.size() - 1; i >= 0; i--)
		{
			if (entities[i]->uid == uid)
			{
				return entities[i];
			}
		}

		return nullptr;
	}

	inline Entity* GetEntityOfClass(std::string type) 
	{
		for (int i = entities.size() - 1; i >= 0; i--)
		{
			if (entities[i]->className == type)
			{
				return entities[i];
			}
		}
	}

	inline std::vector<Entity*> GetEntitiesOfClass(std::string type)
	{
		std::vector<Entity*> ents;

		for (int i = entities.size() - 1; i >= 0; i--)
		{
			if (entities[i]->className == type)
			{
				ents.push_back(entities[i]);
			}
		}

		return ents;
	}

	inline std::vector<Component*> GetComponentsOfClass(std::string type)
	{
		std::vector<Component*> comps;

		for (int i = components.size() - 1; i >= 0; i--)
		{
			if (components[i]->className == type) 
			{
				comps.push_back(components[i]);
			}
		}

		return comps;
	}

	//As a rule of thumb component should never be trying to destroy itself
	//The owing actor should be responsable for crreating and destroying 
	//all of it owned components. Components are only stored globally to 
	//try to reduce code complexity.
	inline void DeleteComponent(Component* comp)
	{
		comp->owner->DetachComponent(comp);

		for (int i = components.size() - 1; i >= 0; i--)
		{
			if (components[i]->uid == comp->uid)
			{
				components[i]->Destroy();
				components.erase(components.begin() + i);
			}
		}
	}


	inline void DeleteEntity(Entity* entity, bool destroy = true)
	{
		//Entities
		for (int i = entities.size() - 1; i >= 0; i--)
		{
			if (entities[i]->uid == entity->uid)
			{
				if(destroy) 
					entities[i]->Destroy();
				else
					entities[i]->Delete();

				entities.erase(entities.begin() + i);
			}
		}

		//Components
		for (int i = components.size() - 1; i >= 0; i--)
		{
			if (components[i] == nullptr) continue;

			if (components[i]->owner->uid == entity->uid)
			{
				if(destroy) 
					components[i]->Destroy();
				else
					components[i]->Delete();

				components.erase(components.begin() + i);
			}
		}
	}

	inline void ClearEntities() 
	{
		for (int i = 0; i < entities.size(); i++) 
		{
			Entity* e = entities[i];
			e->Delete();
			delete e;
		}

		entities.clear();
	}

	inline void ClearComponents() 
	{
		for (int i = 0; i < components.size(); i++)
		{
			Component* c = components[i];
			c->Delete();
			delete c;
		}

		components.clear();
	}

	inline void Update()
	{
		//Entities
		for (int i = entities.size() - 1; i >= 0; i--)
		{
			if (entities[i]->active) entities[i]->Update();		
			if (i >= entities.size()) i = entities.size()-1;
		}

		//Components
		for (int i = components.size() - 1; i >= 0; i--) 
		{
			if (components[i] && components[i]->active) components[i]->Update();
			if (i >= components.size()) i = components.size() - 1;
		}
	}

	inline void Draw()
	{
		//Entities
		for (int i = entities.size()-1; i >= 0; i--)
			if (entities[i] && entities[i]->visible) entities[i]->Draw();

		//Components
		for (int i = components.size()-1; i >= 0; i--)
			if (components[i] && components[i]->visible) components[i]->Draw();
	}

	inline void GUI()
	{
		//Entities
		for (int i = entities.size() - 1; i >= 0; i--)
			if (entities[i] && entities[i]->visible) entities[i]->GUI();

		//Components
		for (int i = components.size() - 1; i >= 0; i--)
			if (components[i] && components[i]->visible) components[i]->GUI();
	}

	inline void Start()
	{
		//Entities
		for (int i = entities.size() - 1; i >= 0; i--)
			if (entities[i] && entities[i]->active) entities[i]->Start();

		//Components
		for (int i = components.size() - 1; i >= 0; i--)
			if (components[i] && components[i]->active) components[i]->Start();
	}

}