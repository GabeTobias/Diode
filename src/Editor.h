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

#include "Undo.h"
#include "Props.h"
#include "Manip.h"

extern Window* window;

namespace Editor 
{
	inline World::Entity* selectedEntt = nullptr;
	inline bool TileEditing = false;

	inline std::map<std::string, Texture*> icons;

	inline void Init()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window->window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		style.ScaleAllSizes(0.9f);
		style.ScrollbarSize = 12;
		style.WindowRounding = 3;

		File::BindConstructors();

		//Load Icons
		{
			Texture* collider = new Texture();
			collider->LoadTexture("resources/textures/icons/collider.png");
			icons["Collidable"] = collider;

			Texture* destruct = new Texture();
			destruct->LoadTexture("resources/textures/icons/destruct.png");
			icons["Destructable"] = destruct;

			Texture* move = new Texture();
			move->LoadTexture("resources/textures/icons/move.png");
			icons["Movable"] = move;
		
			Texture* pan = new Texture();
			pan->LoadTexture("resources/textures/icons/pannable.png");
			icons["Pannable"] = pan;
		
			Texture* sound = new Texture();
			sound->LoadTexture("resources/textures/icons/sound.png");
			icons["Hearable"] = sound;
		
			Texture* tile = new Texture();
			tile->LoadTexture("resources/textures/icons/tilemap.png");
			icons["Tilemap"] = tile;

		}

		//Log Editor State
		printf("Editor Started \n");
	}

	inline void Close()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	inline std::string popupDesc= "";
	inline std::string popupHeader = "";
	inline float popupAlpha = 0;

	inline void PopUp(float time, std::string header, std::string desc)
	{
		popupHeader = header;
		popupDesc = desc;
		popupAlpha = time;
	}


	inline void UI()
	{
		using namespace World;

		// feed inputs to dear imgui, start new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Menu Bar
		{
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Save")) 
					{
						selectedEntt = nullptr;									//Reset Selection
						File::SaveScene("resources/scenes/test.json");			//Save World to File
						PopUp(2, "Saved", "resources/scenes/test.json");		//Poopup Feedback
					}
					if (ImGui::MenuItem("Load"))
					{
						selectedEntt = nullptr;									//Reset Selection
						File::LoadScene("resources/scenes/test.json");			//Load World from File
						PopUp(2, "Loaded", "resources/scenes/test.json");		//Popup Feedback
					}

					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Edit"))
				{
					if (ImGui::BeginMenu("Add"))
					{
						//Entt Factory
						for (std::pair<std::string, ConstructEntt> construct : File::EnttTable)
						{
							if (ImGui::MenuItem(construct.first.c_str())) 
							{
								//Construct Entt
								Entity* ent = construct.second();
								ent->position = glm::ivec2(camera->position);								
								AddEntity(ent);

								//Select Entt
								selectedEntt = ent;

								//Store Action
								Undo::Store();
								PopUp(2, "Added", ent->className.c_str());
							}
						}

						//Component Factory
						if (selectedEntt != nullptr)
						{
							ImGui::Spacing();
							ImGui::Separator();
							ImGui::Spacing();

							for (std::pair<std::string, ConstructComp> construct : File::CompTable)
							{
								if (ImGui::MenuItem(construct.first.c_str())) 
								{
									//Create Component
									Component* comp = construct.second(selectedEntt);
									selectedEntt->AttachComponent(comp);

									//Store Action
									Undo::Store();

									//Popup Feedback
									PopUp(2, "Added", comp->className.c_str());
								}
							}
						}

						ImGui::EndMenu();
					}

					if (selectedEntt != nullptr && ImGui::MenuItem("Delete"))
					{
						Undo::Store();								//Store Undo Action
						DeleteEntity(selectedEntt);									//Delete Selected Entt
						PopUp(2, "Deleted", selectedEntt->className.c_str());		//Popup feedback
					}


					ImGui::EndMenu();
				}

				ImGui::EndMainMenuBar();
			}
		}

		//Camera Movement
		{
			//Store mouse default position
			static glm::vec2 prevMouse = Input::GetMouseWorldPosition();
			
			//Check for middle mouse input
			if (Input::GetMousebutton(GLFW_MOUSE_BUTTON_MIDDLE)) 
			{
				//Update temp position based on mouse movement
				static glm::vec2 tempPosition = Input::GetMouseWorldPosition();
				tempPosition -= (Input::GetMouseWorldPosition() - prevMouse) * 3.f;
				
				//Move camera to Temp position
				camera->position = glm::lerp(camera->position, tempPosition, 0.25f);
			}
			//Store Updated Mouse position
			prevMouse = Input::GetMouseWorldPosition();

			//Apply Change in mouse scroll to camera size for zoom
			camera->size += (Input::scrollDelta.y * 0.04) * camera->size;
			camera->size = glm::clamp(camera->size, 5.f, 1000.f);
		}

		//File Load/Save
		{
			//This currently only loads and saves a preditermined scene
			//from file. IDK if I actually want to change this as the design
			//of the game doesn't really need multiple scenes

			// Ctrl + S | Key Binding
			if (Input::GetKey(GLFW_KEY_LEFT_CONTROL) && Input::GetKeyDown(GLFW_KEY_S))
			{
				TileEditing = false;									//Turn Off Tile Edit Flag
				selectedEntt = nullptr;									//Clear Selection
				File::SaveScene("resources/scenes/test.json");			//Save World to file
				PopUp(2, "Saved", "resources/scenes/test.json");		//Popup Feedback
			}

			// Ctrl + O | Key Binding
			if (Input::GetKey(GLFW_KEY_LEFT_CONTROL) && Input::GetKeyDown(GLFW_KEY_O)) 
			{
				TileEditing = false;									//Turn Off Tile Edit Flag
				selectedEntt = nullptr;									//Clear Selection
				File::LoadScene("resources/scenes/test.json");			//Load World from file
				PopUp(2, "Loaded", "resources/scenes/test.json");		//Popup Feedback
			}
		}

		//Copy/Paste
		{
			//Copied Entity Pointer
			static Entity* copiedEntt;

			// Ctrl + C | Key Binding
			if (Input::GetKey(GLFW_KEY_LEFT_CONTROL) && Input::GetKeyDown(GLFW_KEY_C))
			{
				copiedEntt = selectedEntt;								//Store Selected
				PopUp(2, "Copied", selectedEntt->name);					//Popup Feedback
			}

			// Ctrl + D | Key Binding
			if (Input::GetKey(GLFW_KEY_LEFT_CONTROL) && Input::GetKeyDown(GLFW_KEY_D))
			{
				//Copy Properties
				nlohmann::json data = File::SerializeEntt(selectedEntt);
				Entity* entt = File::DeserializeEntt(nullptr, data);

				//Add to World
				entt->position = glm::ivec2(camera->position);
				World::AddEntity(entt);

				//Store Undo
				Undo::Store();

				//Popup Feedback
				PopUp(2, "Duplicated", entt->name);
			}

			// Ctrl + V | Key Binding
			if (Input::GetKey(GLFW_KEY_LEFT_CONTROL) && Input::GetKeyDown(GLFW_KEY_V))
			{
				//Create Entt
				Entity* PastedEnt = File::EnttTable[copiedEntt->className]();
				
				//Copy Properties
				nlohmann::json data = copiedEntt->Save();
				PastedEnt->Load(data);

				//Add to World
				PastedEnt->position = glm::ivec2(camera->position);
				World::AddEntity(PastedEnt);

				//Store Undo
				Undo::Store();

				//Popup Feedback
				PopUp(2, "Pasted", PastedEnt->name);
			}
		}

		//Undo/Redo
		{
			// Ctrl + Z | Key Binding
			if (Input::GetKey(GLFW_KEY_LEFT_CONTROL) && !Input::GetKey(GLFW_KEY_LEFT_SHIFT) && Input::GetKeyDown(GLFW_KEY_Z))
			{
				//Store selection
				unsigned int selected = NAN;
				if(selectedEntt != nullptr) selected = selectedEntt->uid;

				//Undo action and create popup for result
				if(Undo::Pop()) PopUp(2, "Undo", "resources/scenes/test.json");
				else PopUp(2, "Undo", "End Of Stack");

				//Releselect Actor
				if (selectedEntt != nullptr) selectedEntt = World::GetEntityOfUID(selected);
			}

			// Ctrl + Shift + Z | Key Binding
			if (Input::GetKey(GLFW_KEY_LEFT_CONTROL) && Input::GetKey(GLFW_KEY_LEFT_SHIFT) && Input::GetKeyDown(GLFW_KEY_Z))
			{
				//Store selection
				unsigned int selected = NAN;
				if (selectedEntt != nullptr) selected = selectedEntt->uid;

				//Redo action and create popup for result
				if (Undo::Push()) PopUp(2, "Redo", "resources/scenes/test.json");
				else PopUp(2, "Redo", "End Of Stack");

				//Reselect
				if (selectedEntt != nullptr) selectedEntt = World::GetEntityOfUID(selected);
			}
		}

		//Debug Behaviours
		{
			//Reloads Shaders
			if (Input::GetKeyDown(GLFW_KEY_F2)) 
			{
				Render::ReleadShaders();
				GUI::Init();
			}

			//Prints Serialized Scene Data to console
			if (Input::GetKeyDown(GLFW_KEY_F6)) 
				printf(File::SerializeScene().dump(4).c_str());
		}

		//World Heiarchy
		{
			ImGui::Begin("World Hiearchy");

			//TODO: This is hella hefty and terribly inefficient
			//		It works for now but certainly not a long term solution
			for (Entity* e : entities)
			{
				//Find only Room objects
				if (e->className == "Room") 
				{
					//Start Dropdown list for room
					if (ImGui::TreeNodeEx(e->name.c_str(), (selectedEntt == e) ? ImGuiTreeNodeFlags_Selected:0)) 
					{
						//Get current room
						Room* room = (Room*)e;

						for (Entity* ent : entities)
						{
							//Check if entity is inside current room
							if (room->Contains(ent->position) && room != ent)
							{
								//List Entt inside dropdown
								if (ImGui::Selectable(ent->name.c_str(), selectedEntt && ent->uid == selectedEntt->uid))
								{
									selectedEntt = ent;
									TileEditing = false;
								}
							}
						}

						//Close Dropdown
						ImGui::TreePop();
					}
				}

			}

			ImGui::End();
		}

		//Viewport Selection
		{
			//Listen for viewport click events NOT touching editor
			if (Input::GetMousebutton(0) && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyWindowHovered() && !ImGui::IsAnyItemActive() && !manipHovering && !manipDragging && !TileEditing)
			{
				//Get Mouse data
				glm::vec2 position = Input::GetMouseWorldPosition();

				//Physics raycast to mouse point
				Physics::CollisionInfo info = Physics::Collides(Input::GetMouseWorldPosition(), glm::vec2(0.1f), 0U, 100);

				//Update selection
				if (info) selectedEntt = info.component->owner;
				else 
				{
					selectedEntt = nullptr;

					for (Entity* ent : entities)
					{
						if (BoxOverlaps(position, glm::vec2(0.1), (ent->position) - (ent->editorBounds / 2.f), ent->editorBounds))
						{
							selectedEntt = ent;
						}
					}
				}
			}

			//Ctrl+Backspace Delete Action
			if (Input::GetKeyDown(GLFW_KEY_BACKSPACE) && Input::GetKeyDown(GLFW_KEY_LEFT_CONTROL) && selectedEntt != nullptr && !ImGui::IsAnyItemActive())
			{
				DeleteEntity(selectedEntt);
				selectedEntt = nullptr;
				Undo::Store();
			}

			//Delete Key Delete Action
			if (Input::GetKeyDown(GLFW_KEY_DELETE) && selectedEntt != nullptr && !ImGui::IsAnyItemActive())
			{
				DeleteEntity(selectedEntt);
				selectedEntt = nullptr;
				Undo::Store();
			}
		}

		//Reset Manipulator State
		manipHovering = false;

		//Entt Editor
		if(selectedEntt != nullptr)
		{
			//Component Icons
			{
				//Get List of Attached Components
				std::vector<Component*> enttComps;
				for (int i = 0; i < components.size(); i++)
				{
					if (!components[i]->owner || components[i]->owner->uid != selectedEntt->uid) continue;
					else enttComps.push_back(components[i]);
				}

				//Draw Component Box
				if (enttComps.size() > 0 && !TileEditing)
				{
					//Calculate Screen Position
					glm::vec2 wPos = worldToViewCoordTransform(glm::vec3(selectedEntt->position, 0));
					ImGui::SetNextWindowPos({ wPos.x  + 25, wPos.y - 50 });

					//Draw Button Menu
					ImGui::Begin("##CompIcons", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
					for (int i = 0; i < enttComps.size(); i++)
					{
						if (icons[enttComps[i]->className] != nullptr)
						{
							ImGui::ImageButton((ImTextureID)icons[enttComps[i]->className]->texture_ptr, { 20,20 }, { 0,0 }, { 1,1, }, 0, { 0,0,0,1 }, {1,1,1,0.5});
							ImGui::SameLine();
						}
					}
					ImGui::End();
				}
			}

			ImGui::Begin("Properties");

			//Actor Name and ID
			ImGui::Text("%s", selectedEntt->className.c_str()); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 1, 0.25), "%d", selectedEntt->uid);

			ImGui::Spacing();

			//Begin storing changes in properties to undo actions. 
			Editor::BeginChangeGroup();

			//This currently covers both actor and component 
			//properties but may be seperated in the 
			if (!TileEditing) 
			{
				//Store Starting position of the actor
				glm::vec2 prevPosition = selectedEntt->position;

				//Actor Position Manipulator
				ImGui::DragFloat2("Position", &selectedEntt->position[0]);
				glm::vec2 newPos = Editor::Manipulator(selectedEntt->uid, selectedEntt->position);

				// Contained Entts is currently a static variable but 
				// should probably be contained and updated inside the 
				// rooms themselves

				//List of entities inside room
				static std::vector<Entity*> containedEntts;

				//Move children of Room actors
				if (selectedEntt->className == "Room")
				{
					//Calculate change in actor position
					glm::vec2 posDelta = newPos - prevPosition;

					//Get selected room
					Room* room = (Room*)selectedEntt;
					
					//Check if room is NOT being Moved
					if (!manipHovering && !manipDragging)
					{
						//Clear contained entts
						containedEntts.clear();

						//Update current room's list of contained Entts
						for (Entity* ent : entities)
						{
							if (room->Contains(ent->position))
							{
								containedEntts.push_back(ent);
							}
						}
					}

					//Check if room IS being moved
					if(manipHovering || manipDragging)
					{
						//Move Contained Entts
						for (Entity* ent : containedEntts) 
						{
							ent->position += posDelta;
						}
					}
				}

				selectedEntt->position = newPos;
			}

			//This is an extra comment because I always seem to lose 
			//track of where exactly in the code this happens

			//Selected entity's Editor Behavour
			selectedEntt->Editor();

			//Display Component Editors
			for (Component* comp : components)
			{
				if (comp->owner->uid == selectedEntt->uid) 
				{
					//BIG SPACING
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();
					ImGui::Spacing();

					//Name and ID
					ImGui::Text("%s", comp->className.c_str());
					
					if (ImGui::BeginPopupContextItem(std::to_string(comp->uid).c_str())) 
					{
						if(ImGui::MenuItem("Remove")) 
						{
							World::DeleteComponent(comp);
							ImGui::EndPopup();
							continue;
						}

						ImGui::EndPopup();
					}

					ImGui::SameLine();
					ImGui::TextColored(ImVec4(1, 1, 1, 0.25), "%d", comp->uid);

					ImGui::Spacing();

					//Components Editor
					comp->Editor();
				}
			}

			//Check if Entts props or its components props have changed
			//anf if they have store it in the undo stack
			if (Editor::EndChangeGroup()) 
				Undo::Store();

			ImGui::End();
		}

		//Popup Menu
		if (popupAlpha > 0)
		{
			//TODO: Resize window to always fit contents
			//Set Window Size and position
			ImGui::SetNextWindowPos(ImVec2(window->width - 260, window->height - 40));
			ImGui::SetNextWindowSize(ImVec2(250, 30));
			ImGui::SetNextWindowBgAlpha(popupAlpha);

			//Display Window
			ImGui::Begin("Saved", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
			ImGui::TextColored(ImVec4(1, 1, 1, popupAlpha), popupHeader.c_str()); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 1, 0.25 * popupAlpha), popupDesc.c_str());
			ImGui::End();

			//TODO: Base this off of time instead of cheezy decrement method
			//Fade Window Away
			popupAlpha -= 0.01f;
		}

		//Renderer Debug Settings Menu
		if (false) 
		{
			ImGui::Begin("Render Settings");
			ImGui::DragFloat2("Grid Offset", &Render::gridOffset[0]);
			ImGui::End();
		}

		// Render dear imgui into screen
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}