#version 450 core

uniform mat4 uMVP;


in vec4 aPos;
in vec4 aColor;


out vec4 vColor;

void main(void)
{
	vColor = aColor;
	
	gl_Position = uMVP*aPos;

}