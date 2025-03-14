#version 450 core


in vec4 vertexPos;
in vec4 aColor;
in vec2 aTexCoord;
in vec2 aTexCoordAirBalloon;

out vec4 vColor;
out vec2 vTexCoord;
out vec2 vTexCoordAirBalloon;

void main(void)
{
	gl_Position = vertexPos;
	gl_Position.z *= -1;


	vTexCoord = aTexCoord;

	vTexCoordAirBalloon = aTexCoordAirBalloon;
}