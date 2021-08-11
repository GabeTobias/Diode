#pragma once

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <fstream>

#include <glm/glm.hpp>

#ifndef SHADER_H
#define SHADER_H

class Shader
{
public:
	unsigned int program;

	int fragmentShader;
	int vertexShader;

	std::string vertexString;
	std::string fragmentString;

	const char* shaderRoot = "src/shaders/";

public:
	void load(std::string frag, std::string vert);
	
	////////////////////////////////////////////////////////////
	
	unsigned int getUniform(std::string name);

	void setUniform(std::string name, int value);
	void setUniform(std::string name, float value);
	void setUniform(std::string name, double value);

	void setUniform(std::string name, glm::vec2 value);
	void setUniform(std::string name, glm::vec3 value);
	void setUniform(std::string name, glm::vec4 value);

	void setUniform(std::string name, glm::mat2 value);
	void setUniform(std::string name, glm::mat3 value);
	void setUniform(std::string name, glm::mat4 value);
};

int LoadShader(const char* src, bool isVertex);

#endif