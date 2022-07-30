#include "Room.h"
#include "Input.h"
#include "Editor.h"

#include "Game.h"
#include "File.h"
#include "Log.h"

#include "Player.h"
#include "Coords.h"

using namespace glm;

Room::Room(glm::vec2 position, glm::vec2 size)
{
	this->position = position;
	this->className = "Room";

	Tag("Roomless");

	tilemap = new Tilemap(size);
	AttachComponent(tilemap);
}

void Room::Start()
{
	StoreState();
}

void Room::Update()
{
	if (PointOverlaps(position - (vec2(tilemap->width * TILESCALE, tilemap->height * TILESCALE) / 2.f), vec2(tilemap->width * TILESCALE, tilemap->height * TILESCALE), Game::player->position))
	{
		vec2 offsetPosition = position - (vec2(tilemap->width * TILESCALE, tilemap->height * TILESCALE) / 2.f) + vec2(0,0.5);

		Game::player->target = clamp(
			Game::player->position, offsetPosition + vec2(24),
			offsetPosition + vec2(tilemap->width * TILESCALE, tilemap->height * TILESCALE) - vec2(24)
		);

		//Set Entts inside room to active On Enter
		if (!roomActive) 
			SetEnttActive(true);
	} 
	else 
	{
		//Reload On Exit
		if (roomActive)
		{
			ReloadState();
			SetEnttActive(false);
		}
	}
	
	roomActive = PointOverlaps(position - (vec2(tilemap->width * TILESCALE, tilemap->height * TILESCALE) / 2.f), vec2(tilemap->width * TILESCALE, tilemap->height * TILESCALE), Game::player->position);
}

void Room::Draw(){}

void Room::Editor(){}

nlohmann::json Room::Save()
{
	nlohmann::json data;

	data["width"] = tilemap->width;
	data["height"] = tilemap->height;

	data["tileData"] = nlohmann::json::array();
	for (int i = 0; i < tilemap->width * tilemap->height; i++)
	{
		int x0 = i % tilemap->width;
		int y0 = i / tilemap->width;

		data["tileData"][i] = tilemap->data[x0][y0];
	}

	return data;
}

void Room::Load(nlohmann::json data)
{
	DetachComponent(tilemap);
	World::DeleteComponent(tilemap);

	int w = data["width"].get<int>();
	int h = data["height"].get<int>();

	tilemap = new Tilemap(vec2(w,h));
	AttachComponent(tilemap);

	//Update Data
	for (int i = 0; i < tilemap->width * tilemap->height; i++)
	{
		int x0 = i % tilemap->width;
		int y0 = i / tilemap->width;

		tilemap->data[x0][y0] = data["tileData"][i];
	}

	tilemap->GenGeo();
}

bool Room::Contains(glm::vec2 pos)
{
	return PointOverlaps
	(
		position - (vec2(tilemap->width * TILESCALE, tilemap->height * TILESCALE) / 2.f), 
		vec2(tilemap->width * TILESCALE, tilemap->height * TILESCALE), 
		pos
	);
}

glm::vec2 Room::toLocalSpace(glm::vec2 point)
{
	glm::vec2 relPos = (point - position - tilemap->position);
	return { int(relPos.x / TILESCALE), int(relPos.y / TILESCALE) };
}

glm::vec2 Room::toWorldSpace(glm::vec2 point)
{
	return (point * TILESCALE) + position + tilemap->position;
}

void Room::StoreState()
{
	//Loop threw all entities
	for (World::Entity* e : World::entities)
	{
		if (e->hasTag("Roomless")) continue;

		if (Contains(e->position)) 
		{
			state.push_back(File::SerializeEntt(e));
			e->active = false;
		}
	}
}

void Room::ReloadState()
{
	//Clear all Contained entities
	for (World::Entity* e : World::entities)
	{
		if (e->hasTag("Roomless")) continue;

		if (Contains(e->position)) 
			World::DeleteEntity(e, false);
	}

	//Loop threw all entt data in state
	for (nlohmann::json data : state)
	{	
		//Search for entt
		World::Entity* ent = World::GetEntityOfUID(data["uid"].get<unsigned int>());

		//Update OR Add Entt to scene
		if(ent != nullptr) File::DeserializeEntt(ent, data);
		else World::AddEntity(File::DeserializeEntt(nullptr, data));
	}
}

void Room::SetEnttActive(bool state)
{
	//Clear all Contained entities
	for (World::Entity* e : World::entities)
	{
		if (e->hasTag("Roomless")) continue;

		if (Contains(e->position))
			e->active = state;
	}
}

Room* Room::getRoom(glm::vec2 position)
{
	for (World::Entity* e : World::entities)
	{
		if (e->className != "Room") continue;

		Room* r = (Room*)e;
		if (r->Contains(position)) return r;
	}
}
