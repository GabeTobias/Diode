#pragma once

#include <glm/glm.hpp>

inline glm::vec3 HSV(glm::vec3 color) 
{
    float h = color.r;
    float s = color.g/100.f;
    float v = color.b/100.f;

    float r;
    float g;
    float b;

    if (s <= 0.0) {       // < is bogus, just shuts up warnings
        r = v;
        g = v;
        b = v;
        return glm::vec3(r,g,b);
    }

    double      hh, p, q, t, ff;
    long        i;

    hh = h;
    if (hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = v * (1.0 - s);
    q = v * (1.0 - (s * ff));
    t = v * (1.0 - (s * (1.0 - ff)));

    switch (i) {
        case 0:
            r = v;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = v;
            b = p;
            break;
        case 2:
            r = p;
            g = v;
            b = t;
            break;

        case 3:
            r = p;
            g = q;
            b = v;
            break;
        case 4:
            r = t;
            g = p;
            b = v;
            break;
        case 5:
        default:
            r = v;
            g = p;
            b = q;
            break;
    }

    return glm::vec3(r,g,b);
}

inline glm::vec3 Normalize(glm::vec3 color) 
{
    return {
        color.r / 255,
        color.g / 255,
        color.b / 255,
    };
}

inline glm::vec3 RandomColor()
{
    glm::vec3 RGB = HSV({ rand() % 360, 90, 90 });
    return Normalize(RGB);
}

inline glm::vec4 toRGB(float r, float g, float b)
{
    return glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.f);
}