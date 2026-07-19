#version 330 core

uniform sampler2D tex;

out vec4 fragColor;

in float shadingValue;
in vec2 tc;

void main() {
	fragColor = texture(tex, tc) * shadingValue;
}
