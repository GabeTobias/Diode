#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec3 worldPosition;

flat out int useTexture;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

uniform int textured;

uniform vec2 TexSize;
uniform vec4 TexUV;
uniform mat4 TexScale;

void main() 
{
	mat4 mvp = P * V * M;

	//Texel Size
	float w = 1.0 / (TexSize.x / TexUV.z);
	float h = 1.0 / (TexSize.y/ TexUV.w);

	//Position in UV Space
	float xp = w * TexUV.x;
	float yp = h * TexUV.y;

	FragPos = vec3(M * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(M))) * aNormal;  
	
	if(textured == 1)
		TexCoords = vec2((aTexCoords.x*w) + xp, (aTexCoords.y*h) + yp);
	else
		TexCoords = aTexCoords;

	worldPosition = (M * TexScale * vec4(aPos, 1.0)).xyz;
    gl_Position = mvp * TexScale * vec4(aPos, 1.0);

	useTexture = textured;
}
