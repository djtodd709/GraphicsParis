#version 150

uniform sampler2D diffuseTex;

float near = 1.0f;
float far = 10000.0f;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColor;


void main(void){


	fragColor = texture(diffuseTex, IN.texCoord.xy);
	
	if (abs(fragColor.r - 1.0) > 0.001) {
		fragColor = vec4(0, 0, 0, 1.0);
	}
	if (abs(fragColor.g - (235.0/255.0)) > 0.001) {
		fragColor = vec4(0, 0, 0, 1.0);
	}
	if (abs(fragColor.b - (133.0/255.0)) > 0.001) {
		fragColor = vec4(0, 0, 0, 1.0);
	}
	
}