#version 330 core
layout (location = 0) in vec2 pos;
uniform mat4 model;
uniform mat4 proj;
out vec2 texCoords;
void main()
{
	texCoords = pos;
	gl_Position = proj * model * vec4(pos, 1.0, 1.0);
}
