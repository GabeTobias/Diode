#pragma once
#include <glm/glm.hpp>

#include "World.h"
#include "Audio.h"

class Hearable : public World::Component
{
public:
	AudioSource* source;
	Sound* sound;

	inline Hearable()
	{
		className = "Hearable";
		source = new AudioSource();
	}

	inline void Update()
	{
		source->SetLocation(owner->position);
	}

	inline void SetVolume(float volume) { source->SetVolume(volume); }

	inline void Play(std::string src) { source->Play(Audio::GetSound(src)); }
};