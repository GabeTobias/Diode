#version 330

const float PI = 3.14159265;

out vec4 fragColor;

in vec2 TexCoord;
in vec3 VertexColor;

uniform vec4 color;
uniform vec2 size;

uniform float edgeRadius;
uniform float alpha;

uniform int useTexture;
uniform int useColor;
uniform sampler2D sprite;

    
vec4 RoundEdge(vec2 pos, vec2 size, int radius);

void main() 
{
    //Pixel position on quad
    vec2 pos = (TexCoord.xy*size);
    
    //Final color
    if(useTexture == 1) {
        if(TexCoord.x < 0.01 || TexCoord.x > 0.99 ||  TexCoord.y < 0.01 || TexCoord.y > 0.99) discard;
        vec4 c = texture(sprite,TexCoord);
        
        if(useColor == 1 && c.a > 0.1) fragColor = color;
        else fragColor = texture(sprite,TexCoord) * vec4(1,1,1,alpha);
    }
    else 
        fragColor = color;

    //Discard unused pixels
    if(fragColor.a == 0) discard;
}
