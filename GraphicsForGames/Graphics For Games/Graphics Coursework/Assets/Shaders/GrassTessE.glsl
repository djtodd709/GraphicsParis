#version 400

layout(triangles) in;

uniform sampler2D heightMap;
uniform sampler2D normalMap;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec4 nodeColour;
uniform mat4 textureMatrix;

uniform float rise;

in Vertex{
	vec3 normal;
	vec2 texCoord;
} IN[];

out Vertex {
	vec4 colour;
	vec3 normal;
	vec3 worldPos;
	vec4 shadowProj;
} OUT;


vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
	return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
	return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

void main(void) {

	vec3 combinedPos = interpolate3D(gl_in[0].gl_Position.xyz, gl_in[1].gl_Position.xyz, gl_in[2].gl_Position.xyz);

	vec3 combinedNormal = interpolate3D(IN[0].normal, IN[1].normal, IN[2].normal);
	vec2 texCoord = interpolate2D(IN[0].texCoord, IN[1].texCoord, IN[2].texCoord);

	vec4 worldPos = modelMatrix * vec4(combinedPos, 1.0);

	float height = texture(heightMap, texCoord).x;
	vec3 noise = (2.0*texture(normalMap, texCoord) - 1.0).rgb;

	OUT.colour = nodeColour;

	worldPos.y += height * 500 * rise;

	OUT.worldPos = worldPos.xyz;

	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	OUT.normal = normalize(normalMatrix * normalize(combinedNormal));

	OUT.shadowProj = textureMatrix * worldPos;

	gl_Position = projMatrix * viewMatrix * worldPos;
}