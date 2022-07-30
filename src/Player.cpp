#include "Player.h"

#include "imgui.h"
#include "Props.h"

#include "Input.h"
#include "Renderer.h"

#include "GUI.h"
#include "Game.h"
#include "Physics.h"

#include "Bullet.h"
#include "Missile.h"
#include "Checkpoint.h"
#include "Stock.h"

#include "Destructable.h"
#include "Collidable.h"
#include "Movable.h"
#include "Listenable.h"
#include "Shocked.h"

using namespace glm;

float playerEmpOpacity;
vec2 playerEmpLocation;

bool playerLazerShooting = false;
bool playerLazerDepleted = false;
vec2 playerLazerLocation;

Player::Player(vec2 position)
{
	this->position = position;
	this->className = "Player";

	Tag("Player");
	Tag("Roomless");

	collider = new Collidable(vec2(1));
	AttachComponent(collider);

	movement = new Movable(collider);
	AttachComponent(movement);

	destruct = new Destructable();
	AttachComponent(destruct);
	
	Listenable* listen = new Listenable();
	AttachComponent(listen);

	Game::player = this;
	rotation = glm::quat(vec3());

	//TEMP
	dashMax = 4;
	energyMax = 25;
	energy = energyMax;
	energyRegen = 0.25;
}

void Player::Awake() 
{
	target = position;
}

void Player::Start() {}

void Player::Destroy()
{
	Checkpoint* check;
	std::vector<Entity*> checkpoints = World::GetEntitiesOfClass("Checkpoint");
	for(Entity* ent : checkpoints)
	{
		if (((Checkpoint*)ent)->pointID == currentCheckpoint)
		{
			check = (Checkpoint*)ent;
			break;
		}
	}

	//Recreate Character
	Player* p = new Player(check->position);
	
	//Pass Properties
	nlohmann::json data = Save();
	p->Load(data);

	//Add to World
	World::AddEntity(p);
}

void Player::Editor() 
{
	Editor::Float("Angular Speed", &angularSpeed);
	Editor::Float("Sensitivity", &sensitivity);
	Editor::Float("Acceleration", &acceleration);
	Editor::Float("Drag", &drag);
	Editor::Int("Checkpoint", &currentCheckpoint);
}

nlohmann::json Player::Save()
{
	nlohmann::json data;

	data["AngSpeed"] = angularSpeed;
	data["Sensitivity"] = sensitivity;
	data["Acceleration"] = acceleration;
	data["Drag"] = drag;

	return data;
}
void Player::Load(nlohmann::json data)
{
	if(data.contains("AngSpeed")) angularSpeed = data["AngSpeed"].get<float>();
	if (data.contains("Sensitivity")) sensitivity = data["Sensitivity"].get<float>();
	if (data.contains("Acceleration")) acceleration = data["Acceleration"].get<float>();
	if (data.contains("Drag")) drag = data["Drag"].get<float>();
}

void Player::Draw()
{
	rotation = glm::lerp(rotation, glm::quatLookAt(vec3(movement->velocity,0), vec3(0,0,1)), angularSpeed * (Time::timeScale/2.f));
	Render::DrawTri({ position.x, position.y, 10 }, eulerAngles(rotation).z, vec2(1), vec4(1,0.1,0.1,1));

	if(targeting) 
	{
		//Calculate Stick Direction
		float xIn = Input::gamePads[0].axis[GLFW_GAMEPAD_AXIS_LEFT_X];
		float yIn = -Input::gamePads[0].axis[GLFW_GAMEPAD_AXIS_LEFT_Y];

		//Calculate rotation
		vec2 missileDirection = normalize(vec2(xIn, yIn));
		quat rot = glm::quatLookAt(vec3(missileDirection, 0), vec3(0, 0, 1));

		Render::DrawTri({ position + (missileDirection * 2.f), 10 }, eulerAngles(rot).z, vec2(0.5f, 0.65f), vec4(1, 1, 1, 0.5));
	}

	if (missileTarget != nullptr) Render::DrawQuad(vec3(missileTarget->position, 8), 3.14 / 4.0f, vec2(2), vec4(1, 1, 1, 0.5), true, 0.1f);

	if (playerEmpOpacity > 0)
	{
		Render::DrawQuad(vec3(playerEmpLocation, 8), 3.14f / 4.f, vec2(36) * (1.0f - playerEmpOpacity), vec4(1, 1, 1, playerEmpOpacity * 0.25), true, 0.05f * playerEmpOpacity);
		playerEmpOpacity -= 0.025f;
	}
	
	if (playerLazerShooting)
	{
		//Calculate Lazer quad props
		float opacity = (active) ? 1.0f : 0.5f;
		float dist = length(playerLazerLocation - position);
		vec2 direct = normalize(playerLazerLocation - position);
		quat rotation = glm::quatLookAt(vec3(direct, 0), vec3(0, 0, 1));

		Render::DrawQuad(vec3(position + (direct * (dist / 2.f)), 13), eulerAngles(rotation).z, vec2(megaLazer ? 1.0:0.5, dist), vec4(1, megaLazer ? 0.0:0.5, 0, 1));
	}
}

void Player::Update()
{
	//Interpolated Inputs
	static float xInput = 0;
	static float yInput = 0;

	//Vehicle Movement
	{
		if (!Input::gamePads[0].active) 
		{
			//X - Axis
			if (Input::GetKey(GLFW_KEY_A)) xInput = glm::lerp(xInput, -1.f, sensitivity);
			else if (Input::GetKey(GLFW_KEY_D)) xInput = glm::lerp(xInput, 1.f, sensitivity);
			else xInput = glm::lerp(xInput, 0.f, drag);

			//Y - Axis
			if (Input::GetKey(GLFW_KEY_S)) yInput = glm::lerp(yInput, -1.f, sensitivity);
			else if (Input::GetKey(GLFW_KEY_W)) yInput = glm::lerp(yInput, 1.f, sensitivity);
			else yInput = glm::lerp(yInput, 0.f, drag);
		} 
		else 
		{
			//X - Axis
			xInput = glm::lerp(xInput, Input::gamePads[0].axis[GLFW_GAMEPAD_AXIS_LEFT_X], sensitivity);

			//Y - Axis
			yInput = glm::lerp(yInput, -Input::gamePads[0].axis[GLFW_GAMEPAD_AXIS_LEFT_Y], sensitivity);
		}

		//Set Velocity
		movement->velocity = vec2(xInput, yInput) * acceleration * dash;
	}

	//Camera Movement
	{
		//Calculate Stick Direction
		float xIn = Input::gamePads[0].axis[GLFW_GAMEPAD_AXIS_LEFT_X];
		float yIn = -Input::gamePads[0].axis[GLFW_GAMEPAD_AXIS_LEFT_Y];

		//Calculate rotation
		vec2 missileDirection = normalize(vec2(xIn, yIn));

		camera->position = glm::lerp(camera->position, (targeting) ? target + (missileDirection*4.f):target, 0.05f);
		camera->size = glm::lerp(camera->size, (targeting) ? 20.0f:25.f, 0.05f);
	}

	//Energy Regeneration
	{
		if(energy < energyMax && !playerLazerDepleted) energy += energyRegen;
	}

	//Shooting
	{
		if (Input::GetKey(GLFW_KEY_ENTER) || Input::gamePads[0].buttons[GLFW_GAMEPAD_BUTTON_X])
		{
			if (bulletTimer.getInterval(0.25f))
			{
				vec2 direction = normalize(vec2(xInput, yInput));
				Bullet* b = new Bullet(position + (direction*1.f),direction*0.6f);
				World::AddEntity(b);
			}
		}
	}

	//Dashing
	{
		//Start Dash
		if (Input::GetKeyDown(GLFW_KEY_LEFT_SHIFT) || Input::gamePads[0].buttons[GLFW_GAMEPAD_BUTTON_A])
		{
			if (dash == 1 && dashCount <= dashMax && !dashing && movement->active)
			{
				if (Input::gamePads[0].buttons[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] && energy >= 15)
				{
					dash = 7;
					dashCount++;
					energy -= 15;
				}
				else 
				{
					dash = 3;
					dashCount++;
				}
				
				dashing = true;
				dashResetTimer.reset();
				dashTimer.reset();
			}
		}

		//Reset Dash Count
		if (dashCount > 0 && dash == 1 && dashResetTimer.getInterval(0.5)) dashCount--;

		//Reset Dash Flag
		if (!Input::GetKeyDown(GLFW_KEY_LEFT_SHIFT) && !Input::gamePads[0].buttons[GLFW_GAMEPAD_BUTTON_A])
			dashing = false;

		//Stop Dash
		if (dashTimer.getInterval(0.15) && dash != 1) dash = 1;

		//Update Collider
		collider->active = dash == 1;
	}

	//EMP
	{
		static bool shocking = false;
		if (Input::GetKeyDown(GLFW_KEY_ENTER) || (Input::gamePads[0].buttons[GLFW_GAMEPAD_BUTTON_Y] && !shocking))
		{
			//Cancel if right bumper pressed (SHIELD DROP)
			if (!Input::gamePads[0].buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER]) 
			{
				//Visuals
				playerEmpLocation = position;
				playerEmpOpacity = 1.0;

				for (World::Entity* ent : World::entities)
				{
					//Skip self
					if (ent->uid == uid) continue;
					if (ent->className == "Bullet" || ent->className == "Missile") continue;

					//TODO: Implement Tag System
					//TODO: Use Tag System similar to room
					World::Component* moveComp = ent->GetComponent("Movable");
					float dist = distance(position, ent->position);

					//Check if in range and shockable
					if (dist < 24)
					{
						//TODO: Make shock time adjustable
						ent->AttachComponent(new Shocked(3));

						//Set to Drift
						if (moveComp != nullptr) ((Movable*)moveComp)->drifting = true;
					}

				}
			}
		}
		shocking = Input::gamePads[0].buttons[GLFW_GAMEPAD_BUTTON_B];
	}

	//Missiles
	{
		static bool charging = false;
		if (Input::gamePads[0].buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] && Input::gamePads[0].buttons[GLFW_GAMEPAD_BUTTON_Y])
		{
			Time::timeScale = lerp(Time::timeScale, 0.1f, 0.2f);
			targeting = true;

			//Calculate Stick Direction
			float xIn = Input::gamePads[0].axis[GLFW_GAMEPAD_AXIS_LEFT_X];
			float yIn = -Input::gamePads[0].axis[GLFW_GAMEPAD_AXIS_LEFT_Y];
			vec2 missileDirection = normalize(vec2(xIn, yIn));

			//Missile Target
			std::vector<Physics::CollisionInfo> hits = Physics::RayTrace(position, position + (missileDirection * 36.f), 72, collider->uid, 100);

			//Loop threw all possible hits
			for (Physics::CollisionInfo info : hits)
			{
				//TODO: Use Tag System
				//Skip flagged Items
				if (info.component->owner->className == "Room") continue;
				if (info.component->owner->className == "Bullet") continue;
				if (info.component->owner->className == "Missile") continue;

				//check if targetable
				World::Component* comp = collider->owner->GetComponent("Destructable");
				if (comp != nullptr)
				{
					missileTarget = info.component->owner;
					break;
				}
			}
		}
		else 
		{
			if(charging)
			{
				vec2 direction = normalize(vec2(xInput, yInput));
				Missile* m = new Missile(position + (direction * 2.f), direction * 0.4f, 2, missileTarget);
				m->acurracy = 0.2;
				World::AddEntity(m);

				missileTarget = nullptr;
			}

			missileTarget = nullptr;

			Time::timeScale = lerp(Time::timeScale, 1.f, 0.2f);
			targeting = false;
		}
		charging = Input::gamePads[0].buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] && Input::gamePads[0].buttons[GLFW_GAMEPAD_BUTTON_Y];
	}

	//Lazer
	{
		if (energy <= 3 && !megaLazer) playerLazerDepleted = true;

		if (!Input::gamePads[0].buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER])
			playerLazerDepleted = false;

		//TODO: Make energy > 3 a variable
		//Raycast to Hit
		if (Input::gamePads[0].buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] && energy > 3 && !playerLazerDepleted)
		{
			vec2 direction = rotate(vec2(0, 1), eulerAngles(rotation).z);
			std::vector<Physics::CollisionInfo> hits = Physics::RayTrace(position + (direction * 2.f), position + (direction * 36.f), 72, collider->uid);

			//Loop threw all possible hits
			for (Physics::CollisionInfo info : hits)
			{
				//Do damage
				World::Component* destruct = info.component->owner->GetComponent("Destructable");
				if (destruct)
				{
					//TODO: Make this variable
					//Try to find Destructable Component
					((Destructable*)destruct)->TakeDamage(0.2f * (megaLazer ? 2:1));
				}
			}

			//Raycast for hit position
			Physics::CollisionInfo visualHit = Physics::Raycast(position + (direction * 2.f), position + (direction * 56.f), 144.f);
			if (visualHit)
			{
				playerLazerLocation = visualHit.position;
				playerLazerShooting = true;
			}

			if(!megaLazer)
				energy -= 1.25f;
		}
		else playerLazerShooting = false;
	}

	//Shields
	{
		static bool shielding = false;
		if (Input::gamePads[0].buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] && Input::gamePads[0].buttons[GLFW_GAMEPAD_BUTTON_B]) 
		{
			if (!shielding) 
			{
				Stock* stok = new Stock(position, normalize(movement->velocity));
				World::AddEntity(stok);
			}
		
		}
		shielding = (Input::gamePads[0].buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] && Input::gamePads[0].buttons[GLFW_GAMEPAD_BUTTON_B]);
	}
}

void Player::GUI() 
{
	//Health Bar
	GUI::DrawBoxUI(vec2(100, window->height - 25 - 20) , vec2(((destruct->maxDamage - destruct->damage) / destruct->maxDamage) * 300, 25), vec4(1,1,1,0.5), 0);
	GUI::DrawBoxUI(vec2(100, window->height - 25 - 20) , vec2(300, 25), vec4(0,0,0,0.5), 0);

	//Energy Bar
	float depleteOpacity = ((sin(Time::getTime()) + 1) / 2.f) * 0.5;
	GUI::DrawBoxUI(vec2(100, window->height - 60 - 20), vec2((energy / 100) * 300, 25), vec4(1, 1, 1, (playerLazerDepleted) ? depleteOpacity:0.5), 0);
	GUI::DrawBoxUI(vec2(100, window->height - 60 - 20), vec2(300, 25), vec4(0, 0, 0, 0.5), 0);

	//Dash	
	if (dashMax - dashCount >= 1) GUI::DrawBoxUI(vec2(0  + 20, window->height - 60 - 20), vec2(25), vec4(1, 1, 1, 0.5), 0);
	if (dashMax - dashCount >= 2) GUI::DrawBoxUI(vec2(35 + 20, window->height - 60 - 20), vec2(25), vec4(1, 1, 1, 0.5), 0);
	if (dashMax - dashCount >= 3) GUI::DrawBoxUI(vec2(0  + 20, window->height - 25 - 20), vec2(25), vec4(1, 1, 1, 0.5), 0);
	if (dashMax - dashCount >= 4) GUI::DrawBoxUI(vec2(35 + 20, window->height - 25 - 20), vec2(25), vec4(1, 1, 1, 0.5), 0);

	GUI::DrawBoxUI(vec2(0 + 20, window->height - 60 - 20), vec2(25),  vec4(0, 0, 0, 0.5), 0);
	GUI::DrawBoxUI(vec2(35 + 20, window->height - 60 - 20), vec2(25), vec4(0, 0, 0, 0.5), 0);
	GUI::DrawBoxUI(vec2(0 + 20, window->height - 25 - 20), vec2(25),  vec4(0, 0, 0, 0.5), 0);
	GUI::DrawBoxUI(vec2(35 + 20, window->height - 25 - 20), vec2(25), vec4(0, 0, 0, 0.5), 0);


	GUI::DrawBoxUI(vec2(10, window->height - 80 - 10), vec2(80), vec4(0), 0, 3.f, vec4(1,1,1,0.5));
} 