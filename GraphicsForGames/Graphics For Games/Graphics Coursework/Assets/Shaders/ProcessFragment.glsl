#version 150

uniform sampler2D diffuseTex;
uniform sampler2D depthTex;
uniform vec2 pixelSize;
uniform int isVertical;

uniform float passNum;


float near = 1.0f;
float far = 10000.0f;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColor;

const float weights[5] = float[](0.12,0.22,0.32,0.22,0.12);

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; // back to NDC 
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main(void){
	float threshold = (passNum+1) / 20.0;
	float depth = (texture(depthTex, IN.texCoord.xy)).x;

	depth = LinearizeDepth(depth) / far;

	fragColor = vec4(0, 0, 0, 1);

	if (depth < threshold) {
		fragColor = texture(diffuseTex, IN.texCoord.xy);
		return;
	}


	

	vec2 values[5];

	if(isVertical == 1){
		values = vec2[](vec2(0.0, -pixelSize.y*2),
						vec2(0.0, -pixelSize.y*1),
						vec2(0.0, 0.0),
						vec2(0.0, pixelSize.y*1),
						vec2(0.0, pixelSize.y*2) );
	}
	else{
		values = vec2[](vec2(-pixelSize.x*2, 0.0),
						vec2(-pixelSize.x*1, 0.0),
						vec2(0.0, 0.0),
						vec2(pixelSize.x*1, 0.0),
						vec2(pixelSize.x*2, 0.0) );
	}

	for(int i=0; i < 5; i++){
		vec4 tmp = texture(diffuseTex, IN.texCoord.xy + values[i]);
		fragColor.rgb += (tmp*weights[i]).rgb;
	}

	
}