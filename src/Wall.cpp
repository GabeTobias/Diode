#include "Wall.h"
#include "State.h"
#include "Editor.h"
#include "Props.h"

#include "ImGui.h"

using namespace glm;

Wall::Wall()
{
	className = "Wall";
	
	Tag("Obstacle");
	Tag("Roomless");

	trigger = new Triggerable(vec2(2.5, 0), vec2(4));
	AttachComponent(trigger);
	trigger->active = false;
	trigger->visible = false;

	collider = new Collidable(glm::vec2(1));
	AttachComponent(collider);

	destruct = new Destructable();
	AttachComponent(destruct);
}

void Wall::Awake()
{
	if (triggerable) 
	{
		trigger = (Triggerable*)GetComponent("Triggerable");
		destruct->active = false;
	}
}

void Wall::Draw()
{
	collider->bounds = clamp(glm::abs(Target), glm::vec2(1), glm::vec2(100));
	collider->position = (Target/2.f);

	float dist = glm::length(Target);
	glm::vec2 direction = normalize(Target);
	glm::quat rotation = glm::quatLookAt(glm::vec3(direction, 0), glm::vec3(0, 0, 1));

	glm::vec4 color = vec4(1, 0.5, 0, 1);

	if (triggerable) 
	{
		if (open) color = vec4(0.05, 0.05, 0.05, 1);
		else color = vec4(1, 0, 0.5, 1);
	}

	Render::useShadow = false;
	Render::DrawQuad(glm::vec3(position + (direction * (dist / 2.f)), 13), eulerAngles(rotation).z, glm::vec2(1.0f, dist), color);
	Render::useShadow = true;
}

void Wall::Update()
{
	if (triggerable && trigger)
	{
		//Toggle open on trigger
		open |= trigger->triggered;
		
		//disable collider
		if (open) collider->active = false;
	}
}

void Wall::Editor()
{
	Target = Editor::Manipulator(uid + 1, Target + position) - position;
	Editor::Float2("Target", &Target[0]);

	Editor::CheckBox("Triggered", &triggerable);

	//Update Trigger
	trigger->active = triggerable;
	trigger->visible = triggerable;
	destruct->active = !triggerable;
}

nlohmann::json Wall::Save()
{
	nlohmann::json data;
	data["target"] = File::JsonVec2(Target);
	data["triggerable"] = triggerable;
	return data;
}

void Wall::Load(nlohmann::json data)
{
	if (data.contains("target")) Target = File::Vec2Json(data["target"]);
	if (data.contains("triggerable")) triggerable = data["triggerable"].get<bool>();
}

