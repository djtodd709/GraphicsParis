#version 150
uniform sampler2D diffuseTex;
uniform sampler2D bloomTex;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColor;

void main(void){
	fragColor = texture(diffuseTex, IN.texCoord);
	fragColor.rgb += (texture(bloomTex, IN.texCoord)).rgb;
	fragColor.rgb = clamp(fragColor.rgb, vec3(0, 0, 0), vec3(1, 1, 1));
}