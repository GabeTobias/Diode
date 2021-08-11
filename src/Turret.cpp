#include "Turret.h"

#include "File.h"
#include "State.h"

#include "imgui.h"
#include "Manip.h"
#include "Props.h"

#include "Renderer.h"
#include "Game.h"

#include "Bullet.h"
#include "Missile.h"
#include "Collidable.h"
#include "Destructable.h"

using namespace glm;

Turret::Turret()
{
	className = "Turret";
	Tag("Enemy");

	collider = new Collidable(vec2(3));
	AttachComponent(collider);

	destruct = new Destructable();
	AttachComponent(destruct);

	offsetA = position + vec2(-4, 0);
	offsetB = position + vec2( 4, 0);
}

void Turret::Awake() 
{
	root = position;
}

void Turret::Update()
{
	//Shooting
	if(Game::player != nullptr)
	{
		float dist = distance(Game::player->position, root);
		vec2 direction = normalize(Game::player->position - root);

		if (dist < viewDistance && bulletTimer.getInterval(panInterval))
		{
			Entity* b;
			
			if(bulletType == 0) b = new Bullet(root + direction * 3.75f, direction * 0.3f);
			if(bulletType == 1) b = new Missile(root + direction * 3.75f, direction * 0.3f);

			World::AddEntity(b);
		}
	}

	//Panning
	if(panning)
	{
		//Move position
		if (panTarget)	root = lerp(root, position + offsetA, panSpeed);
		else			root = lerp(root, position + offsetB, panSpeed);

		//Switch Position
		if (panTimer.getInterval(2)) panTarget = !panTarget;

		//Update Collider
		collider->position = root - position;
	}
}

void Turret::Draw()
{
	float opacity = (active) ? 1.0f : 0.5f;
	vec2 direction = normalize(Game::player->position - root);

	if(active) barrelDir = lerp(barrelDir, direction, 0.2f);
	if(active) rotation = glm::lerp(rotation, glm::quatLookAt(vec3(direction, 0), vec3(0, 0, 1)), 0.2f);

	Render::DrawTri(vec3(root + (barrelDir *0.8f), 12), eulerAngles(rotation).z, vec2(1.25f,1.5f), vec4(0.5,0,1, opacity));
	Render::DrawCircle(vec3(root, 12), vec2(1.5), vec4(0.5,0,1, opacity));

	if (GameState::Mode == EditMode) 
	{
		if (panning) 
		{
			//Draw target positions of panning behavior
			Render::DrawQuad(vec3(position + offsetA, 2), 3.14 / 4.0, vec2(0.5), vec4(0.1, 0.1, 1, 0.5));
			Render::DrawQuad(vec3(position + offsetB, 2), 3.14 / 4.0, vec2(0.5), vec4(0.1, 0.1, 1, 0.5));
			Render::DrawQuad(vec3(position, 2), 3.14 / 4.0, vec2(0.75), vec4(0.25, 0.1, 1, 0.5));
		}

		//Update Root Position
		root = position;

		//Update Collider
		collider->position = root - position;
	}
}

void Turret::Editor()
{
	Editor::Float("View Distance", &viewDistance);
	Editor::Float("Fire Rate", &fireRate);
	Editor::IntSlider("Bullet Type", &bulletType, 0,1);
	Editor::CheckBox("Panning", &panning);

	if (panning)
	{
		Editor::Float("Pan Speed", &panSpeed);
		Editor::Float("Pan Interval", &panInterval);

		//Manipulators to edit target positions
		offsetA = Editor::Manipulator(uid + 1, position + offsetA) - position;
		offsetB = Editor::Manipulator(uid + 2, position + offsetB) - position;

		Editor::Float2("Point A", &offsetA[0]);
		Editor::Float2("Point B", &offsetB[0]);
	}
}

nlohmann::json Turret::Save()
{
	nlohmann::json data;
	data["viewDistance"] = viewDistance;
	data["fireRate"] = fireRate;
	data["bulletType"] = bulletType;
	data["panSpeed"] = panSpeed;
	data["panInterval"] = panInterval;
	data["panning"] = panning;

	data["posA"] = File::JsonVec2(offsetA);
	data["posB"] = File::JsonVec2(offsetB);
	return data;
}

void Turret::Load(nlohmann::json data)
{
	if(data.contains("viewDistance")) viewDistance = data["viewDistance"].get<float>();
	if(data.contains("fireRate")) fireRate = data["fireRate"].get<float>();
	if(data.contains("bulletType")) bulletType = data["bulletType"].get<float>();
	if(data.contains("panSpeed")) panSpeed = data["panSpeed"].get<float>();
	if(data.contains("panInterval")) panInterval = data["panInterval"].get<float>();
	if(data.contains("panning")) panning = data["panning"].get<bool>();

	if(data.contains("posA")) offsetA = File::Vec2Json(data["posA"]);
	if(data.contains("posB")) offsetB = File::Vec2Json(data["posB"]);
}
