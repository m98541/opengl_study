#version 450 core
struct light{
	vec4 pos;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 att;

};

struct material{
	 vec4 ambient;
	 vec4 diffuse;
	 vec4 specular;
	 float shineness;

};

uniform light uLightVal;
uniform material uMaterialVal;


uniform float uLightInterpolation;

uniform vec3 uCamPos;

in vec4 vColor;
in vec4 vNorm;

out vec4 FragColor;



vec4 lightSum = vec4(0.F ,0.F ,0.F ,1.F);

void lightFunc(vec4 norm)
{
	vec3 L =  normalize(uLightVal.pos.xyz- norm.xyz) ;
	vec3 N = normalize(norm.xyz);
	vec3 V = normalize(uCamPos - norm.xyz);
	vec3 H = normalize((V+L)/2.F);

	vec4 ambient = uLightVal.ambient * uMaterialVal.ambient;

	float d = length(uLightVal.pos.xyz  - norm.xyz);

	float denom = uLightVal.att.x + uLightVal.att.y*d + uLightVal.att.z*d*d;

	vec4 diffuse = max(dot(L,N), 0.F)*uLightVal.diffuse*uMaterialVal.diffuse/denom;

	vec4 specular = pow(max(dot(N,H),0.F) , 2*uMaterialVal.shineness)*uLightVal.specular*uMaterialVal.specular/denom;

	lightSum = ambient + diffuse + specular;

	lightSum.w = 1.F;

}




void main(void)
{
	lightFunc(vNorm);
	float t = uLightInterpolation;

	FragColor =t*(lightSum * vColor) + (1-t)*vColor;

}