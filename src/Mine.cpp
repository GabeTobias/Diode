#include "Mine.h"

#include "imgui.h"
#include "Props.h"

#include "Game.h"
#include "State.h"
#include "Renderer.h"

#include "Bullet.h"

#include "Collidable.h"
#include "Destructable.h"

using namespace glm;

Mine::Mine()
{
	className = "Mine";
	Tag("Obstacle");

	collider = new Collidable(vec2(1.25));
	AttachComponent(collider);

	destruct = new Destructable();
	AttachComponent(destruct);
}

void Mine::Update()
{
	float dist = distance(Game::player->position, position);
	
	if (dist < range && hitTimer.getInterval(0.5))
	{
		hits++;
	}

	//Destroy without exploding
	if (hits > maxHits)
		World::DeleteEntity(this);
}

void Mine::Draw()
{
	float opacity = (active) ? 1.0f : 0.5f;

	Render::DrawQuad(vec3(position, 12), 0, vec2(0.6), vec4(1,0.5,0, opacity));
	Render::DrawQuad(vec3(position, 12), 3.14/4.0, vec2(0.6), vec4(1,0.5,0, opacity));

	if (GameState::Mode == EditMode) 
		Render::DrawCircle(vec3(position, 2), vec2(range*2), vec4(1,1,1,0.15), true, 0.01f);
}

void Mine::Editor()
{
	Editor::Int("Hits", &hits);
	Editor::Int("Max Hits", &maxHits);
	Editor::Float("Range", &range);
}

void Mine::Destroy()
{
	//TODO: Maybe this should depend on whos bullet destroys it?
	//Explode
	for (int i = 0; i < 8; i++)
	{
		float ang = i * (3.14 / 4.0);
		vec2 dir = normalize(vec2(sin(ang), cos(ang)));

		Bullet* b = new Bullet(position + (dir * 1.2f), dir * 0.3f);
		World::AddEntity(b);
	}
}

nlohmann::json Mine::Save()
{
	nlohmann::json data;

	data["hits"] = hits;
	data["maxHits"] = maxHits;
	data["range"] = range;

	return data;
}

void Mine::Load(nlohmann::json data)
{
	if(data.contains("hits")) hits = data["hits"].get<int>();
	if(data.contains("maxHits")) maxHits = data["maxHits"].get<int>();
	if(data.contains("range")) range = data["range"].get<float>();
}
