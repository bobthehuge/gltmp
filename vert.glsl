#version 330 core

in vec3 iPosition;
uniform float iTime;

out vec4 vertexColor;

void main()
{
    gl_Position = vec4(iPosition, 1.0);
    vertexColor = vec4(cos(iTime)*iPosition.x, 1.0, 1.0, 1.0);
}
