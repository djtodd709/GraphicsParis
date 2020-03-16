#version 150
uniform sampler2D diffuseTex;
uniform int useTexture;

uniform float time;
uniform vec3 fadedir;

uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightDir;

in Vertex {
	vec2 texCoord;
	vec4 colour;
	vec3 normal;
	vec3 worldPos;
	vec3 modelPos;
} IN;

out vec4 fragColor;

void main(void){
	float fadeValue = (fadedir.z * max(-IN.modelPos.x,IN.modelPos.z)) + (fadedir.y * IN.modelPos.y);

	if (fadeValue > time) {
		discard;
	}
	
	vec4 diffuse = IN.colour;
	if(useTexture > 0){
		diffuse *= texture(diffuseTex, IN.texCoord);
	}

	vec3 newLightDir = -normalize(lightDir);

	float intensity = max(0.0, dot(newLightDir, IN.normal));

	intensity = min(1.0, intensity + 0.1);

	vec3 viewDir = normalize(cameraPos - IN.worldPos);
	vec3 halfDir = normalize(newLightDir + viewDir);

	float rFactor = max(0.0, dot(halfDir, IN.normal));
	float sFactor = pow(rFactor, 33.0);

	fragColor = vec4(0, 0, 0, diffuse.a);

	vec3 colour = (diffuse.rgb * lightColour.rgb);
	colour += (lightColour.rgb * sFactor) * 0.33;
	fragColor.rgb = colour * intensity;
	fragColor.rgb += (diffuse.rgb * lightColour.rgb) * 0.1;
	
	fragColor.rgb = clamp(fragColor.rgb, vec3(0, 0, 0), vec3(1, 1, 1));
}