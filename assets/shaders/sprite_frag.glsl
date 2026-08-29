#version 330 core

uniform sampler2D tex;

in vec3 fragpos;

out vec4 fragColor;

uniform bool flipVert;
uniform vec4 multColor;
uniform vec4 color;

void main() {
	float brightness = pow(fragpos.y + 0.5, 0.5) * 0.7 + 0.3;
	vec2 tc = fragpos.xy + vec2(0.5, 0.5);
	tc.x = clamp(tc.x, 0.01, 0.99);
	tc.y = clamp(tc.y, 0.01, 0.99);
	tc.x = (1.0 - tc.x) * float(flipVert) + tc.x * float(!flipVert);
	fragColor = texture(tex, tc);
	fragColor.r *= brightness;
	fragColor.g *= brightness;
	fragColor.b *= brightness;
	
	fragColor *= multColor;
	fragColor += vec4(color.rgb, 0.0);
	fragColor = min(fragColor, 1.0);
	if(fragColor.a < 0.1)
		discard;
}
