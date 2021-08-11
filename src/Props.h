#pragma once

#include <string>

#include "imgui.h"
#include "imgui_str.h"

namespace Editor
{
	//Every function call is simply a wrapper to Imgui that allows
	//me to group detech changes in ui. That way I can detect if 
	//an entire actor has changed rather than each individual item

	//Wheather an Item has Changed
	inline bool propsChanged = false;

	///////////////////////////////////////////////////////////////////////////

	inline void BeginChangeGroup() 
	{
		propsChanged = false;	//Reset the change state
	}

	inline bool EndChangeGroup() 
	{
		return propsChanged;	//Get the change state
	}

	///////////////////////////////////////////////////////////////////////////

	inline void CheckBox(std::string str, bool* value) 
	{
		ImGui::Checkbox(str.c_str(), value);						//ImGui item
		propsChanged |= ImGui::IsItemDeactivatedAfterEdit();		//Store item edit
	}

	///////////////////////////////////////////////////////////////////////////
	
	inline void Float(std::string str, float* value) 
	{
		ImGui::DragFloat(str.c_str(), value);						//ImGui item
		propsChanged |= ImGui::IsItemDeactivatedAfterEdit();		//Store item edit
	}

	inline void Float2(std::string str, float* value)
	{
		ImGui::DragFloat2(str.c_str(), value);						//ImGui item
		propsChanged |= ImGui::IsItemDeactivatedAfterEdit();		//Store item edit
	}

	inline void Float3(std::string str, float* value)
	{
		ImGui::DragFloat3(str.c_str(), value);						//ImGui item
		propsChanged |= ImGui::IsItemDeactivatedAfterEdit();		//Store item edit
	}

	///////////////////////////////////////////////////////////////////////////

	inline void Color3(std::string str, float* value)
	{
		ImGui::ColorEdit3(str.c_str(), value);						//ImGui item
		propsChanged |= ImGui::IsItemDeactivatedAfterEdit();		//Store item edit
	}

	inline void Color4(std::string str, float* value)
	{
		ImGui::ColorEdit4(str.c_str(), value);						//ImGui item
		propsChanged |= ImGui::IsItemDeactivatedAfterEdit();		//Store item edit
	}

	///////////////////////////////////////////////////////////////////////////

	inline void Int(std::string str, int* value) 
	{
		ImGui::DragInt(str.c_str(), value);							//ImGui item
		propsChanged |= ImGui::IsItemDeactivatedAfterEdit();		//Store item edit
	}

	inline void Int2(std::string str, int* value)
	{
		ImGui::DragInt2(str.c_str(), value);						//ImGui item
		propsChanged |= ImGui::IsItemDeactivatedAfterEdit();		//Store item edit
	}

	inline void IntSlider(std::string str, int* value, int min, int max)
	{
		ImGui::SliderInt(str.c_str(), value, min, max);				//ImGui item
		propsChanged |= ImGui::IsItemDeactivatedAfterEdit();		//Store item edit
	}

	///////////////////////////////////////////////////////////////////////////
	
	inline void String(std::string str, std::string* value) 
	{
		InputText(str.c_str(), value, 2048, 0, 0);					//ImGui item
		propsChanged |= ImGui::IsItemDeactivatedAfterEdit();		//Store item edit
	}
}