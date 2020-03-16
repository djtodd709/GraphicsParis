#version 400

layout(vertices = 3) out;

in Vertex{
	vec3 normal;
	vec2 texCoord;
} IN[];

out Vertex {
	vec3 normal;
	vec2 texCoord;
} OUT[];

void main(void) {
	gl_TessLevelInner[0] = 14;
	gl_TessLevelInner[1] = 14;
	gl_TessLevelOuter[0] = 14;
	gl_TessLevelOuter[1] = 14;
	gl_TessLevelOuter[2] = 14;
	gl_TessLevelOuter[3] = 14;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	OUT[gl_InvocationID].normal = IN[gl_InvocationID].normal;
	OUT[gl_InvocationID].texCoord = IN[gl_InvocationID].texCoord;

}