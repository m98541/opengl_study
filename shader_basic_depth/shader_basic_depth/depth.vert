#version 330 core
uniform vec4 uColor;
uniform vec4 uMove;
uniform float uAngle;
in vec4 vertPos;

out vec4 vColor;
void main(void)
{

	gl_Position = vertPos;

	gl_Position.x = vertPos.x*cos(uAngle)-vertPos.y*sin(uAngle);
	gl_Position.y = vertPos.x*sin(uAngle)+vertPos.y*cos(uAngle);

	gl_Position = gl_Position+ uMove;
	vColor = uColor;
}