/*
2024-11-23 
파일의 이름인 bezier_surface 는 구현되었으나 추가적으로 구현하려고 하였던 rtt_mirror 의 경우 구현실패
bezier_surface 파일에 기능을 계속 추가하려다 보니 코드가 더티(? 혹은 지저분) 해져 파일 분리하였으나....
기존 코드에서 너무 많은 부분을 global 변수로 사용 하고 있어(대표적으로 vertex buf 를 global 로 선언 시켜서 프로그램 전체에서 이거 하나로 지지고 복은게 가장큰 잘못)
프로그램 분리시 각 cpp 파일의 함수간 독립성 유지가 너무 힘들어짐 
global 헤더로 기존 global 변수를 모든 cpp 에서 공통적으로 사용하려 하였으나 
global 이라는 파일 이름과 파일내부 코드의 괴리가 너무 큼 => 여기서 이제 기능 더추가하면 수습이 안될정도임...
그리고 애초에 기능을 이렇게 여러개를 추가할거 였으면 bezier_surface.cpp 가 core가 되면 안됨
core.cpp 를 따로 만들고 각 기능들은 
lib1.cpp <->lib1.h<-> core.cpp <->bezier_surface.h <-> bezier_surface.cpp 
위와같이 lib형태로 제작했어야함 ....
그리고 각 cpp 파일의 함수는 그 자체로 완결성이 있어야 했음ㅠㅠ
막 lib1이 갑자기 libA가 없으면 실행이 안되거나 이딴 경우가 생기면 안되는거

그래서 rtt_mirror기능은 여기에 추가 못하고
다음 플젝에서 만들거
*/

#ifndef DEFAULT_MACRO
#define GLM_ENABLE_EXPERIMENTAL
#define _CRT_SECURE_NO_WARNINGS
#endif // !DEFAULT_MACRO

#include "bezier_surface.h"
#include "basic_sphere_interface.h"
#include "basic_rtt_mirror_interface.h"	


system_clock::time_point curTime;
system_clock::time_point lastTime;
milliseconds durationTime;



void InitSurface(void)
{
	patch4x3x4f surfacePatch;
	
	MkPatch(basisQaud , surfacePatch);
	
	GLfloat theta = (durationTime.count() / 1000.F) * PI / 2;
	//control point 16
	surfacePatch.patch[0][0].y = ((sin(theta)) + 1.F) / 4.F;
	surfacePatch.patch[0][1].y = ((cos(theta)) + 1.F) / 4.F;
	surfacePatch.patch[0][2].y = ((cos(theta)) + 1.F) / 4.F;
	surfacePatch.patch[0][3].y = ((sin(theta)) + 1.F) / 4.F;

	surfacePatch.patch[1][0].y = ((cos(theta)) + 1.F) / 4.F;
	surfacePatch.patch[1][1].y = ((sin(theta)) + 1.F) / 4.F;
	surfacePatch.patch[1][2].y = ((sin(theta)) + 1.F) / 4.F;
	surfacePatch.patch[1][3].y = ((cos(theta)) + 1.F) / 4.F;

	surfacePatch.patch[2][0].y = ((cos(theta)) + 1.F) / 4.F;
	surfacePatch.patch[2][1].y = ((sin(theta)) + 1.F) / 4.F;
	surfacePatch.patch[2][2].y = ((sin(theta)) + 1.F) / 4.F;
	surfacePatch.patch[2][3].y = ((cos(theta)) + 1.F) / 4.F;

	surfacePatch.patch[3][0].y = ((sin(theta)) + 1.F) / 4.F;;
	surfacePatch.patch[3][1].y = ((cos(theta)) + 1.F) / 4.F;
	surfacePatch.patch[3][2].y = ((cos(theta)) + 1.F) / 4.F;;
	surfacePatch.patch[3][3].y = ((sin(theta)) + 1.F) / 4.F;
	surfaceVertIdxCnt = 0;
	surfaceBufSize = 0;
	surfaceBufSize = GenPatch(surfaceVertBuf , 5 , surfacePatch);
	//현재 곡률 보정 때문에 InitSurface를 계속 호출 해야 하는 상황s 
	//PrepareVBO(SQ_SURFACE_VERTEX_ID,surfaceVertBuf , surfaceBufSize);
	
}


void DrawInit(void)
{
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.F);
	glClearColor(0.F, 0.F, 0.F, 0.F);
	glDepthRange(-1.F, 1.F);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void UpadteSurfaceMVP(void)
{
	//init
	mat4 surfaceModel = mat4(1.F);
	mat4 surfaceView = mat4(1.F);
	mat4 surfaceProjection = mat4(1.F);
	mat4 surfaceMVP = mat4(1.F);

	surfaceModel = scale(surfaceModel, vec3(0.8F , 0.8F , 0.8F));
	surfaceModel = rotate(surfaceModel, ((PI / 180.0F)) , vec3(1.F , 0.F , 0.F));
	surfaceModel = translate(surfaceModel , vec3(0.F, -0.5F , 0.F));
	vec3 camPos = vec3(camModel * basisCamPos);
	surfaceView = lookAtRH(camPos, vec3(0.F, 0.F, 0.F), vec3(0.F, 1.F, 0.F));

	GLfloat aspect = MAIN_W /MAIN_H;
	GLfloat fovy = ((PI / 180.0F)) * 90.0F;

	surfaceProjection = frustum(-0.5F , 0.5F , -0.5F , 0.5F , 0.3F , 10.F);

	surfaceMVP = surfaceProjection * surfaceView * surfaceModel;
	
	GLuint locAtMVP = glGetUniformLocation(prog, "aMVP");
	GLuint locUniformLightInterpolation = glGetUniformLocation(prog, "uLightInterpolation");
	GLuint locUniformCampos = glGetUniformLocation(prog, "uCamPos");

	static int cnt = 0;
	glUniformMatrix4fv(locAtMVP, 1, GL_FALSE , value_ptr(surfaceMVP));
	glUniform3fv(locUniformCampos, 1, value_ptr(camPos));
	glUniform1f(locUniformLightInterpolation , 0.9F);

}

void DrawSurface(void)
{
	GLuint locAtPos = glGetAttribLocation(prog, "aPos");
	GLuint locAtColor = glGetAttribLocation(prog, "aColor");
	GLuint locAtNorm = glGetAttribLocation(prog , "aNorm");

	glEnableVertexAttribArray(locAtPos);
	glEnableVertexAttribArray(locAtColor);
	glEnableVertexAttribArray(locAtNorm);

	//현재 surfaceVertBuf 를 메인 메모리에서 실시간으로 곡률 보정해줘야 되서 vram 으로 옮기지 않고 사용함 
	//차후 glsl 에서 control point를 실시간으로 제어 할 수 있게하여서 vram을 사용하는 방향으로 개선이 필요함 

	glVertexAttribPointer(locAtPos , 4 , GL_FLOAT , GL_FALSE , sizeof(vertex) , value_ptr(surfaceVertBuf->vertPos));
	glVertexAttribPointer(locAtColor, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), value_ptr(surfaceVertBuf->vertColor));
	glVertexAttribPointer(locAtNorm , 4 , GL_FLOAT, GL_FALSE, sizeof(vertex) , value_ptr(surfaceVertBuf->vertNorm));



	glDrawArrays(GL_TRIANGLES , 0 , surfaceBufSize);
	glFlush();
}





void UpadteSphereMVP(void)
{
	//init
	mat4 sphereModel = mat4(1.F);
	mat4 sphereView = mat4(1.F);
	mat4 sphereProjection = mat4(1.F);
	mat4 sphereMVP = mat4(1.F);

	GLfloat theta = (durationTime.count() / 1000.F) * PI / 2;

	sphereLightPos.y = 1.F + ((sin(theta)) + 1.F) / 4.F;
	sphereModel = scale(sphereModel, vec3(0.05F, 0.1F, 0.05F));
	sphereModel = rotate(sphereModel, ((PI / 180.0F)), vec3(1.F, 0.F, 0.F));
	sphereModel = translate(sphereModel, vec3(sphereLightPos));
	vec3 camPos = vec3(camModel * basisCamPos);
	sphereView = lookAtRH(camPos, vec3(0.F, 0.F, 0.F), vec3(0.F, 1.F, 0.F));

	GLfloat aspect = MAIN_W / MAIN_H;
	GLfloat fovy = ((PI / 180.0F)) * 90.0F;

	sphereProjection = frustum(-0.5F, 0.5F, -0.5F, 0.5F, 0.3F, 10.F);
	
	sphereMVP = sphereProjection * sphereView * sphereModel;

	GLuint locAtMVP = glGetUniformLocation(prog, "aMVP");
	GLuint locUniformCampos = glGetUniformLocation(prog, "uCamPos");
	GLuint locUniformLightInterpolation = glGetUniformLocation(prog , "uLightInterpolation");

	static int cnt = 0;
	glUniformMatrix4fv(locAtMVP, 1, GL_FALSE, value_ptr(sphereMVP));
	glUniform3fv(locUniformCampos, 1, value_ptr(camPos));
	glUniform1f(locUniformLightInterpolation, 0.3F);

}





void DrawSphere()
{
	GLuint locAtPos = glGetAttribLocation(prog, "aPos");
	GLuint locAtColor = glGetAttribLocation(prog, "aColor");
	GLuint locAtNorm = glGetAttribLocation(prog, "aNorm");

	glEnableVertexAttribArray(locAtPos);
	glEnableVertexAttribArray(locAtColor);
	glEnableVertexAttribArray(locAtNorm);

	glVertexAttribPointer(locAtPos, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), value_ptr(sphereVertBuf->vertPos));
	glVertexAttribPointer(locAtColor, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), value_ptr(sphereVertBuf->vertColor));
	glVertexAttribPointer(locAtNorm, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), value_ptr(sphereVertBuf->vertNorm));

	glDrawArrays(GL_TRIANGLES, 0, sphereBufSize);
	glFlush();
}


int main(void)
{
	glfwInit();
	
	GLFWwindow* window = glfwCreateWindow(MAIN_W ,MAIN_H ,"bezierSurface.exe",NULL, NULL);

	glfwMakeContextCurrent(window);
	glewInit();
	glfwSetKeyCallback(window, keyFunc);
	InitGLSL("bezier_surface.vert" , "bezier_surface.frag");
	InitSurface();

	lastTime = system_clock::now();
	
	printf("surfaceBufSize : %d \n", surfaceBufSize);
	
	//init light(white light) set
	ShadingWhiteLight(sphereLightPos, 0.5F , 0.7F , 0.9F);

	GenerateSphere(4, sphereVertBuf, sphereBufSize);

	void InitFBOTBO();
	void InitMirrorTex();
	void InitMirrorFBO();

	while (!glfwWindowShouldClose(window))
	{
		curTime = system_clock::now();
		durationTime = duration_cast<milliseconds>(curTime - lastTime);
		InitSurface();
		DrawInit();
		UpadteSurfaceMVP();
		DrawSurface();
		UpadteSphereMVP();
		DrawSphere();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	
	return 0;
}


void keyFunc(GLFWwindow* window, int key, int scancode, int action, int mods)
{

	switch (key)
	{

	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window , GL_TRUE);
		break;
	case 'A':
		camModel = rotate(camModel, ((PI / 180.0F)) * 1.0F, vec3(0.F, 1.F, 0.F));
		break;
	case 'D':
		camModel = rotate(camModel, ((PI / 180.0F)) * 1.0F, vec3(0.F, -1.F, 0.F));
		break;
	
	case 'Z':
		if (basisCamPos.z > 0.1F)
			basisCamPos.z -= 0.1F;
		break;
	case 'X':
		if (basisCamPos.z <= 1.F)
			basisCamPos.z += 0.1F;
		break;
	default:
		break;
	}
	





}



//2024 10 26: 낼 drawSuface 구현 후 밑에 함수들 파일 나누기
//define default func 
char* LoadFile(const char* fileName){
	FILE* fp = fopen(fileName , "r");
	fseek(fp , 0 , SEEK_END);
	size_t len = ftell(fp);

	char* buf = (char*)malloc(sizeof(char) * (len+4));
	rewind(fp);
	size_t size = fread(buf , 1 , len , fp);
	buf[size] = NULL;
	fclose(fp);
	return buf;
}

//GLSL file Compile 
void DebugLog(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	printf("\nsource : %#x type : %#x id : %d severity : %#x \n", source, type, id, severity);
	printf("message : %s \n", message);
	fflush(stdout);

}

void InitGLSL(const char* vertFileName,const char* fragFileName) 
{

	GLint status = 0;
	char debugLogBuf[1024];

	const char* vertSource = LoadFile(vertFileName);
	vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert , 1 , &vertSource , NULL);
	glCompileShader(vert);
	

	glGetShaderiv(vert , GL_COMPILE_STATUS, &status);
	glGetShaderInfoLog(vert , 1024 ,NULL , debugLogBuf);
	printf("\nvert debug\n");
	printf("\n%s\n",vertSource);
	printf("vert compile_status : %d \n", status);
	printf("vert shader log : %s \n", debugLogBuf);
	
	const char* fragSource = LoadFile(fragFileName);
	frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag , 1 , &fragSource , NULL);
	glCompileShader(frag);
	

	glGetShaderiv(frag, GL_COMPILE_STATUS, &status);
	glGetShaderInfoLog(vert, 1024, NULL, debugLogBuf);
	printf("\nfrag debug\n");
	printf("\n%s\n", fragSource);
	printf("vert compile_status : %d \n", status);
	printf("vert shader log : %s \n", debugLogBuf);
	
	prog = glCreateProgram();
	glAttachShader(prog , vert);
	glAttachShader(prog , frag);
	glLinkProgram(prog);
	glUseProgram(prog);
	
	printf("\nprog debug\n");
	glGetProgramiv(prog, GL_LINK_STATUS, &status);
	printf("prog link_status : %d \n", status);
	glGetProgramInfoLog(prog, 1024, NULL, debugLogBuf);
	glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
	printf("prog validate_status : %d \n", status);
	printf("prog log : %s \n", debugLogBuf);
	glValidateProgram(prog);

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
	glDebugMessageCallback(DebugLog, NULL);


}

void BindVBO(GLuint id)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo[id]);
}

void PrepareVBO(GLuint id,const void* data, size_t size)
{
	static	GLuint id_max = 0;
	// 1000
	if (id_max == 0) // init 
	{
		id_max = 8;
		vbo = (uint*)malloc(sizeof(uint)*id_max);
		
	}
	else if (id >= id_max)
	{
		id_max *= 2;
		realloc(vbo , id_max);
		
	}
	glGenBuffers(id_max, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[id]);
	glBufferData(GL_ARRAY_BUFFER, 2 * size, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

}


//create bezier_curve func
void DivCurve(const mat4x3 p, mat4x3&l , mat4x3&r)
{
	// (mat^T * p^T)^T = p*mat 전치 행렬 성질
	l = p * matL;// 3x4 * 4x4
	r = p * matR;

}

patch4x3x4f transposePatch(patch4x3x4f basisPatch)
{
	patch4x3x4f trPatch = {mat4x3(1.F) ,mat4x3(1.F) ,mat4x3(1.F) ,mat4x3(1.F)};

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			trPatch.patch[i][j] = basisPatch.patch[j][i];
	
	return trPatch;
}

void MkPatch(mat4x3 quad ,patch4x3x4f &patch)// 4x3_f matrixt =>4x3x4_f matrix(flat surface)interpolation 
{
	GLfloat k = 0, t = 0;
	for (int i = 0; i < 4; i++)
	{

		if (i == 2)k += 0.25F;
		t = 0;
		for (int j = 0 ; j < 4; j++)
		{	
		
			if (j == 2)t += 0.25F;

			patch.patch[i][j] =  vec4((1 - k) * (1 - t), (1 - k) * t, k * (1 - t), k * t) * transpose(quad);

			t += 0.25;
		}
		
		k += 0.25;
	}
}

void MkQuad(vertex* vbuf,mat4x3 controlPoint ,GLuint& idxOffset)
{

	mat4x3 normalPoint = controlPoint;
	GLfloat ambientNormal = 0.2F;
	for (int i = 0; i < 4; i++)
	{
		if(normalPoint[i].y == 0)normalPoint[i].y = ambientNormal;

		if (normalPoint[i].y < 0)normalPoint[i].y = ambientNormal+normalPoint[i].y;

	}
	

	vec4 colorArr[5] = {
		WHITE , RED , BLUE , GREEN , YELLOW
	};
	static int colorCnt = 0;
	int colorIdx = 2;
	colorCnt++;
	vbuf[idxOffset + 0].vertPos = vec4(controlPoint[0], 1.F);
	vbuf[idxOffset + 1].vertPos = vec4(controlPoint[2], 1.F);
	vbuf[idxOffset + 2].vertPos = vec4(controlPoint[1], 1.F);
	vbuf[idxOffset + 0].vertColor = colorArr[colorIdx];
	vbuf[idxOffset + 1].vertColor = colorArr[colorIdx];
	vbuf[idxOffset + 2].vertColor = colorArr[colorIdx];
	vbuf[idxOffset + 0].vertNorm = vec4(normalPoint[0], 1.F);
	vbuf[idxOffset + 1].vertNorm = vec4(normalPoint[2], 1.F);
	vbuf[idxOffset + 2].vertNorm = vec4(normalPoint[1], 1.F);

	vbuf[idxOffset + 3].vertPos = vec4(controlPoint[2], 1.F);
	vbuf[idxOffset + 4].vertPos = vec4(controlPoint[0], 1.F);
	vbuf[idxOffset + 5].vertPos = vec4(controlPoint[3], 1.F);
	vbuf[idxOffset + 3].vertColor = colorArr[colorIdx];
	vbuf[idxOffset + 4].vertColor = colorArr[colorIdx];
	vbuf[idxOffset + 5].vertColor = colorArr[colorIdx];
	vbuf[idxOffset + 3].vertNorm = vec4(normalPoint[2], 1.F);
	vbuf[idxOffset + 4].vertNorm = vec4(normalPoint[0], 1.F);;
	vbuf[idxOffset + 5].vertNorm = vec4(normalPoint[3], 1.F);;
	
	idxOffset += 6;
}


GLuint GenPatch(vertex* vbuf, GLuint depth , patch4x3x4f basisPatch)
{
	
	if (depth < 1)
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				mat4x3 bezierMat = {
					basisPatch.patch[i	][j	 ], 
					basisPatch.patch[i+1][j	 ],
					basisPatch.patch[i+1][j+1],
					basisPatch.patch[i	][j+1] 
				};
				MkQuad(vbuf, bezierMat , surfaceVertIdxCnt);
				
			}

		}

	}
	else
	{
		// 변수 앞의 u,v(dot(u,v) = 0)는 분할 방향을 뜻 합니다.
		patch4x3x4f uLeftPatch , uRightPatch;
		patch4x3x4f vLeftPatch , vRightPatch;
		patch4x3x4f uvLeftPatch[2], uvRightPatch[2];

		for (int i = 0; i < 4; i++)
		{
			DivCurve(basisPatch.patch[i] , uLeftPatch.patch[i] , uRightPatch.patch[i]);
			DivCurve(uLeftPatch.patch[i] , uvLeftPatch[0].patch[i], uvRightPatch[0].patch[i]);//u LL LR
			DivCurve(uRightPatch.patch[i], uvLeftPatch[1].patch[i], uvRightPatch[1].patch[i]);//u RL RR
		}

		vLeftPatch = transposePatch(uLeftPatch);
		vRightPatch = transposePatch(uRightPatch);
		for (int i = 0; i < 4; i++)
		{
			DivCurve(vLeftPatch.patch[i], uvLeftPatch[0].patch[i], uvRightPatch[0].patch[i]);//v LL LR
			DivCurve(vRightPatch.patch[i], uvLeftPatch[1].patch[i], uvRightPatch[1].patch[i]);//v RL RR
		}
		
		GenPatch(vbuf, depth - 1, uvLeftPatch[0]);
		GenPatch(vbuf, depth - 1, uvRightPatch[0]);
		GenPatch(vbuf, depth - 1, uvLeftPatch[1]);
		GenPatch(vbuf, depth - 1, uvRightPatch[1]);

	}
	return surfaceVertIdxCnt;
}


void ShadingWhiteLight(vec4 lightPos,GLfloat ambient , GLfloat diffuse , GLfloat specular)
{
	light lightVal;
	material materalVal;
	
	lightVal.ambient = vec4(ambient , ambient , ambient , 1.F);
	lightVal.diffuse = vec4(diffuse , diffuse, diffuse, 1.F);
	lightVal.specular = vec4(specular , specular , specular , 1.F);
	lightVal.att = vec4(1.F ,0.F ,0.F ,1.F );
	lightVal.pos = lightPos;


	materalVal.ambient = vec4(ambient, ambient, ambient, 1.F);
	materalVal.diffuse = vec4(diffuse, diffuse, diffuse, 1.F);
	materalVal.specular = vec4(1.F, 1.F, 1.F, 1.F);
	materalVal.shineness = 64.F;

	GLuint locUniformLight = glGetUniformLocation(prog , "uLightVal.ambient");
	glUniform4fv(locUniformLight,1,value_ptr(lightVal.ambient));

	locUniformLight = glGetUniformLocation(prog, "uLightVal.diffuse");
	glUniform4fv(locUniformLight, 1, value_ptr(lightVal.diffuse));

	locUniformLight = glGetUniformLocation(prog, "uLightVal.specular");
	glUniform4fv(locUniformLight, 1, value_ptr(lightVal.specular));

	locUniformLight = glGetUniformLocation(prog, "uLightVal.att");
	glUniform4fv(locUniformLight, 1, value_ptr(lightVal.att));

	locUniformLight = glGetUniformLocation(prog, "uLightVal.pos");
	glUniform4fv(locUniformLight, 1, value_ptr(lightVal.pos));


	GLuint locUniformMaterial = glGetUniformLocation(prog , "uMaterialVal.ambient");
	glUniform4fv(locUniformMaterial, 1, value_ptr(materalVal.ambient));

	locUniformMaterial = glGetUniformLocation(prog, "uMaterialVal.diffuse");
	glUniform4fv(locUniformMaterial, 1, value_ptr(materalVal.diffuse));
	
	locUniformMaterial = glGetUniformLocation(prog, "uMaterialVal.specular");
	glUniform4fv(locUniformMaterial, 1, value_ptr(materalVal.specular));

	locUniformMaterial = glGetUniformLocation(prog, "uMaterialVal.shineness");
	glUniform1f(locUniformMaterial,materalVal.shineness);

	GLuint locUniformCampos = glGetUniformLocation(prog , "uCamPos");
	glUniform3fv(locUniformCampos , 1 , value_ptr(vec3(basisCamPos)));
	// 내일 여기에서 light material 값 uniform 으로 보내는거 구현해야함  그리고 projection 도 고치고
}