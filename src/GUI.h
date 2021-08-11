#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Window.h"

#include "Text.h"

extern Window* window;

namespace GUI 
{
	inline unsigned int quadGeo;

	inline Shader uiShader;

	inline int Quad()
	{
		// set up vertex data (and buffer(s)) and configure vertex attributes
		// ------------------------------------------------------------------
		float vertices[] = {
			// positions          // colors           // texture coords
			 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
			 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
			-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
		};

		unsigned int indices[] = {
			0, 1, 3, // first triangle
			1, 2, 3  // second triangle
		};

		unsigned int VBO, VAO, EBO;

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// texture coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		return VAO;
	}

	inline void Init()
	{
		//Load GUI Shaders
		uiShader.load("resources/shaders/uiFrag.glsl", "resources/shaders/uiVert.glsl");

		Text::InitFont();

		quadGeo = Quad();
	}

	inline void DrawTextUI(std::string str, glm::vec2 pos, float scale, glm::vec4 col)
	{
		//Bind to main Frame
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, window->width, window->height); // Render on the whole framebuffer, complete from the lower left corner to the upper right

		Text::DrawFontText(str, { pos.x, window->height - pos.y }, col, scale / 100);
	}

	inline std::vector<std::string> BreakString(std::string str, std::string delimiter) 
	{
		size_t pos = 0;
		std::string token;
		std::vector<std::string> listing;
		while ((pos = str.find(delimiter)) != std::string::npos) {
			token = str.substr(0, pos);
			listing.push_back(token);
			str.erase(0, pos + delimiter.length());
		}
		listing.push_back(str.substr(0, pos));

		return listing;
	}

	inline glm::ivec2 DrawWrappedTextUI(std::string str, glm::vec2 pos, float scale, float width, glm::vec4 col)
	{
		//Bind to main Frame
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, window->width, window->height); // Render on the whole framebuffer, complete from the lower left corner to the upper right

		//Break Text into words
		std::vector<std::string> words = BreakString(str, " ");

		//Break words into lines
		std::string currentLine = "";
		int lineLength = 0;
		int lineCount = 0;
		int maxLineLength = 0;
		for (int i = 0; i < words.size(); i++) 
		{
			//Start new Line
			if (lineLength + words[i].length() > width) 
			{
				//Draw Current Line
				Text::DrawFontText(currentLine, { pos.x, window->height - (pos.y + (20*lineCount)) }, col, scale / 100);

				if (lineLength > maxLineLength) maxLineLength = lineLength;

				//Reset Line Data
				currentLine = "";
				lineLength = 0;

				lineCount++;
			}

			//Add Word to line
			currentLine += words[i];
			currentLine += " ";
			lineLength += words[i].length() + 1;
		}

		//Draw Current Line
		Text::DrawFontText(currentLine, { pos.x, window->height - (pos.y + (20 * lineCount)) }, col, scale / 100);
	
		return { (maxLineLength == 0) ? currentLine.length() : maxLineLength, lineCount + 1 };
	}

	inline void DrawBoxUI(glm::vec2 pos, glm::vec2 size, glm::vec4 col, float radius, float stroke = 0, glm::vec4 strokeCol = glm::vec4(1))
	{
		//Bind to main Frame
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, window->width, window->height); // Render on the whole framebuffer, complete from the lower left corner to the upper right

		//Bind UI shader
		glUseProgram(uiShader.program);

		//Pass Panel properties to shader
		uiShader.setUniform("color", glm::vec4(col.r, col.g, col.b, col.a));
		uiShader.setUniform("size", glm::vec2(size.x, size.y));
		uiShader.setUniform("pos", glm::vec2(pos.x, pos.y));
		uiShader.setUniform("edgeRadius", radius);

		uiShader.setUniform("stroke", stroke);
		uiShader.setUniform("strokeColor", strokeCol);

		//Pass Texture Information
		glUniform1i(uiShader.getUniform("useTexture"), 0);

		//Pass Window transformation to shader
		glm::mat4 p = glm::ortho(0.0f, float(window->width), float(window->height), 0.0f);
		uiShader.setUniform("projection", p);

		//Draw Defualt Quad
		glBindVertexArray(quadGeo);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
}