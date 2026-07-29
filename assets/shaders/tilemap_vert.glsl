#version 330 core

layout(location = 0) in vec4 pos;
layout(location = 1) in vec2 textureOffset;
layout(location = 2) in float shading;

uniform mat4 windowMat;
uniform vec2 offset;
uniform vec2 textureScale;
uniform float z;

out float shadingValue;
out vec2 tc;

void main() {
	gl_Position = windowMat * vec4(pos.xyz + vec3(-0.5, -0.5, 0.0) + vec3(offset, z), 1.0);

	tc = textureOffset;
	tc.x /= textureScale.x;
	tc.y /= textureScale.y;

	shadingValue = shading;
}
