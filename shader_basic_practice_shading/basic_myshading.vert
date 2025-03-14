#version 330 core

uniform vec4 camPosition;



uniform vec4 lightPosition;
uniform vec4 lightAmbient;
uniform vec4 lightDiffuse;
uniform vec4 lightSpecular;
uniform vec4 lightAttenuation;

uniform vec4 materialAmbient;
uniform vec4 materialDiffuse;
uniform vec4 materialSpecular;
uniform float materialShineness;

uniform mat4 uModel;
uniform mat4 uProj;
uniform mat4 uView;

in vec4 vertPos;
in vec4 aColor;
out vec4 vColor;

out vec4 vVertPos;

out vec4 vCamPosition;

out vec4 vLightPosition;
out vec4 vLightAmbient;
out vec4 vLightDiffuse;
out vec4 vLightSpecular;
out vec4 vLightAttenuation;

out vec4 vMaterialAmbient;
out vec4 vMaterialDiffuse;
out vec4 vMaterialSpecular;
out float vMaterialShineness;


void main(void)
{
		vColor = aColor;
		gl_Position =uProj*uView*uModel*vertPos;//matProj*matView*uModel*vertPos;//
		vVertPos =  uModel*vertPos;

		vCamPosition = camPosition;

		vLightPosition = lightPosition;
		vLightAmbient = lightAmbient;
		vLightDiffuse = lightDiffuse;
		vLightSpecular = lightSpecular;
		vLightAttenuation = lightAttenuation;
	
		vMaterialAmbient = materialAmbient;
		vMaterialDiffuse = materialDiffuse;
		vMaterialSpecular = materialSpecular;
		vMaterialShineness = materialShineness;
}