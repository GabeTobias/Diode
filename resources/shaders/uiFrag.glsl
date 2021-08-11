#version 330

const float PI = 3.14159265;

out vec4 fragColor;

in vec2 TexCoord;
in vec3 VertexColor;

uniform vec4 color;
uniform vec4 strokeColor;
uniform vec2 size;

uniform float edgeRadius;
uniform float alpha;
uniform float stroke;

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
    
    //Round Corners
    vec4 val = RoundEdge(pos,size, int(edgeRadius));

    //Discard unused pixels
    if(val.a == 0) discard;

    if(val == vec4(0,0,1,1)) fragColor = strokeColor;

    if(pos.x < stroke || pos.x > size.x-stroke|| pos.y < stroke || pos.y > size.y-stroke) fragColor = strokeColor;
}

vec4 RoundEdge(vec2 pos, vec2 size, int radius)
{
    //Distance to corner
    float dist;

    //Edge value
    bool edge = false;
    bool px = false;
    bool py = false;

    //Find Corner
    if(pos.x > size.x/2) px = true;
    if(pos.y > size.y/2) py = true;

    //Edge Detection
    if((pos.x < radius || pos.x > size.x-radius) && (pos.y < radius || pos.y > size.y-radius)) edge = true;
    if(!edge) return vec4(1);

    //Get Distance of nearest corner
    if(!px && !py) dist = distance(pos, vec2(radius));
    if(px && !py) dist = distance(pos, vec2(size.x-radius, radius));
    if(px && py) dist = distance(pos, size-vec2(radius));
    if(!px && py) dist = distance(pos, vec2(radius, size.y-radius));

    //Return Edge value
    if(dist > radius) return vec4(0);
    else return vec4(1);
}