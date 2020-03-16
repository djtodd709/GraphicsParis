#version 150
uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;
uniform sampler2DShadow shadowTex;
uniform int useTexture;

uniform float shadowR;

uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightDir;

uniform vec4 pLightColour[49];
uniform vec3 pLightPos[49];
uniform float pLightRadius[49];

in Vertex{
	vec2 texCoord;
	vec4 colour;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	vec4 shadowProj;
} IN;

out vec4 fragColor;

void main(void) {
	vec4 diffuse = IN.colour;
	if (useTexture > 0) {
		diffuse *= texture(diffuseTex, IN.texCoord);
	}

	mat3 TBN = mat3(IN.tangent, IN.binormal, IN.normal);

	vec3 normal = normalize(TBN * (texture(bumpTex, IN.texCoord).rgb * 2.0 - 1.0));

	vec3 newLightDir = -normalize(lightDir);

	float intensity = max(0.0, dot(newLightDir, normal));

	vec3 viewDir = normalize(cameraPos - IN.worldPos);
	vec3 halfDir = normalize(newLightDir + viewDir);

	float rFactor = max(0.0, dot(halfDir, normal));
	float sFactor = pow(rFactor, 33.0);


	fragColor = vec4(0, 0, 0, 1);

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

	
	for (int i = 0; i < 49; i++) {
		float dist = length(pLightPos[i] - IN.worldPos);
		if (dist > pLightRadius[i]) {
			continue;
		}

		float falloff = pow((pLightRadius[i] - dist)/ pLightRadius[i], 0.25);

		vec3 incident = normalize(pLightPos[i] - IN.worldPos);
		float lambert = max(0.0, dot(incident, normal));
		float atten = 1.0 - clamp(dist / pLightRadius[i], 0.0, 1.0);

		viewDir = normalize(cameraPos - IN.worldPos);
		halfDir = normalize(incident + viewDir);

		rFactor = max(0.0, dot(halfDir, normal));
		sFactor = pow(rFactor, 50.0);

		vec3 colour = (diffuse.rgb * pLightColour[i].rgb);
		colour += (pLightColour[i].rgb * sFactor) * 0.33;
		fragColor.rgb += colour * atten * lambert * falloff * shadowR;
		fragColor.rgb += (diffuse.rgb * pLightColour[i].rgb) * 0.1 * falloff * shadowR;
	}

	fragColor.rgb = clamp(fragColor.rgb, vec3(0, 0, 0), vec3(1, 1, 1));
}