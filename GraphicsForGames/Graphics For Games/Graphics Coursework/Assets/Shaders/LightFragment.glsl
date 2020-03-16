#version 150
uniform sampler2D diffuseTex;
uniform int useTexture;

uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightDir;

in Vertex{
	vec2 texCoord;
	vec4 colour;
	vec3 normal;
	vec3 worldPos;
	vec4 shadowProj;
} IN;

out vec4 fragColor;

void main(void) {

	fragColor = IN.colour;

}