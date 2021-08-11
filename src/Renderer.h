#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>

//TODO: GABE WHY THE FUCK
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

//Utility inluces
#include "Window.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Tilemap.h"
#include "FrameBuffer.h"

#include "GUI.h"

extern Window* window;
extern Camera* camera;

namespace Render
{
	using namespace glm;

	inline Shader defaultShader;
	inline Shader checkerShader;

	inline int triGeo;
	inline int quadGeo;

	inline vec2 gridOffset = vec2(0.5);

	inline int GenerateTri()
	{
		float vertices[] = {
			-0.5f, 0.0f,  0.5f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // left  
			 0.5f, 0.0f,  0.5f,  0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // right 
			 0.0f, 0.0f, -0.5f,  0.0f, 0.0f, -1.0f, 0.5f, 1.0f // top   
		};

		unsigned int VBO, VAO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//Describe position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		return VAO;
	}
	inline int GenerateQuad()
	{
		unsigned int geo;

		float vertices[] = {
			// positions          
			 0.5f,  0.5f, 0.0f,   0.0f, 0.0f, -1.0f,   1.0f, 1.0f,
			 0.5f, -0.5f, 0.0f,   0.0f, 0.0f, -1.0f,   1.0f, 0.0f,
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
			-0.5f,  0.5f, 0.0f,   0.0f, 0.0f, -1.0f,   0.0f, 1.0f
		};

		unsigned int indices[]
		{
			0, 2, 1,
			0, 3, 2,
		};

		unsigned int VBO, EBO;

		//Generate GPU Buffers
		glGenVertexArrays(1, &geo);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		//Select Buffer to user
		glBindVertexArray(geo);

		//Bind Vertex positions
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//Bind Quad indeces
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		//Describe position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		return geo;
	}

	inline int GenerateTilemapGeo(std::vector<std::vector<short int>> map, int width, int height)
	{
		unsigned int geo;

		std::vector<float> data;
		std::vector<unsigned int> index;

		unsigned int indexOffset = 0;

		for (int x0 = 0; x0 < width; x0++)
		{
			for (int y0 = 0; y0 < height; y0++)
			{
				//Skip empty values
				if (map[x0][y0] != 0) 
				{

					//TODO: THIS DOES NOT TAKE OFFSET INTO ACOUNT
					float vertices[] = {
						// positions          
						float(x0) + 0.5f, float(y0) + 0.5f, 0.0f,   0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
						float(x0) + 0.5f, float(y0) + -0.5f, 0.0f,   0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
						float(x0) + -0.5f, float(y0) + -0.5f, 0.0f,   0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
						float(x0) + -0.5f, float(y0) + 0.5f, 0.0f,   0.0f, 0.0f, -1.0f,  0.0f, 1.0f
					};

					unsigned int indices[]
					{
						0, 2, 1,
						0, 3, 2,
					};

					//Add Data
					for (int i = 0; i < 32; i++)
						data.push_back(vertices[i]);

					//Add Indices
					for (int i = 0; i < 6; i++)
						index.push_back(indices[i] + indexOffset);

					//Offset Index
					indexOffset += 4;
				}
			}
		}

		unsigned int VBO, EBO;

		//Generate GPU Buffers
		glGenVertexArrays(1, &geo);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		//Select Buffer to user
		glBindVertexArray(geo);

		//Bind Vertex positions
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);

		//Bind Quad indeces
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(unsigned int), &index[0], GL_STATIC_DRAW);

		//Describe position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		return geo;
	}

	inline void DrawCircle(vec3 position = vec3(), vec2 size = vec2(1), vec4 color = glm::vec4(1), bool outline = false, float weight = 0.05f)
	{
		glBindBuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, window->width, window->height);

		//Load the default shader
		glUseProgram(defaultShader.program);

		//Replace with getMatrix from TRANSFORM class
		glm::mat4 model = glm::translate(glm::mat4(1.0f), vec3(position.x, position.y, camera->viewDistance - position.z)) * glm::toMat4(glm::quat(glm::vec3(0))) * glm::scale(glm::mat4(1.0f), vec3(size, 1));

		//Pass Transform Uniforms to shader
		defaultShader.setUniform("M", model);
		defaultShader.setUniform("V", camera->view);
		defaultShader.setUniform("P", camera->projection);

		//Pass Camera Position
		defaultShader.setUniform("viewPos", camera->position);

		//Texture Matrix
		defaultShader.setUniform("TexScale", mat4(1));
		defaultShader.setUniform("TexSize", vec2(1));
		defaultShader.setUniform("TexUV", vec4(0, 0, 1, 1));

		//Pass Color
		defaultShader.setUniform("color", color);
		defaultShader.setUniform("outline", (outline) ? 1 : 0);
		defaultShader.setUniform("weight", weight);
		defaultShader.setUniform("textured", 0);
		defaultShader.setUniform("ellipse", 1);

		//Bind and Render
		glBindVertexArray(quadGeo);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	inline void DrawQuad(vec3 position = vec3(), float rotation = 0, vec2 size = vec2(1), vec4 color = glm::vec4(1), bool outline = false, float weight = 0.05f)
	{
		glBindBuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, window->width, window->height);

		//Load the default shader
		glUseProgram(defaultShader.program);

		//Replace with getMatrix from TRANSFORM class
		glm::mat4 model = glm::translate(glm::mat4(1.0f), vec3(position.x, position.y, camera->viewDistance - position.z)) * glm::toMat4(glm::quat(glm::vec3(0, 0, rotation))) * glm::scale(glm::mat4(1.0f), vec3(size,1));

		//Pass Transform Uniforms to shader
		defaultShader.setUniform("M", model);
		defaultShader.setUniform("V", camera->view);
		defaultShader.setUniform("P", camera->projection);

		//Pass Camera Position
		defaultShader.setUniform("viewPos", camera->position);

		//Texture Matrix
		defaultShader.setUniform("TexScale", mat4(1));
		defaultShader.setUniform("TexSize", vec2(1));
		defaultShader.setUniform("TexUV", vec4(0,0,1,1));

		//Pass Color
		defaultShader.setUniform("color", color);
		defaultShader.setUniform("outline", (outline) ? 1:0);
		defaultShader.setUniform("weight", weight);
		defaultShader.setUniform("textured", 0);
		defaultShader.setUniform("ellipse", 0);

		//Bind and Render
		glBindVertexArray(quadGeo);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	inline void DrawTri (vec3 position = vec3(), float rotation = 0, vec2 size = vec2(1), vec4 color = glm::vec4(1))
	{
		glBindBuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, window->width, window->height);

		//Load the default shader
		glUseProgram(defaultShader.program);

		//Replace with getMatrix from TRANSFORM class
		glm::mat4 model = glm::translate(glm::mat4(1.0f), vec3(position.x, position.y, camera->viewDistance - position.z)) * glm::toMat4(glm::quat(glm::vec3(3.14 / 2.0, 0, rotation))) * glm::scale(glm::mat4(1.0f), glm::vec3(size.x, 1, size.y));

		//Pass Transform Uniforms to shader
		defaultShader.setUniform("M", model);
		defaultShader.setUniform("V", camera->view);
		defaultShader.setUniform("P", camera->projection);

		//Pass Camera Position
		defaultShader.setUniform("viewPos", camera->position);

		//Texture Matrix
		defaultShader.setUniform("TexScale", mat4(1));
		defaultShader.setUniform("TexSize", vec2(1));
		defaultShader.setUniform("TexUV", vec4(0, 0, 1, 1));
		defaultShader.setUniform("ellipse", 0);

		//Pass Color
		defaultShader.setUniform("color", color);
		defaultShader.setUniform("outline", 0);
		defaultShader.setUniform("textured", 0);

		//Bind and Render
		glBindVertexArray(triGeo);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	inline void DrawSprite(vec3 position, Texture* texture, vec4 UV, float rotation = 0, vec2 size = vec2(1), vec4 color = vec4(1))
	{
		//Load the default shader
		glUseProgram(defaultShader.program);

		//Replace with getMatrix from TRANSFORM class
		glm::mat4 model = glm::translate(glm::mat4(1.0f), vec3(position.x, position.y, camera->viewDistance - position.z)) * glm::toMat4(glm::quat(glm::vec3(0, 0, rotation))) * glm::scale(glm::mat4(1.0f), vec3(size,1));

		//Pass Transform Uniforms to shader
		defaultShader.setUniform("M", model);
		defaultShader.setUniform("V", camera->view);
		defaultShader.setUniform("P", camera->projection);
		
		//Calculate Texture Matrix
		float aspect = float(UV.z) / float(UV.w);
		mat4 texScale = glm::scale(glm::mat4(1.0f), vec3(aspect, -1.0, 1));			//Y is -1 to flip the y axis

		//Texture Matrix
		defaultShader.setUniform("TexScale", texScale);
		defaultShader.setUniform("TexSize", vec2(texture->width, texture->height));
		defaultShader.setUniform("TexUV", UV);

		//Pass Camera Position
		defaultShader.setUniform("viewPos", camera->position);

		//Pass Color
		defaultShader.setUniform("color", color);
		defaultShader.setUniform("outline", 0);
		defaultShader.setUniform("textured", 1);
		defaultShader.setUniform("ellipse", 0);

		//Bind Texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->texture_ptr);

		//Bind and Render
		glBindVertexArray(quadGeo);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	inline void DrawFrame(vec3 position, FrameBuffer* frame, vec2 size)
	{
		//Load the default shader
		glUseProgram(defaultShader.program);

		//Replace with getMatrix from TRANSFORM class
		glm::mat4 model = glm::translate(glm::mat4(1.0f), vec3(position.x, position.y, camera->viewDistance - position.z)) * glm::toMat4(glm::quat(glm::vec3(0))) * glm::scale(glm::mat4(1.0f), vec3(size, 1));

		//Pass Transform Uniforms to shader
		defaultShader.setUniform("M", model);
		defaultShader.setUniform("V", camera->view);
		defaultShader.setUniform("P", camera->projection);

		//Calculate Texture Matrix
		float aspect = float(frame->width) / float(frame->height);
		mat4 texScale = glm::scale(glm::mat4(1.0f), vec3(1.0, -1.0, 1));			//Y is -1 to flip the y axis

		//Texture Matrix
		defaultShader.setUniform("TexScale", texScale);
		defaultShader.setUniform("TexSize", vec2(frame->width, frame->height));
		defaultShader.setUniform("TexUV", vec4(0,0, frame->width, frame->height));

		//Pass Camera Position
		defaultShader.setUniform("viewPos", camera->position);

		//Pass Color
		defaultShader.setUniform("color", vec4(1));
		defaultShader.setUniform("outline", 0);
		defaultShader.setUniform("textured", 1);
		defaultShader.setUniform("ellipse", 0);

		//Bind Texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, frame->colorBuffer);

		//Bind and Render
		glBindVertexArray(quadGeo);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	inline void DrawMap(unsigned int geo, int count, vec3 position = vec3(), float rotation = 0, vec2 size = vec2(1), vec4 color = glm::vec4(1), bool outline = false, float weight = 0.05f)
	{
		glBindBuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, window->width, window->height);

		//Load the default shader
		glUseProgram(defaultShader.program);

		//Replace with getMatrix from TRANSFORM class
		glm::mat4 model = glm::translate(glm::mat4(1.0f), vec3(position.x, position.y, camera->viewDistance - position.z)) * glm::toMat4(glm::quat(glm::vec3(0, 0, rotation))) * glm::scale(glm::mat4(1.0f), vec3(size, 1));

		//Pass Transform Uniforms to shader
		defaultShader.setUniform("M", model);
		defaultShader.setUniform("V", camera->view);
		defaultShader.setUniform("P", camera->projection);

		//Pass Camera Position
		defaultShader.setUniform("viewPos", camera->position);

		//Texture Matrix
		defaultShader.setUniform("TexScale", mat4(1));
		defaultShader.setUniform("TexSize", vec2(1));
		defaultShader.setUniform("TexUV", vec4(0, 0, 1, 1));

		//Pass Color
		defaultShader.setUniform("color", color);
		defaultShader.setUniform("outline", (outline) ? 1 : 0);
		defaultShader.setUniform("weight", weight);
		defaultShader.setUniform("textured", 0);
		defaultShader.setUniform("ellipse", 0);

		//Bind and Render
		glBindVertexArray(geo);
		glDrawElements(GL_TRIANGLES, count * 6, GL_UNSIGNED_INT, 0);
	}


	inline void DrawCheck(int size)
	{
		//Load the default shader
		glUseProgram(checkerShader.program);

		//Replace with getMatrix from TRANSFORM class
		glm::mat4 model = glm::translate(glm::mat4(1.0f), vec3(gridOffset, 1)) * glm::toMat4(glm::quat(glm::vec3(0, 0, 0))) * glm::scale(glm::mat4(1.0f), vec3(size));

		//Pass Transform Uniforms to shader
		checkerShader.setUniform("M", model);
		checkerShader.setUniform("V", camera->view);
		checkerShader.setUniform("P", camera->projection);
		
		checkerShader.setUniform("size", int(size/TILESCALE));

		//Bind and Render
		glBindVertexArray(quadGeo);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	inline void Clear() 
	{
		glBindBuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, window->width, window->height);

		glClearColor(5 / 255.0, 5 / 255.0, 5 / 255.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	}

	inline void Init()
	{
		//Create Shaders
		defaultShader.load("resources/shaders/frag.glsl", "resources/shaders/vert.glsl");
		checkerShader.load("resources/shaders/checkerFrag.glsl", "resources/shaders/checkerVert.glsl");

		//Generate Primitives
		triGeo = GenerateTri();
		quadGeo = GenerateQuad();

		GUI::Init();

		//Setup Opengl transparency
		{
			//Enable depth testing
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);

			// Enable blending
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			// Set clear color
			glClearColor(5 / 255.0, 5 / 255.0, 5 / 255.0, 1.0);
		}
	}

	inline void ReleadShaders()
	{
		defaultShader.load("resources/shaders/frag.glsl", "resources/shaders/vert.glsl");
		checkerShader.load("resources/shaders/checkerFrag.glsl", "resources/shaders/checkerVert.glsl");
	}
}