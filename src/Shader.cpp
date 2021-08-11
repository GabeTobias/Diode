#include "Shader.h"

#include <GL/glew.h>

std::string readFile(const char* filePath);

void Shader::load(std::string frag, std::string vert)
{
	//Save File Source
	vertexString = vert;
	fragmentString = frag;

	//Load Shaders from Data
	fragmentShader = LoadShader(readFile(frag.c_str()).c_str(), false);
	vertexShader = LoadShader(readFile(vert.c_str()).c_str(), true);

	// link shaders
	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	
	// check for linking errors
	int success;
	char infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	printf("Loaded Shader %s & %s \n", frag.c_str(), vert.c_str());

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int Shader::getUniform(std::string name)			{ return glGetUniformLocation(program, name.c_str()); }

void Shader::setUniform(std::string name, int value)		{ glUniform1i(getUniform(name), value); }
void Shader::setUniform(std::string name, float value)		{ glUniform1f(getUniform(name), value); }
void Shader::setUniform(std::string name, double value)		{ glUniform1d(getUniform(name), value); }

void Shader::setUniform(std::string name, glm::vec2 value)	{ glUniform2f(getUniform(name), value.x, value.y); }
void Shader::setUniform(std::string name, glm::vec3 value)	{ glUniform3f(getUniform(name), value.x, value.y, value.z); }
void Shader::setUniform(std::string name, glm::vec4 value)	{ glUniform4f(getUniform(name), value.x, value.y, value.z, value.w); }

void Shader::setUniform(std::string name, glm::mat2 value)	{ glUniformMatrix2fv(getUniform(name), 1, GL_FALSE, &value[0][0]); }
void Shader::setUniform(std::string name, glm::mat3 value)	{ glUniformMatrix3fv(getUniform(name), 1, GL_FALSE, &value[0][0]); }
void Shader::setUniform(std::string name, glm::mat4 value)	{ glUniformMatrix4fv(getUniform(name), 1, GL_FALSE, &value[0][0]); }

/////////////////////////////////////////////////////////////////////////////////////////////////////////

int LoadShader(const char* src, bool isVertex)
{
	// Compile shader
	int shader = glCreateShader((isVertex) ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);

	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	return shader;
}

//TODO: Move this into a seperate File Handling file
std::string readFile(const char* filePath) {
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);

	if (!fileStream.is_open()) {
		std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
		return "";
	}

	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}
