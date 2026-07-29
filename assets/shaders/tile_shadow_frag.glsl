#version 330 core

in vec3 fragpos;

out vec4 fragColor;

uniform float shading;

void main() {
	float t = fragpos.y + 0.5;
	float alpha = t * shading;
	fragColor = vec4(0.0, 0.0, 0.0, alpha);
}
