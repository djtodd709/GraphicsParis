#version 400

layout(triangles) in;
layout(triangle_strip, max_vertices = 7) out;

uniform float time;

in Vertex{
	vec4 colour;
	vec3 normal;
	vec3 worldPos;
	vec4 shadowProj;
} IN[];

out Vertex {
	vec4 colour;
	vec3 normal;
	vec3 worldPos;
	vec4 shadowProj;
	flat bool useTex;
	vec2 texCoord;
} OUT;

void main(void) {
	for (int i = 0; i < gl_in.length(); ++i) {
		gl_Position = gl_in[i].gl_Position;

		OUT.colour = IN[i].colour;
		OUT.normal = IN[i].normal;
		OUT.worldPos = IN[i].worldPos;
		OUT.shadowProj = IN[i].shadowProj;
		OUT.useTex = false;
		OUT.texCoord = vec2(0.0, 0.0);
		EmitVertex();
	}
	EndPrimitive();

	if (IN[2].worldPos.y > -52.0) {
		gl_Position = gl_in[2].gl_Position;
		gl_Position.x -= 20.0;
		OUT.colour = IN[2].colour;
		OUT.normal = IN[2].normal;
		OUT.worldPos = IN[2].worldPos;
		OUT.shadowProj = IN[2].shadowProj;
		OUT.useTex = true;
		OUT.texCoord = vec2(0.0, 0.0);
		EmitVertex();

		gl_Position = gl_in[2].gl_Position;
		gl_Position.x += 20.0;
		OUT.colour = IN[2].colour;
		OUT.normal = IN[2].normal;
		OUT.worldPos = IN[2].worldPos;
		OUT.shadowProj = IN[2].shadowProj;
		OUT.useTex = true;
		OUT.texCoord = vec2(1.0, 0.0);
		EmitVertex();

		gl_Position = gl_in[2].gl_Position;
		gl_Position.x -= 30.0 - 2.0 * sin(5.0f*(time + IN[2].worldPos.x + IN[2].worldPos.z));
		gl_Position.y += 50.0;
		OUT.colour = IN[2].colour;
		OUT.normal = IN[2].normal;
		OUT.worldPos = IN[2].worldPos;
		OUT.shadowProj = IN[2].shadowProj;
		OUT.useTex = true;
		OUT.texCoord = vec2(0.0, 1.0);
		EmitVertex();

		gl_Position = gl_in[2].gl_Position;
		gl_Position.x += 30.0 + 2.0 * sin(5.0f * (time + IN[2].worldPos.x + IN[2].worldPos.z));
		gl_Position.y += 50.0;
		OUT.colour = IN[2].colour;
		OUT.normal = IN[2].normal;
		OUT.worldPos = IN[2].worldPos;
		OUT.shadowProj = IN[2].shadowProj;
		OUT.useTex = true;
		OUT.texCoord = vec2(1.0, 1.0);
		EmitVertex();

		EndPrimitive();
	}
}