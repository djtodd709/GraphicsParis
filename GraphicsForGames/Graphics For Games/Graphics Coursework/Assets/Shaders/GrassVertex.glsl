#version 400

in vec3 position;
in vec3 normal;
in vec2 texCoord;

out Vertex {
	vec3 normal;
	vec2 texCoord;
} OUT;

void main(void) {
	gl_Position = vec4(position, 1.0);
	OUT.normal = normal;
	OUT.texCoord = texCoord;

}