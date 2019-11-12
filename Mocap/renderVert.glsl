#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
uniform mat4 projView;
uniform mat4 model;
out vec3 fragPos;
out vec3 normal;
void main(){
	gl_Position = projView * model * vec4(pos, 1.0);
	fragPos = vec3(model * vec4(pos, 1.0));
	normal = norm;
}