#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

uniform vec2 pos;
uniform vec2 size;

uniform mat4 projection;

out vec2 TexCoord;
out vec3 VertexColor;

void main() 
{   
	//Vertex attributes
	TexCoord = aTexCoord;
	VertexColor = aColor;

	//Final Vertex position
	vec4 p = 
		(vec4(aPos.x, aPos.y, aPos.z, 1.0) * vec4(size.x, size.y, 1,1)) + 
		 vec4(pos.x+(size.x/2.f), pos.y+(size.y/2.f), 1, 0);

	gl_Position = projection * p;
}
