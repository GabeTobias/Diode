#pragma once

#include "World.h"
#include "File.h"

#include "Player.h"
#include "Checkpoint.h"
#include "Room.h"
#include "Wall.h"

#include "Note.h"

#include "Turret.h"
#include "Mine.h"
#include "Lazer.h"
#include "Drone.h"
#include "Hive.h"
#include "Pickup.h"
#include "Charger.h"
#include "Phazer.h"
#include "Killpad.h"
#include "Breaker.h"
#include "Dropper.h"
#include "Wanderer.h"
#include "Stock.h"
#include "Graviton.h"
#include "Ambience.h"
#include "DashTarget.h"

#include "Destructable.h"
#include "Triggerable.h"
#include "Collidable.h"
#include "Movable.h"
#include "Pannable.h"
#include "Shield.h"

namespace File 
{
	inline void BindConstructors()
	{
		//Entt Constructors
		MapConst("Player", &Player::Construct);
		MapConst("Room", &Room::Construct);
		MapConst("Checkpoint", &Checkpoint::Construct);
		MapConst("Health", &Health::Construct);
		MapConst("Stock", &Stock::Construct);
		MapConst("Turret", &Turret::Construct);
		MapConst("Drone", &Drone::Construct);
		MapConst("Hive", &Hive::Construct);
		MapConst("Charger", &Charger::Construct);
		MapConst("Phazer", &Phazer::Construct);
		MapConst("Dropper", &Dropper::Construct);
		MapConst("Wanderer", &Wanderer::Construct);

		//Audio Entt Constructor
		MapConst("Ambience", &Ambience::Construct);

		//Editor Entt Constructor
		MapConst("Note", &Note::Construct);
		
		//Traps Constructors
		MapConst("Wall", &Wall::Construct);
		MapConst("Lazer", &Lazer::Construct);
		MapConst("Mine", &Mine::Construct);
		MapConst("Killpad", &Killpad::Construct);
		MapConst("Breaker", &Breaker::Construct);
		MapConst("Graviton", &Graviton::Construct);
		MapConst("DashTarget", &DashTarget::Construct);
		MapConst("LazerPilot", &DashTarget::Construct);
	
		//Component Constructors
		MapConst("Destructable", &Destructable::Construct);
		MapConst("Triggerable", &Triggerable::Construct);
		MapConst("Collidable", &Collidable::Construct);
		MapConst("Movable", &Movable::Construct);
		MapConst("Pannable", &Pannable::Construct);
		MapConst("Shield", &Shield::Construct);
	}
}