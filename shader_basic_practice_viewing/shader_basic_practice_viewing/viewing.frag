#version 450 core

uniform sampler2D texSampler;
uniform int displayMode;

in vec2 vTexCoord;
in vec4 vColor;
out vec4 FragColor;
out vec4 FragScreenShotColor;

void main(void)
{

	float z = (gl_FragCoord.z - 0.35F);
	float fog = 1.F - 1.8F*z;
	vec4 src = vColor;
	vec4 dst = vec4(0.8F , 0.8F , 0.8F , 1.F);


	if(displayMode == 1)
	{
		FragColor = texture2D(texSampler,vTexCoord);
	
	}
	else{
		
		FragColor = fog*src + (1-fog)*dst;
	}

	FragScreenShotColor = vec4( z , z , z , 1.F);

}