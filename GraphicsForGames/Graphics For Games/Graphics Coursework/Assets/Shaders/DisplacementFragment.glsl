#version 330
uniform sampler2D diffuseTex;
uniform sampler2D displacementMap;

uniform float time;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColor;

void main(void){
	float disp = texture(displacementMap, vec2(IN.texCoord.x + time, IN.texCoord.y + time)).r - 0.5f;

	vec2 newCoords = IN.texCoord + vec2(disp*0.1f,disp*0.1f);

	fragColor = texture(diffuseTex, vec2(IN.texCoord.x, IN.texCoord.y));

	fragColor.a = 0.7;

	fragColor.b *= 1.5;
	fragColor.b = clamp(fragColor.b, 0.0, 1.0);
}