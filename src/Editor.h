#pragma once

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

#include "imgui_internal.h"
#include "ImGuizmo.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>
#include <map>

#include "Texture.h"

#include "Input.h"
#include "Coords.h"
#include "GUI.h"

#include "Window.h"
#include "World.h"

#include "EnttInf.h"
#include "File.h"

#include "tinyfiledialogs.h"

#include "Undo.h"
#include "Props.h"
#include "Manip.h"

extern Window* window;

namespace Editor
{
	inline std::vector<World::Entity*> selectedEntt;
	inline bool TileEditing = false;

	inline std::map<std::string, Texture*> icons;

	void Init();
	void Close();

	inline std::string popupDesc = "";
	inline std::string popupHeader = "";
	inline float popupAlpha = 0;

	inline std::string currentFile = "";
	inline char const* lFilterPatterns[2] = { "*.json", "*.JSON" };

	void PopUp(float time, std::string header, std::string desc);
	
	void EntityEditor(World::Entity* entt, bool root = false);
	void UI();

	bool isSelected(World::Entity* entt);
	bool isRootSelection(World::Entity* entt);
}