#version 330 core

in vec4 vCamPosition;

in vec4 vLightPosition;
in vec4 vLightAmbient;
in vec4 vLightDiffuse;
in vec4 vLightSpecular;
in vec4 vLightAttenuation;

in vec4 vMaterialAmbient;
in vec4 vMaterialDiffuse;
in vec4 vMaterialSpecular;
in float vMaterialShineness;

in vec4 vVertPos;
in vec4 vColor;
out vec4 fragColor;


void main(void)
{
	float d = length(vLightPosition.xyz - vVertPos.xyz);

	vec3 iUnit = normalize(vLightPosition.xyz - vVertPos.xyz);
	vec3 nUnit = normalize(vVertPos.xyz);

	vec3 rUnit = normalize(2*dot(nUnit,iUnit)*nUnit-iUnit);
	vec3 vUnit = vec3(normalize(vCamPosition.xyz - vVertPos.xyz));


	float attenuation = vLightAttenuation.x + vLightAttenuation.y*d + vLightAttenuation.z*d*d;

	vec4 iAmbient = vMaterialAmbient*vLightAmbient;
	vec4 iDiffuse = (vMaterialDiffuse*max(dot(iUnit,nUnit),0)*vLightDiffuse)/attenuation;
	vec4 iSpecular = (vMaterialSpecular*pow(max(dot(rUnit , vUnit),0),vMaterialShineness)*vLightSpecular)/attenuation;


	fragColor = (iAmbient+iDiffuse+iSpecular);
	fragColor.w = 1.F;

}