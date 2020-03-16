#version 150

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec4 nodeColour;
uniform mat4 textureMatrix;

in vec3 position;
in vec3 normal;
in vec3 tangent;
in vec2 texCoord;

out Vertex {
	vec2 texCoord;
	vec4 colour;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	vec4 shadowProj;
} OUT;

void main(void) {
	gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
	OUT.texCoord = texCoord;
	OUT.colour = nodeColour;
	OUT.worldPos = (modelMatrix * vec4(position, 1.0)).xyz;

	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	OUT.normal = normalize(normalMatrix * normalize(normal));
	OUT.tangent = normalize(normalMatrix * normalize(tangent));
	OUT.binormal = normalize(normalMatrix * normalize(cross(normal, tangent)));

	OUT.shadowProj = (textureMatrix * modelMatrix * vec4(position + (normalize(normal) * 0.008), 1));
}