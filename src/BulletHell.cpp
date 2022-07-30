#include "BulletHell.h"

#include "Renderer.h"
#include "Props.h"
#include "Bullet.h"

using namespace glm;

BulletHell::BulletHell()
{ 
	className = "BulletHell"; 

	collider = new Collidable(vec2(2));
	AttachComponent(collider);

	destruct = new Destructable();
	AttachComponent(destruct);
}


void BulletHell::Awake()
{
	phaseTimer.reset();
	waveTimer.reset();
}

void BulletHell::Update()
{
	if (phaseTimer.getInterval(phaseInterval)) 
	{
		phase = (phase + 1) % 4;
	}

	if (phase == 0)
	{
		rotation = 0;
		SpawnBullets(24, waveInterval);
	}

	if (phase == 1)
	{
		SpawnBullets(4, spinInterval);
		rotation += rotationSpeed;
	}

	if (phase == 2)
	{
		SpawnBullets(8, spinInterval);
		rotation += rotationSpeed;
	}

	// Overheat phase
	// What if the entire computer overheats like a cpu so the room 
	// itself slowly becomes redder and smokier
	if (phase == 4) {}
}

void BulletHell::Draw()
{
	vec4 color = vec4(0, 1, 0.9, 1);
	Render::DrawQuad(vec3(position, 12), (3.14 / 2.0f) - rotation, vec2(2), color);
	Render::DrawQuad(vec3(position, 12), (3.14 / 2.0f) - rotation, vec2(3), color, true);
}

void BulletHell::SpawnBullets(int count, float interval)
{
	if (waveTimer.getInterval(interval))
	{
		float bulletCount = (float)count;

		for (float i = 0; int(i) < bulletCount; i++)
		{
			float tau = ((2 * 3.14f) / bulletCount) * i;
			vec2 direction = vec2(sin(tau + rotation), cos(tau + rotation));

			Entity* bullet = new Bullet(position + direction * 3.0f, direction * 0.3f);
			World::AddEntity(bullet);
		}
	}
}

void BulletHell::Editor()
{
	Editor::Int("Phase", &phase);
	Editor::Float("Phase Interval", &phaseInterval);
	Editor::Float("Wave Interval", &waveInterval);
	Editor::Float("Spin Interval", &spinInterval);
	Editor::Float("Spin Speed", &rotationSpeed);
	Editor::Float("Bullet Speed", &bulletSpeed);
}

nlohmann::json BulletHell::Save()
{
	nlohmann::json data;

	data["phase"] = phase;
	data["phaseInterval"] = phaseInterval;
	data["waveInterval"] = waveInterval;
	data["spinInterval"] = spinInterval;
	data["spinSpeed"] = rotationSpeed;
	data["bulletSpeed"] = bulletSpeed;

	return data;
}

void BulletHell::Load(nlohmann::json data)
{
	phase = data["phase"].get<int>();
	phaseInterval = data["phaseInterval"].get<float>();
	waveInterval = data["waveInterval"].get<float>();
	spinInterval = data["spinInterval"].get<float>();
	rotationSpeed = data["spinSpeed"].get<float>();
	bulletSpeed = data["bulletSpeed"].get<float>();
}
