#version 450 core


in vec4 vColor;
in vec2 vTexCoord;
in vec2 vTexCoordAirBalloon;

uniform sampler2D texSampler;
uniform sampler2D texSamplerAirBalloon;
out vec4 FragColor;

void main(void)
{	
	vec4 colorDst = texture(texSampler,vTexCoord);
	vec4 colorSrc = texture(texSamplerAirBalloon,vTexCoordAirBalloon);
	float arr1[2] = {0.F, 1.F};
	bool b  = (vec3(1.F , 1.F ,1.F) != colorSrc.xyz);

	colorSrc.w = int(b);
	FragColor =  colorSrc.w*colorSrc+(1.F-colorSrc.w)*colorDst;
	FragColor.w = 1.F;
}