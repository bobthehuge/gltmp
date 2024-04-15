#version 330 core

in vec2 fragCoord;
out vec4 fragColor;

uniform float iTime;
uniform vec2 iResolution;
uniform vec2 iMouse;
uniform uint iFrame;

uniform sampler2D iChannel0;

vec4 texget(float x, float y) 
{
    return texture(iChannel0, (texCoord.xy + vec2(x, y) / iResolution));
}

int texgetval(float x, float y)
{
    return int(texget(x,y).r);
}

float rand(vec2 pos)
{
    return fract(sin(dot(pos, vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
    int density = 
        texgetval(-1, -1) +
        texgetval(-1,  0) +
        texgetval(-1,  1) +
        texgetval( 0, -1) +
        texgetval( 0,  1) +
        texgetval( 1, -1) +
        texgetval( 1,  0) +
        texgetval( 1,  1);

    if (gl_FragCoord.x < 4 && gl_FragCoord.y < 4)
    {
        if (iFrame % 2u == 0u)
        {
            fragColor = vec4(1.0, 0.0, 0.0, 1.0);
        }
        else
        {
            fragColor = vec4(0.0, 1.0, 0.0, 1.0);
        }
    }

    if (iTime < 1.0)
    {
        float f = rand(texCoord) > 0.5 ? 0.0 : 1.0;
        fragColor = vec4(f, f, f, 1.0);
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
