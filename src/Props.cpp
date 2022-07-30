#include "Props.h"

#include <string>
#include <map>

#include "imgui.h"
#include "imgui_str.h"

namespace Editor
{
	void BeginChangeGroup()
	{
		propsChanged = false;	//Reset the change state
	}

	bool EndChangeGroup()
	{
		editPropMap.clear();
		return propsChanged;	//Get the change state
	}

	void StoreGroupProp(std::string str, void* value) 
	{
		GroupEditProp gep;
		gep.srcPtr = value;

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			gep.edited = true;
			propsChanged = true;
		}

		editPropMap[str] = gep;
	}

	///////////////////////////////////////////////////////////////////////////

	void CheckBox(std::string str, bool* value)
	{
		if (editPropMap.contains(str)) 
		{
			if(editPropMap[str].edited)
				*value = *(bool*)editPropMap[str].srcPtr;
		}
		else
		{
			ImGui::Checkbox(str.c_str(), value);						//ImGui item
			StoreGroupProp(str, value);
		}
	}

	///////////////////////////////////////////////////////////////////////////

	void Float(std::string str, float* value)
	{
		if (editPropMap.contains(str))
		{
			if (editPropMap[str].edited)
				*value = *(float*)editPropMap[str].srcPtr;
		}
		else
		{
			ImGui::DragFloat(str.c_str(), value);					//ImGui item
			StoreGroupProp(str, value);
		}
	}

	void Float2(std::string str, float* value)
	{
		if (editPropMap.contains(str))
		{
			if (editPropMap[str].edited)
				*value = *(float*)editPropMap[str].srcPtr;
		}
		else
		{
			ImGui::DragFloat2(str.c_str(), value);					//ImGui item
			StoreGroupProp(str, value);
		}
	}

	void Float3(std::string str, float* value)
	{
		if (editPropMap.contains(str))
		{
			if (editPropMap[str].edited)
				*value = *(float*)editPropMap[str].srcPtr;
		}
		else
		{
			ImGui::DragFloat3(str.c_str(), value);					//ImGui item
			StoreGroupProp(str, value);
		}
	}

	///////////////////////////////////////////////////////////////////////////

	void Color3(std::string str, float* value)
	{
		if (editPropMap.contains(str))
		{
			if (editPropMap[str].edited)
				*value = *(float*)editPropMap[str].srcPtr;
		}
		else
		{
			ImGui::ColorEdit3 (str.c_str(), value);					//ImGui item
			StoreGroupProp(str, value);
		}
	}

	void Color4(std::string str, float* value)
	{
		if (editPropMap.contains(str))
		{
			if (editPropMap[str].edited)
				*value = *(float*)editPropMap[str].srcPtr;
		}
		else
		{
			ImGui::ColorEdit4(str.c_str(), value);					//ImGui item
			StoreGroupProp(str, value);
		}
	}

	///////////////////////////////////////////////////////////////////////////

	void Int(std::string str, int* value)
	{
		if (editPropMap.contains(str))
		{
			if (editPropMap[str].edited)
				*value = *(int*)editPropMap[str].srcPtr;
		}
		else
		{
			ImGui::DragInt(str.c_str(), value);						//ImGui item
			StoreGroupProp(str, value);
		}
	}

	void Int2(std::string str, int* value)
	{
		if (editPropMap.contains(str))
		{
			if (editPropMap[str].edited)
				*value = *(int*)editPropMap[str].srcPtr;
		}
		else
		{
			ImGui::DragInt2(str.c_str(), value);					//ImGui item
			StoreGroupProp(str, value);
		}
	}

	void IntSlider(std::string str, int* value, int min, int max)
	{
		if (editPropMap.contains(str))
		{
			if (editPropMap[str].edited)
				*value = *(int*)editPropMap[str].srcPtr;
		}
		else
		{
			ImGui::SliderInt(str.c_str(), value, min, max);					//ImGui item
			StoreGroupProp(str, value);
		}
	}

	///////////////////////////////////////////////////////////////////////////

	void String(std::string str, std::string* value)
	{
		if (editPropMap.contains(str))
		{
			if (editPropMap[str].edited)
				*value = *(std::string*)editPropMap[str].srcPtr;
		}
		else
		{
			InputText(str.c_str(), value, 2048, 0, 0);				//ImGui item
			StoreGroupProp(str, value);
		}
	}
}