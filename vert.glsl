#version 330 core

in vec3 iPosition;

out vec4 vertexColor;

void main()
{
    gl_Position = vec4(iPosition, 1.0);
    vertexColor = vec4(iPosition.xy, 0.0, 1.0);
}
