#version 330 core

in vec2 texCoord;
out vec4 fragColor;

uniform float iTime;
uniform vec2 iResolution;
uniform vec2 iMouse;

uniform sampler2D iChannel0;
uniform sampler2D iChannel1;

int texget(float x, float y) 
{
    return int(texture(iChannel0, (texCoord.xy + vec2(x, y) / iResolution)).g);
}

void main()
{
    int density = 
        texget(-1, -1) +
        texget(-1,  0) +
        texget(-1,  1) +
        texget( 0, -1) +
        texget( 0,  1) +
        texget( 1, -1) +
        texget( 1,  0) +
        texget( 1,  1);

    if (iTime < 1.0)
    {
        fragColor = vec4(vec3(texture(iChannel1, texCoord.xy).x), 1.0);
    }
    else if (density == 3)
    {
        fragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
    else if (density == 2)
    {
        float current = float(texget(0, 0));
        fragColor = vec4(current, current, current, 1.0);
    }
    else
    {
        fragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
