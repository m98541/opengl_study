#version 330 core
uniform float uAngle;
in vec4 vertexPos;
in vec4 aColor;

out vec4 vColor;


void main(void)
{	
	
	gl_Position.x = vertexPos.x*cos(uAngle)-vertexPos.y*sin(uAngle);
	gl_Position.y = vertexPos.x*sin(uAngle)+vertexPos.y*cos(uAngle);
	gl_Position.zw = vertexPos.zw;
	gl_Position.xyz = 3*gl_Position.xyz;
	vColor = aColor;
}