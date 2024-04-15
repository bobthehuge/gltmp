#version 330 core

in vec2 fragCoord;
out vec4 fragColor;

uniform float iTime;
uniform vec2 iResolution;
uniform vec2 iMouse;
uniform uint iFrame;

uniform sampler2D iChannel0;
uniform sampler2D iChannel1;

void main()
{
    vec4 color = texture(iChannel0, fragCoord);
    vec3 vars = vec3(sin(iTime), cos(iTime), tan(iTime));

    fragColor = vec4(color.xyz, 1.0);
    /* fragColor = vec4(color.xyz + vars, 1.0); */
    /* fragColor = vec4(1.0, 0.0, 0.0, 1.0); */
}
