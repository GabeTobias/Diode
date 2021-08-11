#pragma once

#include <vector>
#include <map>

#include "World.h"

#include "nlohmann.hpp"
#include "File.h"

namespace Undo
{
	inline std::vector<nlohmann::json> states;				//Undo Stack
	inline std::vector<nlohmann::json> UndoneStates;		//Redo Stack

	inline void Store() 
	{
		states.push_back(File::SerializeScene());		//Add to Undo Stack
		UndoneStates.clear();							//Clear Redo Stack
	}

	inline bool Push()
	{
		//Return if Nothing to Redo
		if (UndoneStates.size() <= 0) return false;	

		//Add the last Undo action to current state
		states.push_back(UndoneStates.back());
		
		//Remove item from Undo Stack
		UndoneStates.pop_back();

		//Reload Scene OR Edited Actor
		File::DeserializeScene(states.back());

		//Return Success
		return true;
	}

	inline bool Pop() 
	{
		//Return if nothing to und
		if (states.size() <= 1) return false;

		//Add last action to Redo stack
		UndoneStates.push_back(states.back());
		
		//Remove it from Undo Stack
		states.pop_back();

		//Reload Scene OR Edited Actor
		File::DeserializeScene(states.back());

		//Return Success
		return true;
	}
}