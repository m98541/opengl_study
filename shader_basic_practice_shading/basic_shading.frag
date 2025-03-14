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
uniform light vLightBuf[10];
uniform material vMaterialBuf;
uniform vec3 uCamPos;
uniform int lightBufLen;
uniform int uDrawMode;


uniform samplerCube texSampler;
uniform samplerCube normSampler;
uniform samplerCube surfaceSampler;

in vec4 vNorm;
in vec4 vColor;
in vec3 vTexCoord;
in vec3 vNormalCoord;

in vec4 vPos;
out vec4 FragColor;

vec4 lightSum = vec4(1.F ,1.F ,1.F ,1.F);

vec4 texSum = vec4(0.F ,0.F ,0.F ,1.F);

void texFunc(vec3 texCoord)
{
	texSum = texture(texSampler, texCoord);
}

void lightFunc(vec4 norm)
{
	for(int i =0 ; i < lightBufLen; i++){
		vec3 L =  normalize(vLightBuf[i].pos.xyz- norm.xyz) ;
		vec3 N = normalize(norm.xyz);
		vec3 V = normalize(uCamPos - norm.xyz);
		vec3 H = normalize((V+L)/2.F);

		vec4 ambient = vLightBuf[i].ambient * vMaterialBuf.ambient;

		float d = length(vLightBuf[i].pos.xyz  - norm.xyz);

		float denom = vLightBuf[i].att.x + vLightBuf[i].att.y*d + vLightBuf[i].att.z*d*d;

		vec4 diffuse = max(dot(L,N), 0.F)*vLightBuf[i].diffuse*vMaterialBuf.diffuse/denom;

		vec4 specular = pow(max(dot(N,H),0.F) , 2*vMaterialBuf.shineness)*vLightBuf[i].specular*vMaterialBuf.specular/denom;

		lightSum += ambient + diffuse + specular;
	}
	
	lightSum.w = 1.F;

}



void main(void)
{
	
	vec3 nVector = normalize(vNorm.xyz);
	vec3 tVector = normalize(cross(vec3(0,0,vPos.z) , nVector));
	vec3 bVector = cross(nVector, tVector);
	vec4 tbnNormal = texture(normSampler,vNormalCoord);

	tbnNormal = 2.F * tbnNormal - 1.F;

	tbnNormal.xyz = mat3(tVector ,bVector,nVector)*tbnNormal.xyz;




	vec3 texCoord;

	if(uDrawMode == 0)
	{
		texCoord = reflect(tbnNormal.xyz , normalize(uCamPos));
		vec4 surface = texture(surfaceSampler, texCoord);

		lightFunc(vNorm);
		float t = lightSum.x;
		float t2= 0.8;
		texSum = texture(texSampler, texCoord*vec3(1.F, -1.F ,1.F ));	
		texSum = (1-t+t2)*texSum +  (t-t2)*surface;
	
	}else
	{
		texCoord = reflect(vNorm.xyz , normalize(uCamPos));
		texSum = texture(texSampler, texCoord*vec3(1.F, -1.F ,1.F ));
	}


	texSum.w = 1.F;

		

		FragColor =texSum*lightSum;
	
	
	
}