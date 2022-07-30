#include "Editor.h"
#include "Game.h"
#include "Log.h"

using namespace World;

namespace Editor 
{
	void Init()
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
		LOG("Editor Started");
	}

	void Close()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void PopUp(float time, std::string header, std::string desc)
	{
		popupHeader = header;
		popupDesc = desc;
		popupAlpha = time;
	}

	void EntityEditor(World::Entity* entt, bool root)
	{
		//This currently covers both actor and component 
		//properties but may be seperated in the 
		if (!TileEditing)
		{
			//Store Starting position of the actor
			glm::vec2 prevPosition = entt->position;

			//Actor Position Manipulator
			Float2("Position", &entt->position[0]);
			glm::vec2 newPos = Editor::Manipulator(entt->uid, entt->position);

			// Contained Entts is currently a static variable but 
			// should probably be contained and updated inside the 
			// rooms themselves

			//List of entities inside room
			static std::vector<Entity*> containedEntts;

			//Move children of Room actors
			if (entt->className == "Room")
			{
				//Calculate change in actor position
				glm::vec2 posDelta = newPos - prevPosition;

				//Get selected room
				Room* room = (Room*)entt;

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
				if (manipHovering || manipDragging)
				{
					//Move Contained Entts
					for (Entity* ent : containedEntts)
					{
						ent->position += posDelta;
					}
				}
			}

			entt->position = newPos;
		}

		//This is an extra comment because I always seem to lose 
		//track of where exactly in the code this happens

		//Selected entity's Editor Behavour
		entt->Editor();

		//Display Component Editors
		for (Component* comp : components)
		{
			if (comp->owner->uid == entt->uid)
			{
				if (root)
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
						if (ImGui::MenuItem("Remove"))
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
				}

				//Components Editor
				comp->Editor();
			}
		}
	}

	bool isRootSelection(World::Entity* entt)
	{
		return entt->uid ==selectedEntt[0]->uid;
	}

	void UI()
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
						if (currentFile == "")
						{
							currentFile = tinyfd_saveFileDialog
							(
								"Save Scene",
								"resources/scenes/",
								2,
								lFilterPatterns,
								NULL
							);
						}
						if (currentFile == "") return;

						selectedEntt.clear();				//Reset Selection
						Game::StoreGameState();
						File::SaveScene(currentFile);		//Save World to File
						PopUp(2, "Saved", currentFile);		//Poopup Feedback
					}

					if (ImGui::MenuItem("Save as..."))
					{
						currentFile = tinyfd_saveFileDialog
						(
							"Save Scene",
							"resources/scenes/",
							2,
							lFilterPatterns,
							NULL
						);
						if (currentFile == "") return;

						selectedEntt.clear();				//Reset Selection
						Game::StoreGameState();
						File::SaveScene(currentFile);		//Save World to File
						PopUp(2, "Saved", currentFile);		//Poopup Feedback
					}

					if (ImGui::MenuItem("Load"))
					{
						currentFile = tinyfd_openFileDialog
						(
							"Load Scene", 
							"resources/scenes/", 
							2, 
							lFilterPatterns, 
							NULL, 
							0
						);
						if (currentFile == "") return;
						

						selectedEntt.clear();				//Reset Selection
						File::LoadScene(currentFile);		//Load World from File

						Game::StoreGameState();
						
						PopUp(2, "Loaded", currentFile);	//Popup Feedback
					}

					if (ImGui::MenuItem("New"))
					{
						currentFile = "";

						World::ClearEntities();
						World::ClearComponents();

						Game::StoreGameState();
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
								selectedEntt.clear();
								selectedEntt.push_back(ent);

								//Store Action
								Undo::Store();
								PopUp(2, "Added", ent->className.c_str());
							}
						}

						//Component Factory
						if (selectedEntt.size())
						{
							ImGui::Spacing();
							ImGui::Separator();
							ImGui::Spacing();

							for (std::pair<std::string, ConstructComp> construct : File::CompTable)
							{
								if (ImGui::MenuItem(construct.first.c_str()))
								{
									for (Entity* ent : selectedEntt) 
									{
										//Create Component
										Component* comp = construct.second(ent);
										ent->AttachComponent(comp);
									}
									
									//Store Action
									Undo::Store();

									//Popup Feedback
									PopUp(2, "Added", construct.first.c_str());
								}
							}
						}

						ImGui::EndMenu();
					}

					if (selectedEntt.size() && ImGui::MenuItem("Delete"))
					{
						Undo::Store();											//Store Undo Action
						
						for (Entity* ent : selectedEntt)
						{
							DeleteEntity(ent);									//Delete Selected Entt
						}
						PopUp(2, "Deleted", "Selected Entities");				//Popup feedback
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
				selectedEntt.clear();									//Clear Selection
				
				if (currentFile == "")
				{
					currentFile = tinyfd_saveFileDialog
					(
						"Save Scene",
						"resources/scenes/",
						2,
						lFilterPatterns,
						NULL
					);
				}

				Game::StoreGameState();

				File::SaveScene(currentFile);							//Save World to file
				PopUp(2, "Saved", currentFile);							//Popup Feedback
			}

			// Ctrl + O | Key Binding
			if (Input::GetKey(GLFW_KEY_LEFT_CONTROL) && Input::GetKeyDown(GLFW_KEY_O))
			{
				TileEditing = false;									//Turn Off Tile Edit Flag
				selectedEntt.clear();									//Clear Selection

				currentFile = tinyfd_openFileDialog
				(
					"Load Scene",
					"resources/scenes/",
					2,
					lFilterPatterns,
					NULL,
					0
				);

				File::LoadScene(currentFile);			//Load World from file
				
				Game::StoreGameState();
				
				PopUp(2, "Loaded", currentFile);		//Popup Feedback
			}
		}

		//Copy/Paste
		{
			//Copied Entity Pointer
			static std::vector<Entity*> copiedEntt;

			// Ctrl + C | Key Binding
			if (Input::GetKey(GLFW_KEY_LEFT_CONTROL) && Input::GetKeyDown(GLFW_KEY_C))
			{
				copiedEntt = selectedEntt;								//Store Selected
				PopUp(2, "Copied", "Selected");							//Popup Feedback
			}

			// Ctrl + D | Key Binding
			if (Input::GetKey(GLFW_KEY_LEFT_CONTROL) && Input::GetKeyDown(GLFW_KEY_D))
			{
				for (Entity* ent : selectedEntt)
				{
					//Copy Properties
					nlohmann::json data = File::SerializeEntt(ent);
					Entity* entt = File::DeserializeEntt(nullptr, data);

					//Add to World
					entt->position = glm::ivec2(camera->position);
					entt->uid = -1;
					World::AddEntity(entt);
				}

				//Store Undo
				Undo::Store();

				//Popup Feedback
				PopUp(2, "Duplicated", "Selected");
			}

			// Ctrl + V | Key Binding
			if (Input::GetKey(GLFW_KEY_LEFT_CONTROL) && Input::GetKeyDown(GLFW_KEY_V))
			{
				for (Entity* ent : copiedEntt)
				{
					//Create Entt
					Entity* PastedEnt = File::EnttTable[ent->className]();

					//Copy Properties
					nlohmann::json data = ent->Save();
					PastedEnt->Load(data);

					//Add to World
					PastedEnt->position = glm::ivec2(camera->position);
					World::AddEntity(PastedEnt);
				}

				//Store Undo
				Undo::Store();

				//Popup Feedback
				PopUp(2, "Pasted", "Clipboard Entities");
			}
		}

		//Undo/Redo
		{
			// Ctrl + Z | Key Binding
			if (Input::GetKey(GLFW_KEY_LEFT_CONTROL) && !Input::GetKey(GLFW_KEY_LEFT_SHIFT) && Input::GetKeyDown(GLFW_KEY_Z))
			{
				//Store selection
				std::vector<unsigned int> selected;
				if (selectedEntt.size())
				{
					for (Entity* ent : selectedEntt)
					{
						selected.push_back(ent->uid);
					}
				}

				//Undo action and create popup for result
				if (Undo::Pop()) PopUp(2, "Undo", "resources/scenes/test.json");
				else PopUp(2, "Undo", "End Of Stack");

				//Releselect Actor
				if (selectedEntt.size()) 
				{
					selectedEntt.clear();

					for (unsigned int uid : selected)
					{
						selectedEntt.push_back(World::GetEntityOfUID(uid));
					}
				}
			}

			// Ctrl + Shift + Z | Key Binding
			if (Input::GetKey(GLFW_KEY_LEFT_CONTROL) && Input::GetKey(GLFW_KEY_LEFT_SHIFT) && Input::GetKeyDown(GLFW_KEY_Z))
			{
				//Store selection
				std::vector<unsigned int> selected;
				if (selectedEntt.size())
				{
					for (Entity* ent : selectedEntt)
					{
						selected.push_back(ent->uid);
					}
				}

				//Redo action and create popup for result
				if (Undo::Push()) PopUp(2, "Redo", "resources/scenes/test.json");
				else PopUp(2, "Redo", "End Of Stack");

				//Releselect Actor
				if (selectedEntt.size())
				{
					selectedEntt.clear();

					for (unsigned int uid : selected)
					{
						selectedEntt.push_back(World::GetEntityOfUID(uid));
					}
				}
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
					if (ImGui::TreeNodeEx(e->name.c_str(), isSelected(e) ? ImGuiTreeNodeFlags_Selected : 0))
					{
						//Get current room
						Room* room = (Room*)e;

						for (Entity* ent : entities)
						{
							//Check if entity is inside current room
							if (room->Contains(ent->position) && room != ent)
							{
								//List Entt inside dropdown
								if (ImGui::Selectable(ent->name.c_str(), selectedEntt.size() && isSelected(ent)))
								{
									if(!Input::GetKey(GLFW_KEY_LEFT_SHIFT) && !Input::GetKey(GLFW_KEY_RIGHT_SHIFT))
										selectedEntt.clear();

									selectedEntt.push_back(ent);
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
			if (Input::GetMouseButtonDown(0) && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyWindowHovered() && !ImGui::IsAnyItemActive() && !manipHovering && !manipDragging && !TileEditing && !marqueeDragging)
			{
				//Get Mouse data
				glm::vec2 position = Input::GetMouseWorldPosition();

				//Physics raycast to mouse point
				Physics::CollisionInfo info = Physics::Collides(Input::GetMouseWorldPosition(), glm::vec2(0.1f), 0U, 100);

				//Update selection
				if (info)
				{
					if (!Input::GetKey(GLFW_KEY_LEFT_SHIFT) && !Input::GetKey(GLFW_KEY_RIGHT_SHIFT))
						selectedEntt.clear();
					
					selectedEntt.push_back(info.component->owner);
				}
				else
				{
					if (!Input::GetKey(GLFW_KEY_LEFT_SHIFT) && !Input::GetKey(GLFW_KEY_RIGHT_SHIFT))
						selectedEntt.clear();

					for (Entity* ent : entities)
					{
						if (BoxOverlaps(position, glm::vec2(0.1), (ent->position) - (ent->editorBounds / 2.f), ent->editorBounds))
						{
							selectedEntt.push_back(ent);
						}
					}
				}
			}

			//Ctrl+Backspace Delete Action
			if (Input::GetKeyDown(GLFW_KEY_BACKSPACE) && Input::GetKeyDown(GLFW_KEY_LEFT_CONTROL) && selectedEntt.size() && !ImGui::IsAnyItemActive())
			{
				for(Entity* entt : selectedEntt)
					DeleteEntity(entt);
				
				selectedEntt.clear();
				Undo::Store();
			}

			//Delete Key Delete Action
			if (Input::GetKeyDown(GLFW_KEY_DELETE) && selectedEntt.size() && !ImGui::IsAnyItemActive())
			{
				for (Entity* entt : selectedEntt)
					DeleteEntity(entt);
				
				selectedEntt.clear();
				Undo::Store();
			}
		}

		//Reset Manipulator State
		manipHovering = false;

		//Entt Editor
		if (selectedEntt.size())
		{
			std::map<std::string, bool> compMap;

			//Calculate Screen Position
			glm::vec2 wPos = worldToViewCoordTransform(glm::vec3(selectedEntt[0]->position, 0));
			ImGui::SetNextWindowPos({ wPos.x + 25, wPos.y - 50 });

			//Draw Button Menu
			ImGui::Begin("##CompIcons", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

			//Component Icons
			for (Entity* entt : selectedEntt)
			{
				//Get List of Attached Components
				std::vector<Component*> enttComps;
				for (int i = 0; i < components.size(); i++)
				{
					if (!components[i]->owner || components[i]->owner->uid != entt->uid) continue;
					else enttComps.push_back(components[i]);
				}

				//Draw Component Box
				if (enttComps.size() > 0 && !TileEditing)
				{
					for (int i = 0; i < enttComps.size(); i++)
					{
						if (compMap.contains(enttComps[i]->className)) continue;

						if (icons[enttComps[i]->className] != nullptr)
						{
							ImGui::ImageButton((ImTextureID)icons[enttComps[i]->className]->texture_ptr, { 20,20 }, { 0,0 }, { 1,1, }, 0, { 0,0,0,1 }, { 1,1,1,0.5 });
							ImGui::SameLine();

							compMap[enttComps[i]->className] = true;
						}
					}
				}
			}
			ImGui::End();


			//Begin storing changes in properties to undo actions. 
			Editor::BeginChangeGroup();

			ImGui::Begin("Properties");

			//Actor Name and ID
			ImGui::Text("%s", selectedEntt[0]->className.c_str()); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 1, 0.25), "%d", selectedEntt[0]->uid);

			ImGui::Spacing();

			for (int i = 0; i < selectedEntt.size(); i++) 
			{
				Entity* entt = selectedEntt[i];
				EntityEditor(entt, i == 0);
			}

			ImGui::End();

			//Check if Entts props or its components props have changed
			//anf if they have store it in the undo stack
			if (Editor::EndChangeGroup())
				Undo::Store();
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

		Marquee();

		// Render dear imgui into screen
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	bool isSelected(Entity* entt)
	{
		for (Entity* ent : selectedEntt)
		{
			if (ent->uid == entt->uid)
				return true;
		}

		return false;
	}

}