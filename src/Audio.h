#pragma once

#include <string>
#include <map>

#include "glm/glm.hpp"

#include "al.h" 
#include "alc.h"
#include "alut.h"

#define NUM_BUFFERS = 16

struct Sound
{
	std::string	src;

	ALuint		buffer;
	ALenum		format;
	ALsizei		size;
	ALsizei		freq;
	ALboolean	loop;

	void* data;
};

class AudioSource
{
public:
	glm::vec2 position;
	unsigned int id;

	bool looping = false;

	float gain = 1;
	float pitch = 1;

public:
	inline AudioSource() { Init(); }

	inline void SetLoop(bool loop){ alSourcei(id, AL_LOOPING, loop); looping = loop; }
	inline void SetVolume(float volume){ alSourcef(id, AL_GAIN, gain); gain = volume; }
	inline void SetLocation(glm::vec2 pos) { alSource3f(id, AL_POSITION, 0, position.x, position.y); position = pos; }

	inline void Play(Sound* sound)
	{	
		////Source Properties
		alSourcef(id, AL_PITCH, 1);
		alSourcef(id, AL_GAIN, gain);
		alSource3f(id, AL_POSITION, 0, position.x, position.y);
	
		//Bind Sound
		alSourcei(id, AL_BUFFER, sound->buffer);

		//Play Source
		alSourcePlay(id);
	}

	inline void Init() { alGenSources(1, &id); }
	inline void Delete() { alDeleteSources(1, &id); }
};

namespace Audio
{
	inline ALCdevice* _device;
	inline ALCcontext* _context;
	inline ALboolean	_eax;

	inline std::map<std::string, Sound*> _sources;

	inline glm::vec2 listenerPos = glm::vec2(0);

	inline Sound* LoadSound(std::string src)
	{
		// Clear error code 
		alGetError();

		//Create New Audio Buffer
		Sound* sound = new Sound();
		sound->src = src;

		// Generate Buffers 
		alGenBuffers(1, &sound->buffer);

		//Check for buffer errors
		ALenum error = alGetError();
		if (error != AL_NO_ERROR)
		{
			printf("Error Generating Buffers: %s \n", alutGetErrorString(error));
			return nullptr;
		}

		//Load Audio Data from file
		alutLoadWAVFile(
			(ALbyte*)(("resources/sounds/" + src).c_str()),
			&sound->format,
			&sound->data,
			&sound->size, &sound->freq,
			&sound->loop
		);

		//Check for load errors
		error = alutGetError();
		if (error != AL_NO_ERROR)
		{
			printf("Failed to load: %s \n", alutGetErrorString(error));
			alDeleteBuffers(1, &sound->buffer);
			return nullptr;
		}

		// Copy test.wav data into AL Buffer 0 
		alBufferData(sound->buffer, sound->format, sound->data, sound->size, sound->freq);

		//Check for buffer errors
		error = alutGetError();
		if (error != AL_NO_ERROR)
		{
			printf("Error Filling Buffers: %s \n", alutGetErrorString(error));
			alDeleteBuffers(1, &sound->buffer);
			return nullptr;
		}

		return sound;
	}
	inline Sound* GetSound(std::string src)
	{
		Sound* s = _sources[src];

		if (s != nullptr) return s;
		else
		{
			_sources[src] = LoadSound(src);
			return _sources[src];
		}
	}
	
	inline void SetListener(glm::vec2 pos, glm::vec2 vel = glm::vec2(0))
	{
		alListener3f(AL_POSITION, 0, pos.x, pos.y);
		alListener3f(AL_VELOCITY, 0, vel.x, vel.y);

		listenerPos = pos;
	}

	inline void Init() 
	{
		// Initialization 
		_device = alcOpenDevice(NULL);

		// select the "preferred device" 
		if (_device)
		{
			_context = alcCreateContext(_device, NULL);
			alcMakeContextCurrent(_context);
		}

		// Check for EAX 2.0 support 
		_eax = alIsExtensionPresent("EAX2.0");

		//Set Default Audio Listener Position
		SetListener({ 0,0 });
	}
}