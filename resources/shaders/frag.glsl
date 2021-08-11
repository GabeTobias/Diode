#version 330

out vec4 fragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
flat in int useTexture;

uniform vec3 viewPos;
uniform vec4 color;

uniform sampler2D sprite;

uniform int ellipse;
uniform int outline;
uniform float weight;
uniform float angle;

void main() 
{
    fragColor = color;
    if(useTexture == 1) fragColor *= texture(sprite, TexCoords);

    if(outline == 1 && ellipse == 0)
    {
        if(TexCoords.x > weight && TexCoords.x < 1.0 - weight && TexCoords.y > weight && TexCoords.y < 1.0 - weight)
            fragColor.a *= 0;
    }

    if(ellipse == 1)
    {
        if(distance(TexCoords, vec2(0.5)) > 0.5) fragColor.a = 0;
        if(outline == 1 && distance(TexCoords, vec2(0.5)) < 0.5-weight) fragColor.a = 0;
    }

    if(fragColor.a == 0) discard;
}