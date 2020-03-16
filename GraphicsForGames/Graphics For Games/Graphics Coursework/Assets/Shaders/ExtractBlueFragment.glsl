#version 150

uniform sampler2D diffuseTex;


in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColor;


void main(void){


	fragColor = texture(diffuseTex, IN.texCoord.xy);
	
	if (abs(fragColor.r - 0.0) > 0.001) {
		fragColor = vec4(0, 0, 0, 1.0);
	}
	if (abs(fragColor.g - 0.0) > 0.001) {
		fragColor = vec4(0, 0, 0, 1.0);
	}
	if (abs(fragColor.b - 1.0) > 0.001) {
		fragColor = vec4(0, 0, 0, 1.0);
	}
	
}