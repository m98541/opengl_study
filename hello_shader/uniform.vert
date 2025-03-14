#version 330 core
in vec4 aPos;
in vec4 aColor;
out vec4 vColor;
uniform vec4 uMove;

void main(void)
{
	gl_Position = aPos + uMove;
	vColor = aColor;
}