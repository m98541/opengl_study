#version 450 core

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform int uDrawMode;

in vec4 vertPos;
in vec4 aNorm;
in vec4 aColor;

in vec3 aTexCoord;
in vec3 aNormalCoord;

out vec4 vColor;
out vec4 vNorm;
out vec3 vTexCoord;
out vec3 vNormalCoord;
out vec4 vPos;

void main(void)
{

	vColor = aColor;
	vNorm = transpose(inverse(uView*uModel))*aNorm;
	vTexCoord = aTexCoord;
	vNormalCoord = aNormalCoord;

	vPos = vertPos;
	gl_Position = uProj*uView*uModel*vertPos;





}