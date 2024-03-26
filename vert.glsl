#version 330 core

in vec3 Position;
out vec2 texCoord;

void main() 
{
    gl_Position = vec4(Position, 1.0);
    texCoord = Position.xy;
}
