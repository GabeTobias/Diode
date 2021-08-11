#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec3 worldPosition;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main() 
{
	mat4 mvp = P * V * M;

	FragPos = vec3(M * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(M))) * aNormal;  
	TexCoords = aTexCoords;

	worldPosition = (M * vec4(aPos, 1.0)).xyz;
    gl_Position = mvp * vec4(aPos, 1.0);
}
