#version 450 core
uniform mat4 uMatModel;
uniform mat4 uMatView;
uniform mat4 uMatProj;
in vec2  aTexCoord;;
in vec4 vertPos;
in vec4 aColor;

out vec2  vTexCoord;
out vec4 vColor;

void main(void)
{
	vColor = aColor;
	vTexCoord = aTexCoord;
	gl_Position = uMatProj*uMatView*uMatModel*vertPos ;

}