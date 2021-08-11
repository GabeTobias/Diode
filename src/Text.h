#pragma once


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <map>

#include <ft2build.h>
#include <iostream>
#include FT_FREETYPE_H  

#include "Shader.h"
#include "Window.h"

extern Window* window;

namespace Text 
{
	/// Holds all state information relevant to a character as loaded using FreeType
	struct TXTCHAR {
		unsigned int TextureID; // ID handle of the glyph texture
		glm::ivec2   Size;      // Size of glyph
		glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
		unsigned int Advance;   // Horizontal offset to advance to next glyph
	};

	//Character map for the current font
	inline std::map<GLchar, TXTCHAR> Characters;
	inline unsigned int textVAO, textVBO;

	//Loaded Font information
	inline FT_Library ft;
	inline FT_Face face;

	//Rendering information
	inline Shader textShader;

	//Loads the Robot Mono font into memory
	inline void InitFont()
	{
		//Load Text Shader
		textShader.load("resources/shaders/textFrag.glsl", "resources/shaders/textVert.glsl");

		//Init FT
		if (FT_Init_FreeType(&ft))
		{
			std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		}

		//Try to Load Font
		if (FT_New_Face(ft, "resources/fonts/RobotoMono-Regular.ttf", 0, &face))
		{
			std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		}
		// Unpack loaded font into memory
		else
		{
			// set size to load glyphs as
			FT_Set_Pixel_Sizes(face, 0, 128);

			// disable byte-alignment restriction
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			// load first 128 characters of ASCII set
			for (unsigned char c = 0; c < 128; c++)
			{
				// Load character glyph 
				if (FT_Load_Char(face, c, FT_LOAD_RENDER))
				{
					std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
					continue;
				}
				// generate texture
				unsigned int texture;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RED,
					face->glyph->bitmap.width,
					face->glyph->bitmap.rows,
					0,
					GL_RED,
					GL_UNSIGNED_BYTE,
					face->glyph->bitmap.buffer
				);

				// set texture options
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				// now store character for later use
				TXTCHAR character = {
					texture,
					glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
					glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
					face->glyph->advance.x
				};
				Characters.insert(std::pair<char, TXTCHAR>(c, character));
			}

			//Unbind the texture
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// configure VAO/VBO for texture quads
		// -----------------------------------
		glGenVertexArrays(1, &textVAO);
		glGenBuffers(1, &textVBO);
		glBindVertexArray(textVAO);
		glBindBuffer(GL_ARRAY_BUFFER, textVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// destroy FreeType once we're finished
		FT_Done_Face(face);
		FT_Done_FreeType(ft);
	}

	inline void DrawFontText(std::string str, glm::vec2 pos, glm::vec4 col, float scale = 1)
	{
		//Load text shader
		glUseProgram(textShader.program);

		//Pass Window Matrix and Text color to shader
		glm::mat4 p = glm::ortho(0.0f, float(window->width), 0.0f, float(window->height));
		glUniform4f(textShader.getUniform("textColor"), col.r, col.g, col.b, col.a);
		glUniformMatrix4fv(textShader.getUniform("projection"), 1, GL_FALSE, &p[0][0]);

		//Bind Font texture
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(textVAO);

		// iterate through all characters
		std::string::const_iterator c;
		for (c = str.begin(); c != str.end(); c++)
		{
			//Get current character data from font
			TXTCHAR ch = Characters[*c];

			//Calculate character position
			float xpos = pos.x + ch.Bearing.x * scale;
			float ypos = pos.y - (ch.Size.y - ch.Bearing.y) * scale;

			//Calculate character size
			float w = ch.Size.x * scale;
			float h = ch.Size.y * scale;

			// update VBO for each character
			float vertices[6][4] = {
				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos,     ypos,       0.0f, 1.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },

				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },
				{ xpos + w, ypos + h,   1.0f, 0.0f }
			};

			// render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);

			// update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, textVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

			//Unbind Array buffer
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			// render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);

			// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			pos.x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
		}

		//Unbind vertex buffer and font texture
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}