#pragma once

#include <string>
#include <map>

#include "imgui.h"
#include "imgui_str.h"

namespace Editor
{
	//Every function call is simply a wrapper to Imgui that allows
	//me to group detech changes in ui. That way I can detect if 
	//an entire actor has changed rather than each individual item

	/*
		enum PropType 
		{
			CHECKBOX,
			FLOAT,
			FLOAT2,
			FLOAT3,
			COLOR3,
			COLOR4,
			INT,
			INT2,
			INTSLIDER,
			STRING
		};
	*/

	struct GroupEditProp
	{
		void* srcPtr;
		bool edited = false;
	};

	inline std::map<std::string, GroupEditProp> editPropMap;

	//Wheather an Item has Changed
	inline bool propsChanged = false;

	///////////////////////////////////////////////////////////////////////////

	void BeginChangeGroup();
	bool EndChangeGroup();
	
	///////////////////////////////////////////////////////////////////////////

	void CheckBox(std::string str, bool* value);

	///////////////////////////////////////////////////////////////////////////
	
	void Float(std::string str, float* value);
	void Float2(std::string str, float* value);
	void Float3(std::string str, float* value);

	///////////////////////////////////////////////////////////////////////////

	void Color3(std::string str, float* value);
	void Color4(std::string str, float* value);

	///////////////////////////////////////////////////////////////////////////

	void Int(std::string str, int* value);
	void Int2(std::string str, int* value);
	void IntSlider(std::string str, int* value, int min, int max);

	///////////////////////////////////////////////////////////////////////////
	
	void String(std::string str, std::string* value);
}