#version 330 core

in vec2 fragCoord;

layout (location = 0) out vec4 out0;
layout (location = 1) out vec4 out1;

uniform float iTime;
uniform vec2 iResolution;
uniform vec2 iMouse;
uniform uint iFrame;

uniform sampler2D iChannel0;
uniform sampler2D iChannel1;

void main()
{
    /* vec4 act = texture(iChannel0, fragCoord); */
    uint linear = uint(gl_FragCoord.y * iResolution.x + gl_FragCoord.x);

    if (linear % 2u == 0u)
    {
        if (iFrame % 2u == 0u)
        {
            out0 = vec4(1.0, 0.0, 0.0, 1.0);
            out1 = texture(iChannel1, fragCoord);
        }
        else
        {
            out0 = texture(iChannel0, fragCoord);
            out1 = vec4(0.0, 0.0, 0.0, 1.0);
        }
    }
    else
    {
        if (iFrame % 2u == 0u)
        {
            out0 = vec4(0.0, 0.0, 0.0, 1.0);
            out1 = texture(iChannel1, fragCoord);
        }
        else
        {
            out0 = texture(iChannel0, fragCoord);
            out1 = vec4(0.0, 1.0, 0.0, 1.0);
        }
    }
}
