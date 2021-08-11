#include "Ambience.h"

#include "Renderer.h"
#include "Coords.h"

#include "File.h"
#include "Props.h"

using namespace glm;

Ambience::Ambience()
{
	className = "Ambience";
	Tag("Roomless");
}

void Ambience::Awake() 
{
	editorBounds = bounds;

	source = new AudioSource();
	source->SetLoop(true);
	source->SetVolume(0);
	source->Play(Audio::GetSound(soundSource));
}

void Ambience::Update()
{
	if(source->gain >= 0.01) source->SetLocation(Audio::listenerPos);

	if (PointOverlaps(position - (bounds / 2.f), bounds, Audio::listenerPos))
	{
		//Set Source Properties
		source->SetVolume(clamp(source->gain + fadeIn, 0.f,0.5f));
		containsListener = true;
	}
	else 
	{
		containsListener = false;
		source->SetVolume(clamp(source->gain - fadeOut, 0.f, 0.5f));
	}
}

void Ambience::Draw()
{
	Render::DrawQuad(vec3(position, 15), 0, bounds, vec4(0.5,1,0.5,0.25), true, camera->size / (150.f * bounds.x));
	Render::DrawQuad(vec3(position, 15), 0, bounds, vec4(0.5,1,0.5,0.05));
}

void Ambience::Editor()
{
	Editor::Float("FadeIn", &fadeIn);
	Editor::Float("FadeOut", &fadeOut);
	Editor::Float2("Bounds", &bounds[0]);
	Editor::String("Sound", &soundSource);

	editorBounds = bounds;
}

void Ambience::Delete()
{
	source->Delete();
}

void Ambience::Destroy()
{
	source->Delete();
}

nlohmann::json Ambience::Save()
{
	nlohmann::json data;

	data["src"] = soundSource.c_str();
	data["bounds"] = File::JsonVec2(bounds);

	data["fadeIn"] = fadeIn;
	data["fadeOut"] = fadeOut;

	return data;
}

void Ambience::Load(nlohmann::json data)
{
	if (data.contains("fadeIn")) fadeIn = data["fadeIn"].get<float>();
	if (data.contains("fadeOut")) fadeOut = data["fadeOut"].get<float>();
	if (data.contains("src")) soundSource = data["src"].get<std::string>();
	if (data.contains("bounds")) bounds = File::Vec2Json(data["bounds"]);
}
