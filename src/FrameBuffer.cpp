#include "FrameBuffer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>


FrameBuffer::FrameBuffer(int w, int h) : width(w), height(h) 
{
	//Initialize variables
	Init();

	//Log to the console
	//printf("Create Frame Buffer \n");
}

void FrameBuffer::Init()
{
	//Create and bind the buffer id
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	// create a color attachment texture
	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthBuffer); // now actually attach it
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete! \n");

	//Unbind buffer once complete
	Unbind();
}

void FrameBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void FrameBuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Clear() 
{
	//Bind the current buffer
	Bind();

	glClearColor(0,0,0,0);

	//Empty the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Unbind after clear
	//Maybe wanna delete this later for optimization IDK
	Unbind();
}

void FrameBuffer::Delete()
{
	glDeleteRenderbuffers(1, &depthBuffer);
	glDeleteTextures(1, &colorBuffer);
	glDeleteFramebuffers(1, &id);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
