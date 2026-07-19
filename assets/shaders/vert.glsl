#version 330 core

layout(location = 0) in vec4 pos;

uniform mat4 windowMat;
uniform mat4 transform;

out vec3 fragpos;

void main() {
	gl_Position = windowMat * transform * vec4(pos.xyz, 1.0);
	fragpos = pos.xyz;
}
