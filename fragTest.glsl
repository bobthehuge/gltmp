#version 330 core

in vec2 texCoord;
out vec4 fragColor;

uniform float iTime;
uniform vec2 iResolution;
uniform vec2 iMouse;

uniform sampler2D iChannel0;
uniform sampler2D iChannel1;

void main()
{
    vec2 mouse = (2. * iMouse - iResolution) / iResolution;

    vec2 norm = 1 / iResolution;

    /* for(float i = -5 * norm; i < 5 * norm; i += norm) */
    /* { */
    /*     for(float j = -5 * norm; i < 5 * norm; i += norm) */
    /*     { */
    /*         if(texCoord.x + j ) */
    /*     } */
    /* } */


    if (gl_FragCoord.x < 100)
    { 
        fragColor = vec4(0.0, 1.0, 0.0, 1.0);
    }
    else if (texCoord.x <= mouse.x)
    {
        fragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
    else
    {
        fragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
}
