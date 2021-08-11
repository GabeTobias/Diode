#include "Bitmap.h"
#include "Shader.h"

using namespace glm;

Bitmap::Bitmap(int width, int height) : FrameBuffer(width, height){}

void Bitmap::Initialize()
{
	FrameBuffer::Init();

	if (quadGeo == 0)
	{
		quadGeo = GenerateQuad();
	}

	if (bitmapShader == nullptr) 
	{
		bitmapShader = new Shader();
		bitmapShader->load("resources/shaders/bitmapFrag.glsl", "resources/shaders/bitmapVert.glsl");
	}
}

void Bitmap::Unbind()
{
	FrameBuffer::Unbind();
}

int Bitmap::GenerateQuad()
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

void Bitmap::DrawQuad(glm::vec2 pos, glm::vec2 size, glm::vec4 col, float radius)
{
	//Bind to main Frame
	glViewport(0, 0, width, height); // Render on the whole framebuffer, complete from the lower left corner to the upper right

	//Bind UI shader
	glUseProgram(bitmapShader->program);

	//Pass Panel properties to shader
	bitmapShader->setUniform("color", glm::vec4(col.r, col.g, col.b, col.a));
	bitmapShader->setUniform("size", glm::vec2(size.x, size.y));
	bitmapShader->setUniform("pos", glm::vec2(pos.x, pos.y));
	bitmapShader->setUniform("edgeRadius", radius);

	//Pass Texture Information
	glUniform1i(bitmapShader->getUniform("useTexture"), 0);

	//Pass Window transformation to shader
	bitmapShader->setUniform("projection", getProjection());

	//Draw Defualt Quad
	glBindVertexArray(quadGeo);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

glm::mat4 Bitmap::getProjection()
{
	return glm::ortho(0.0f, float(width), float(height), 0.0f);
}
