#version 150
uniform sampler2D diffuseTex;
uniform samplerCube cubeTex;

uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;

in Vertex{
	vec2 texCoord;
	vec4 colour;
	vec3 normal;
	vec3 worldPos;
	vec4 shadowProj;
} IN;

out vec4 fragColor;

void main(void){

	vec3 incident = normalize(IN.worldPos - cameraPos);

	vec4 reflection = texture(cubeTex, reflect(incident, normalize(IN.normal)));

	fragColor = reflection;

	fragColor.b += 0.1;

	fragColor.b = clamp(fragColor.b, 0.0, 1.0);
	
}