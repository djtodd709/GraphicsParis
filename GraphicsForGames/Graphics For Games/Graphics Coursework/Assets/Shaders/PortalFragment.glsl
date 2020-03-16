#version 150

uniform sampler2D diffuseTex;
uniform sampler2D portalTex;
uniform sampler2D portalSpaceTex;
uniform sampler2D displacementMap;

uniform float time;
uniform float whiteout;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColor;


void main(void){
	float disp = texture(displacementMap, vec2(IN.texCoord.x + time, IN.texCoord.y + time)).r - 0.5f;

	vec2 newCoords = IN.texCoord + vec2(disp * 0.2f, disp * 0.2f);

	fragColor = texture(diffuseTex, IN.texCoord.xy);
	vec4 fragColor2 = texture(portalSpaceTex, IN.texCoord.xy);
	
	if (fragColor2.r == 0.0 && fragColor2.g == 0.0 && fragColor2.b == 1.0) {
		fragColor = texture(portalTex, newCoords.xy);
	}

	fragColor.rgba += (1-whiteout);

	fragColor.rgba = clamp(fragColor.rgba, vec4(0, 0, 0, 0), vec4(1, 1, 1, 1));
}