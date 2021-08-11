#include "Tilemap.h"
#include "Renderer.h"
#include "Input.h"
#include "ImGui.h"

#include "Editor.h"
#include "Props.h"
#include "Undo.h"

using namespace glm;

Tilemap::Tilemap(glm::vec2 size)
{
	className = "Tilemap";

	width = size.x;
	height = size.y;
	
	position = -vec2((width*TILESCALE)/2, ((height*TILESCALE)/2)-1);

	//Resize Array
	data.resize(width);
	for (int i = 0; i < width; ++i)
		data[i].resize(height);

	//Load Test Map
	for (int x0 = 0; x0 < width; x0++)
	{
		for (int y0 = 0; y0 < height; y0++)
		{
			if (x0 == 0 || y0 == 0 || x0 == width - 1 || y0 == height - 1) data[x0][y0] = 1;
			else data[x0][y0] = 0;

			if (x0 == 31 && y0 == 16) data[x0][y0] = 0;
			if (x0 == 31 && y0 == 15) data[x0][y0] = 0;

			if (x0 == 16 && y0 == 31) data[x0][y0] = 0;
			if (x0 == 15 && y0 == 31) data[x0][y0] = 0;

			if (x0 == 0 && y0 == 16) data[x0][y0] = 0;
			if (x0 == 0 && y0 == 15) data[x0][y0] = 0;
			
			if (x0 == 16 && y0 == 0) data[x0][y0] = 0;
			if (x0 == 15 && y0 == 0) data[x0][y0] = 0;
		}
	}
}

void Tilemap::GenGeo()
{
	geo = Render::GenerateTilemapGeo(data, width, height);

	//Count number of filled tiles
	geoCount = 0;
	for (int x0 = 0; x0 < width; x0++)
		for (int y0 = 0; y0 < height; y0++)
			if (data[x0][y0] != 0) geoCount++;
}

void Tilemap::Update()
{
	editing = false;
}

void Tilemap::Draw()
{
	if (editing) 
	{
		//Iterate over tiles
		for (int x0 = 0; x0 < width; x0++)
		{
			for (int y0 = 0; y0 < height; y0++)
			{
				//Draw Non-Zero tiles
				if (data[x0][y0] == 1)
				{
					Render::DrawQuad
					(
						{ owner->position.x + position.x + (x0 * TILESCALE) + 0.5, owner->position.y + position.y + (y0 * TILESCALE) + 0.5, 10 },
						0,
						{ TILESCALE,TILESCALE },									//Tile size 32 X 32
						{ 0.1, 0.1, 0.1, 1 }, 0
					);
				}
			}
		}
	} 
	else 
	{
		//Draw Generate Mesh
		Render::DrawMap(geo, geoCount, vec3(owner->position + position + vec2(TILESCALE/4.f), 10), 0, vec2(2), { 0.1, 0.1, 0.1, 1 }, 0);
	}
}

void Tilemap::Delete(){}

void Tilemap::Editor() 
{
	//Handle Map Resize
	static ivec2 dimensions = ivec2(width, height);
	Editor::Int2("Resolution", &dimensions[0]);
	if(ImGui::IsItemDeactivatedAfterEdit())
	{
		width = dimensions.x;
		height = dimensions.y;

		//Regenerate Data array
		data.resize(width);
		for (int i = 0; i < width; ++i)
			data[i].resize(height);

		//Adjust position
		position = -vec2((width * TILESCALE) / 2, ((height * TILESCALE) / 2) - 1);
	}

	//Handle Tile edit
	Editor::CheckBox("Edit Tiles", &editing);
	if (ImGui::IsItemDeactivatedAfterEdit()) 
	{
		Editor::TileEditing = !Editor::TileEditing;
		
		//Rengenerate Tilemap mesh
		if (!editing) GenGeo();
	}

	if (editing)
	{
		vec2 pos = Input::GetMouseWorldPosition();
		ivec2 point = ((pos - owner->position - position) / TILESCALE) + vec2(0.5);

		//Check if left-clicking mouse over worldspace
		static bool placeState = false;
		if (Input::GetMousebutton(0) && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyWindowHovered() && !ImGuizmo::IsOver())
		{
			//Check if clicking away from tilemap
			if (point.x < 0 || point.x >= width || point.y < 0 || point.y >= height)
			{
				editing = false;
				Editor::TileEditing = false;
			}
			else 
			{
				//Edit Tilemap
				data[point.x][point.y] = 1;
				placeState = true;
			}
		}

		//Push undo state
		if (placeState && !Input::GetMousebutton(0)) 
		{
			Undo::Store();
			placeState = false;
		}

		//Check if right-clicking mouse over worldspace
		static bool removeState = false;
		if (Input::GetMousebutton(1) && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyWindowHovered() && !ImGuizmo::IsOver())
		{
			//Check if clicking away from tilemap
			if (point.x < 0 || point.x >= width || point.y < 0 || point.y >= height)
			{
				editing = false;
				Editor::TileEditing = false;
			}
			else 
			{
				//Edit Tilemap
				data[point.x][point.y] = 0;
				removeState = true;
			}
		}

		//Push undo state
		if (removeState && !Input::GetMousebutton(1))
		{
			Undo::Store();
			placeState = false;
		}
	}

	//Draw Tilemap Bounds
	Render::DrawQuad(vec3(owner->position + position + vec2(((width * TILESCALE) / 2) - 0.5, ((height * TILESCALE) / 2) - 0.5), 5), 0, vec2(width*TILESCALE, height*TILESCALE), vec4(1), true, camera->size / 10000);
}