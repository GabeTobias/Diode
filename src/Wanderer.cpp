#include "Wanderer.h"
#include "Pathfinding.h"

#include "Color.h"

#include "Collidable.h"
#include "Movable.h"
#include "Room.h"

using namespace glm;

Wanderer::Wanderer(glm::vec2 pos)
{
	className = "Wanderer";
	Tag("Enemy");

	position = pos;

	collider = new Collidable(vec2(0.5));
	AttachComponent(collider);

	rigidbody = new Movable(collider);
	AttachComponent(rigidbody);
}

void Wanderer::Start(){}

void Wanderer::Update()
{
	//Find path to target
	room = Room::getRoom(position);
	path = Pathfinding::FindPath(room->tilemap->data, room->toLocalSpace(position), vec2(12,12), room->tilemap->width, room->tilemap->height, 0);
}

void Wanderer::Draw()
{
	//Draw path to target
	for (int i = 0; i < path.size(); i++) 
	{
		Render::DrawQuad(vec3(room->toWorldSpace(path[i]) + vec2(0.5), 11), 0, vec2(1), vec4(HSV(vec3(i*(360.f/path.size()), 100, 100)), 1));
	}
		
	//Current position
	Render::DrawTri(vec3(position, 11), 0, vec2(1), vec4(1,0,0,1));
}

void Wanderer::Editor()
{
	//Find path to target
	room = Room::getRoom(position);
	path = Pathfinding::FindPath(room->tilemap->data, room->toLocalSpace(position), vec2(12, 12), room->tilemap->width, room->tilemap->height, 0);

	//Display path node position
	if (path.size() > 0 && ImGui::TreeNode("Path"))
	{
		for (int i = 0; i < path.size(); i++)
		{
			Editor::Float2(("Node: " + std::to_string(i)).c_str(), &path[i][0]);
		}

		ImGui::TreePop();
	}
}

nlohmann::json Wanderer::Save()
{
	return nlohmann::json();
}

void Wanderer::Load(nlohmann::json data){}
