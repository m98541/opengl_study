#version 330 core
uniform mat4 uMat;
in vec4 vertPos;
in vec4 aColor;
out vec4 vColor;




void main(void)
{
		
		vColor = aColor;
		gl_Position = uMat* vertPos;
	
}