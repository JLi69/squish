#version 330 core

uniform sampler2D tex;
uniform sampler2D colorTex;

in vec3 fragpos;

out vec4 fragColor;

uniform bool flipVert;
uniform vec4 color;

uniform vec2 scale;
uniform vec2 offset;

uniform vec2 colorScale;
uniform vec2 colorOffset;
uniform bool useColorTexture;

void main() {
	vec2 tc = fragpos.xy + vec2(0.5, 0.5);
	tc.x = clamp(tc.x, 0.01, 0.99);
	tc.y = clamp(tc.y, 0.01, 0.99);
	tc.x = (1.0 - tc.x) * float(flipVert) + tc.x * float(!flipVert);
	fragColor = texture(tex, tc * scale + offset) * color;

	tc = fragpos.xy * 0.5 + vec2(0.5, 0.5);
	vec4 texColor = texture(colorTex, tc * colorScale + colorOffset);
	texColor.r = max(texColor.r, float(!useColorTexture));
	texColor.g = max(texColor.g, float(!useColorTexture));
	texColor.b = max(texColor.b, float(!useColorTexture));
	texColor.a = max(texColor.a, float(!useColorTexture));
	fragColor *= texColor;
	
	if(fragColor.a < 0.1)
		discard;
}
