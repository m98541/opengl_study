#version 450 core

uniform int colorID;
uniform vec4 panelColor;

in vec4 vColor;
out vec4 FragColor;

void main(void)
{
	
	vec4 outColor[4] = {
		vColor,						//hl on
		vec4(1.F , 1.F , 0.8F , 1.F),//hl off
		vec4(0.F , 0.F , 1.F , 1.F),//hole_panel out
		vec4(1.F , 0.F , 0.F , 1.F)

	};

	FragColor = outColor[colorID];
}