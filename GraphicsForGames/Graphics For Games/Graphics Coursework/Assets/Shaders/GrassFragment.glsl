#version 400
uniform sampler2DShadow shadowTex;
uniform sampler2D grassTex;

uniform float shadowR;

uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightDir;

in Vertex{
	vec4 colour;
	vec3 normal;
	vec3 worldPos;
	vec4 shadowProj;
	flat bool useTex;
	vec2 texCoord;
} IN;

out vec4 fragColor;

void main(void) {
	vec4 diffuse = IN.colour;
	if (IN.useTex == true) {
		diffuse = texture(grassTex, IN.texCoord);
	}

	if (diffuse.a < 0.8f) {
		discard;
	}
	if (diffuse.a >= 0.8f) {
		diffuse.a = 1.0;
	}

	vec3 newLightDir = -normalize(lightDir);

	float intensity = max(0.0, dot(newLightDir, IN.normal));

	vec3 viewDir = normalize(cameraPos - IN.worldPos);
	vec3 halfDir = normalize(newLightDir + viewDir);

	float rFactor = max(0.0, dot(halfDir, IN.normal));
	float sFactor = pow(rFactor, 33.0);

	float shadow = 1.0;

	if (IN.shadowProj.w > 0.0) {
		shadow = textureProj(shadowTex, IN.shadowProj);
	}

	intensity = ((1.0-shadowR)*intensity) + (shadowR*intensity*shadow);


	fragColor = vec4(0, 0, 0, diffuse.a);

	vec3 colour = (diffuse.rgb * lightColour.rgb);
	colour += (lightColour.rgb * sFactor) * 0.33;
	fragColor.rgb = colour * intensity;
	fragColor.rgb += (diffuse.rgb * lightColour.rgb) * 0.1;

	fragColor.rgb = clamp(fragColor.rgb, vec3(0, 0, 0), vec3(1, 1, 1));
}