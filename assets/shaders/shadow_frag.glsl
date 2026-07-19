#version 330 core

in vec3 fragpos;

out vec4 fragColor;

const float ALPHA_FACTOR = 0.4;

float calculateAlpha() {
	float dist = length(fragpos * 2.0);
	return ALPHA_FACTOR * clamp(pow(1.0 - pow(dist, 5.0), 3.0), 0.0, 1.0);
}

void main() {
	float alpha = calculateAlpha();
	fragColor = vec4(0.0, 0.0, 0.0, alpha);
}
