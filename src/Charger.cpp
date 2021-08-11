#include "Charger.h"
#include "Renderer.h"
#include "Editor.h"
#include "Game.h"
#include "State.h"
#include "Killpad.h"

using namespace glm;

Charger::Charger(glm::vec2 pos)
{
	className = "Charger";
	Tag("Enemy");

	this->position = pos;

	collider = new Collidable(vec2(1.5));
	AttachComponent(collider);

	rigidbody = new Movable(collider);
	AttachComponent(rigidbody);

	destruct = new Destructable();
	AttachComponent(destruct);

	destruct->maxDamage = 2;
	collider->layer = 1;
}

void Charger::Update()
{
	//Update engagment
	engaged = distance(position, Game::player->position) < attackRadius;

	//Check cooldown timer
	if (engaged && !attacking && cooldownTimer.getInterval(rateOfAttack))
	{
		attacking = true;
		chargeDirection = normalize(Game::player->position - position);
		chargeStartPosition = position;
	}

	//Charge
	if (engaged && attacking)
	{
		//Stop Moving
		if (distance(position, chargeStartPosition + (chargeDirection * attackRadius)) <= 2 || distance(position, Game::player->position) <= 2)
			rigidbody->velocity = lerp(rigidbody->velocity, vec2(0), 0.2f);
		else
			rigidbody->velocity = chargeDirection * chargeSpeed;

		//Checke distance to player
		if (distance(position, Game::player->position) <= 2)
		{
			//Camera Shake
			camera->delta += normalize(chargeDirection);

			//Explode
			if (explosive) 
			{
				for (int i = 0; i < 8; i++)
				{
					float ang = i * (3.14 / 4.0);
					vec2 dir = normalize(vec2(sin(ang), cos(ang)));

					Bullet* b = new Bullet(position + (dir * 1.2f), dir * 0.3f);
					World::AddEntity(b);
				}

				World::DeleteEntity(this);
			}
			else 
			{
				Game::player->destruct->TakeDamage(damage);
			}

			attacking = false;
		}

		//Check distance to charge target
		if (distance(position, chargeStartPosition + (chargeDirection * attackRadius)) <= 2 || cooldownTimer.getInterval(rateOfAttack))
		{
			attacking = false;
			dashCount++;

			if (dashCount > 3 && explosive)
			{
				for (int i = 0; i < 8; i++)
				{
					float ang = i * (3.14 / 4.0);
					vec2 dir = normalize(vec2(sin(ang), cos(ang)));

					Bullet* b = new Bullet(position + (dir * 1.2f), dir * 0.3f);
					World::AddEntity(b);
				}

				World::DeleteEntity(this);
			}
		}

		//Destroy on collision
		if (explosive && attacking && rigidbody->colliding)
		{
			for (int i = 0; i < 8; i++)
			{
				float ang = i * (3.14 / 4.0);
				vec2 dir = normalize(vec2(sin(ang), cos(ang)));

				Bullet* b = new Bullet(position + (dir * 1.2f), dir * 0.3f);
				World::AddEntity(b);
			}

			World::DeleteEntity(this);
		}

		//Reset attack if movement stops
		if (length(rigidbody->velocity) < 0.05)
		{					
			attacking = false;
		}

		//Leave Trail
		if (trailing && trailTimer.getInterval(0.1f)) 
		{
			vec2 snapPos = vec2 (
				int(position.x),
				int(position.y)
			);

			Killpad* pad = new Killpad(snapPos);
			pad->bounds = vec2(3);
			pad->dying = true;
			pad->hitTime = 2;
			pad->coolDownTime = 0.25f;
			pad->deathTime = 4;
			World::AddEntity(pad);
		}
	}
	else 
	{
		rigidbody->velocity = lerp(rigidbody->velocity, vec2(0), 0.1f);
		attacking = false;
	}

}

void Charger::Draw()
{
	float opacity = (active) ? 1.0f : 0.5f;
	vec4 color = (attacking) ? vec4(1,0,0.5,opacity) : lerp(vec4(1,0.5,0,opacity), vec4(1,0,0.5,opacity), cooldownTimer.delta(rateOfAttack));
	vec2 direction = normalize(Game::player->position - position);
	
	if(engaged && active) rotation = glm::quatLookAt(vec3(direction, 0), vec3(0, 0, 1));
	if(engaged && active) offset = direction / 2.f;

	if(trailing) Render::DrawQuad(vec3(position,10), eulerAngles(rotation).z, vec2(1,0.5), color);
	else Render::DrawCircle(vec3(position,10), vec2(1), color);

	if(explosive) Render::DrawQuad(vec3(position + offset, 10), eulerAngles(rotation).z, vec2(1), color);
	else Render::DrawTri(vec3(position + offset,10), eulerAngles(rotation).z, vec2(1), color);

	if (GameState::Mode == EditMode)
		Render::DrawCircle(vec3(position, 2), vec2(attackRadius * 2), vec4(1, 1, 1, 0.15), true, 0.01f);
}

void Charger::Editor()
{
	Editor::Float("Attack Rate", &rateOfAttack);
	Editor::Float("Charge Speed", &chargeSpeed);
	Editor::Float("Attack Radius", &attackRadius);

	Editor::Int("Attack Damage", &damage);

	Editor::CheckBox("Explosive", &explosive);
	Editor::CheckBox("Shock Trail", &trailing);
}

nlohmann::json Charger::Save()
{
	nlohmann::json data;

	data["rateOfAttack"] = rateOfAttack;
	data["chargeSpeed"] = chargeSpeed;
	data["attackRadius"] = attackRadius;
	data["damage"] = damage;
	data["explosive"] = explosive;
	data["trailing"] = trailing;

	return data;
}

void Charger::Load(nlohmann::json data)
{
	if (data.contains("rateOfAttack")) rateOfAttack = data["rateOfAttack"].get<float>();
	if (data.contains("chargeSpeed")) chargeSpeed = data["chargeSpeed"].get<float>();
	if (data.contains("attackRadius")) attackRadius = data["attackRadius"].get<float>();
	if (data.contains("damage")) damage = data["damage"].get<int>();
	if (data.contains("explosive")) explosive = data["explosive"].get<bool>();
	if (data.contains("trailing")) trailing = data["trailing"].get<bool>();
}

