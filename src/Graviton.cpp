#include "Graviton.h"

#include "Movable.h"

#include "Renderer.h"

#include "Props.h"
#include "File.h"

#include "Coords.h"

using namespace glm;

Graviton::Graviton(glm::vec2 pos)
{
	className = "Graviton";
	Tag("Obstacle");

	this->position = pos;
}

void Graviton::Awake()
{
	editorBounds = bounds;
}

void Graviton::Update()
{
	std::vector<World::Component*> movables = World::GetComponentsOfClass("Movable");
	for (World::Component* comp : movables) 
	{
		//Check if colliding with movable
		if (PointOverlaps(position - (bounds / 2.f), bounds, comp->owner->position))
		{
			//Set its passive velocity
			Movable* mov = (Movable*)comp;
			if (mov) mov->passive = lerp(mov->passive, gravity, 0.1f);
		}
	}
}

void Graviton::Draw()
{
	Render::DrawQuad(vec3(position, 11), 0, bounds, vec4(0,1,0.2,0.3));
}

void Graviton::Editor()
{
	Editor::Float2("Bounds", &bounds[0]);
	Editor::Float2("Gravity", &gravity[0]);

	editorBounds = bounds;
}

nlohmann::json Graviton::Save()
{
	nlohmann::json data;

	data["bounds"] = File::JsonVec2(bounds);
	data["gravity"] = File::JsonVec2(gravity);

	return data;
}

void Graviton::Load(nlohmann::json data)
{
	if (data.contains("bounds")) bounds = File::Vec2Json(data["bounds"]);
	if (data.contains("gravity")) gravity = File::Vec2Json(data["gravity"]);
}

