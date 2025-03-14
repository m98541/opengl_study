#version 450 core

uniform mat4 aMVP;	

in vec4 aPos;
in vec4 aColor;
in vec4 aNorm;

out vec4 vColor;
out vec4 vNorm;

void main(void)
{
	vColor = aColor;
	vNorm = aNorm;
	gl_Position =  aMVP*aPos;
}