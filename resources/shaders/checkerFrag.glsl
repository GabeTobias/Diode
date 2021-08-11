#version 330

out vec4 fragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform int size;

float checker(vec2 uv, float repeats) 
{
  float cx = floor(repeats * uv.x);
  float cy = floor(repeats * uv.y); 
  float result = mod(cx + cy, 2.0);
  return max(sign(result), 0.75);
}

void main() 
{
    fragColor = (vec4(checker(TexCoords, size)) * 0.2);
    if(fragColor.a == 0) discard;
}