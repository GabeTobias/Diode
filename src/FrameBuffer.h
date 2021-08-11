#pragma once

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

class FrameBuffer
{
public:

	unsigned int id = 0;
	unsigned int colorBuffer = 0;
	unsigned int depthBuffer = 0;

	int width, height;

public:
	FrameBuffer(int w = 1920, int h = 1080);

	void Init();
	
	void Bind();
	void Unbind();

	void Clear();

	void Delete();
};

#endif