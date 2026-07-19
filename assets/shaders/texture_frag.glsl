#version 330 core

uniform sampler2D tex;
uniform float texScale;
uniform vec2 texOffset;

in vec3 fragpos;

out vec4 fragColor;

void main() {
	vec2 tc = fragpos.xy * 0.5 + vec2(0.5, 0.5);
	tc /= texScale;
	tc += texOffset / texScale;
	fragColor = texture(tex, tc);
}
