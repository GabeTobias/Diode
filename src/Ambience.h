#pragma once
#include <string>
#include <glm/glm.hpp>

#include "World.h"
#include "Audio.h"

class Ambience : public World::Entity
{
public:
	std::string soundSource = "bat.wav";
	glm::vec2 bounds = glm::vec2(2);

	float fadeIn = 0.001f;
	float fadeOut = 0.005f;

	AudioSource* source;
	bool containsListener = false;

public:
	Ambience();

	void Awake();

	void Update();
	void Draw();

	void Editor();

	void Delete();
	void Destroy();

	nlohmann::json Save();
	void Load(nlohmann::json data);

	static World::Entity* Construct() { return new Ambience(); }
};