#define _CRT_SECURE_NO_WARNINGS
#define GLM_ENABLE_EXPERIMENTAL
#define WIN_W 3840
#define WIN_H 2160

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <windows.h>
#include <string.h>
#include "teapot.h"


using namespace std;
using namespace glm;
using namespace std::chrono;



typedef struct vertex_t
{
	vec4 pos;
	vec4 color;
	vec4 norm;
}vertex;

int numVertTeapotIdx = 0;
vertex teapotBufMain[9999999];
vec3 teapotAVGpos;

typedef struct _quad {
	vec3 p0;
	vec3 p1;
	vec3 p2;
	vec3 p3;
}quad;

typedef struct _tri
{
	vec3 p0;
	vec3 p1;
	vec3 p2;
}tri;
typedef struct _patch4f {
	quad patch[4];
}patch4f;



typedef struct uniform4fv_t
{
	vec4 val;
	char uniformName[40];
}uniform4fv;

typedef struct uniform1f_t
{
	GLfloat val;
	char uniformName[40];
}uniform1f;


typedef struct light_t
{
	uniform4fv pos;
	uniform4fv ambient;
	uniform4fv diffuse;
	uniform4fv specular;
	uniform4fv att;

}light;

typedef struct material_t
{
	uniform4fv ambient;
	uniform4fv diffuse;
	uniform4fv specular;
	uniform1f  shineness;
}material;

typedef struct bmpFile_t
{
	char fileName[20];
	GLubyte* buf;
	int size;
	int height;
	int width;
}bmpFile;

light lightBuf[10];
material materialBuf[10];

int vert = 0;
int frag = 0;
int prog = 0;
int cullMode = 0;

vec2 dragStart;
vec2 dragCur;
vec2 dragMove;

int mousePressed = GL_FALSE;


vec4 dstOrigin;
GLfloat xAngle = 0.F;
GLfloat yAngle = 0.F;
GLfloat zAngle = 0.F;

mat4 mat4f = mat4(1.F);
mat4 matDrag = mat4(1.F);
mat4 matUpdate = mat4(1.F);
mat4 matRot = mat4(1.F);


vec4 vertexArr[] = {
	{ 0.0F ,  0.5F,  0.0F, 1.F},
	{ 0.5F , -0.3F,  0.0F, 1.F},
	{ 0.0F , -0.3F, -0.5F, 1.F},
	{-0.5F , -0.3F,  0.0F, 1.F},
	{ 0.0F , -0.3F,  0.5F, 1.F},
};
GLuint face[18] = {
	0, 1, 2, // red
	0, 2, 3, // green 
	0, 3, 4, // blue 
	0, 4, 1, // yellow
	1, 4, 3, // cyan
	1, 3, 2  // cyan
};
vec4 vertexPos[18];

vec4 faceColor[] = {
	{1.F , 0.F , 0.F , 1.F},
	{1.F , 0.F , 0.F , 1.F},
	{1.F , 0.F , 0.F , 1.F},

	{1.F , 1.F , 0.F , 1.F},
	{1.F , 1.F , 0.F , 1.F},
	{1.F , 1.F , 0.F , 1.F},

	{0.F , 1.F , 0.F , 1.F},
	{0.F , 1.F , 0.F , 1.F},
	{0.F , 1.F , 0.F , 1.F},

	{1.F , 0.F , 1.F , 1.F},
	{1.F , 0.F , 1.F , 1.F},
	{1.F , 0.F , 1.F , 1.F},

	{0.F , 0.F , 1.F , 1.F},
	{0.F , 0.F , 1.F , 1.F},
	{0.F , 0.F , 1.F , 1.F},

	{0.F , 1.F , 1.F , 1.F},
	{0.F , 1.F , 1.F , 1.F},
	{0.F , 1.F , 1.F , 1.F},


};
vertex vArr[18];
vertex iArr[5] = {
	{{ 0.0F ,  0.5F,  0.0F, 1.F},{1.F , 1.F ,1.F ,1.F}},
	{{ 0.5F , -0.3F,  0.0F, 1.F},{1.F , 0.F ,0.F ,1.F}},
	{{ 0.0F , -0.3F, -0.5F, 1.F},{0.F , 1.F ,0.F ,1.F}},
	{{-0.5F , -0.3F,  0.0F, 1.F},{0.F , 0.F ,1.F ,1.F}},
	{{ 0.0F , -0.3F,  0.5F, 1.F},{1.F , 1.F ,0.F ,1.F}}

};


int sphereVertCnt = 0;
int mapVertCnt = 0;
int teapotSize = 0;


vertex sphereBuf[9999999];
vertex sphereMapBuf[9999999];
vec4 sphereNorm[9999999];
vec3 coordBuf[9999999];
vec3 coordBufTeapot[9999999];

GLuint vbo[10];
GLuint ibo[1];
GLuint texObj[10];
GLuint texMap_h = 0;
GLuint texMap_w = 0;

mat4 matModel = mat4(1.F);
mat4 matView = mat4(1.F);
mat4 matProj = mat4(1.F);

vec2 tilt = vec2(0.F , 0.F);
GLfloat fov = 0.F;
vec3 camPos = vec3(0.F, 0.F, 3.F);

system_clock::time_point lastTime = system_clock::now();
system_clock::time_point curTime;


//mvp
mat4 myLookAtRH(vec3 eye, vec3 at, vec3 up);
mat4 myFrustumShear(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat frustumNear, GLfloat frustumFar);
mat4 myPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar, vec2 tilt);


void prepareVBO(int id, const void* data, size_t size);
void initTex(void);

void triSubdivision(int level, vec4 a, vec4 b, vec4 c);
void generateSphere(int level);


quat q0 = angleAxis(PI / 4.F, vec3(1, 0, 0));
quat q1 = angleAxis(PI / 4.F, vec3(0, 0, 1));

system_clock::time_point last_time = system_clock::now();

vec3 calcUnitVec(const vec2& raw);
mat4 myTrackball(vec3 u, vec3 v);
mat4 calcTracball(const vec2& start, const vec2& cur);
void cursorEnterFunc(GLFWwindow* win, int entered);
void cursorPosFunc(GLFWwindow* win, double xscr, double yscr);
GLubyte screenShotBuf[WIN_W * WIN_H * 4];

GLuint rboScreenShot_color = 0;
GLuint rboScreenShot_depth = 0;
GLuint fbo = 0;

void initFboRbo()
{
	glGenRenderbuffers(1, &rboScreenShot_color);
	glBindRenderbuffer(GL_RENDERBUFFER , rboScreenShot_color);
	glRenderbufferStorage(GL_RENDERBUFFER , GL_BGRA ,WIN_W,WIN_H);


	glGenRenderbuffers(1, &rboScreenShot_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboScreenShot_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIN_W, WIN_H);


	glFramebufferRenderbuffer(GL_FRAMEBUFFER  , GL_COLOR_ATTACHMENT1  , GL_RENDERBUFFER , rboScreenShot_color);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT , GL_RENDERBUFFER, rboScreenShot_depth);


}


void screenShotFunc()
{
	BITMAPFILEHEADER fh;
	BITMAPINFOHEADER ih;

	fh.bfType = 'MB';
	fh.bfSize = sizeof(screenShotBuf) + 54;
	fh.bfReserved1 = NULL;
	fh.bfReserved2 = NULL;
	

	ih.biSize = 40;
	ih.biWidth = WIN_W;
	ih.biHeight = WIN_H;
	ih.biPlanes = 1;
	ih.biBitCount = 32;
	ih.biCompression = 0;
	ih.biSizeImage = WIN_W * WIN_H;
	ih.biXPelsPerMeter = WIN_W;
	ih.biYPelsPerMeter = WIN_H;
	ih.biClrUsed = 0;
	ih.biClrImportant = 0;
	fh.bfOffBits = sizeof(fh) + sizeof(ih);

	HANDLE hFile;
	DWORD dwByte = 0;

	hFile = CreateFileA("screenShotTest.bmp", GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return;
	}

	glReadBuffer(GL_BACK);
	glReadPixels(0, 0, WIN_W, WIN_H, GL_BGRA, GL_UNSIGNED_BYTE, screenShotBuf);

	WriteFile(hFile  , &fh , sizeof(fh) ,&dwByte , NULL);
	printf("write filehead : %d \n", dwByte);
	WriteFile(hFile,  &ih, sizeof(ih), &dwByte, NULL); 
	printf("write infohead : %d \n", dwByte);


	WriteFile(hFile, &screenShotBuf, WIN_H * WIN_W * 4 , &dwByte, NULL);
	printf("write image : %d \n", dwByte);

	CloseHandle(hFile);
}

int i = 0;

void keyCall(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{

		switch (key)
		{
		
		case 'Q':
			fov -= ((GLfloat)3.141592 / 180.F) * 1.F;
			break;
		case 'E':
			fov += ((GLfloat)3.141592 / 180.F) * 1.F;
			break;
		case 'A':
			yAngle += ((GLfloat)3.141592 / 180.F) * 0.1F;
			quat q = angleAxis(yAngle, vec3(0.F, 1.F, 0.F));
			mat4 rot = toMat4(q);
			camPos = vec3(rot * vec4(camPos,1.F));
			break;
		case 'Z':
			tilt.x -= 0.01F;
			break;
		case 'X':
			tilt.x += 0.01F;
			break;
		case 'R':
			sphereVertCnt = 0;
			generateSphere(i++);
			prepareVBO(1, coordBuf, sphereVertCnt * 3 * sizeof(GLfloat));
			prepareVBO(0,sphereBuf, sphereVertCnt * 12 * sizeof(GLfloat));
			break;
		case 'P':
			
			screenShotFunc();
			
			break;

		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window , GL_TRUE);
			break;
		default:
			break;
		}

	}
}


void buttonFunc(GLFWwindow* win, int button, int action, int mods)
{
	double x, y;
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_1: printf("mouse button 1:"); break;
	case GLFW_MOUSE_BUTTON_2: printf("mouse button 2:"); break;
	case GLFW_MOUSE_BUTTON_3: printf("mouse button 3:"); break;
	default:
		break;
	}

	switch (action)
	{
	case GLFW_PRESS:
		printf("preassed\n");
		mousePressed = GL_TRUE;
		glfwGetCursorPos(win, &x, &y);
		dragStart = vec2((GLfloat)x, (GLfloat)y);

		break;
	case GLFW_RELEASE:
		printf("released :");
		mousePressed = GL_FALSE;
		glfwGetCursorPos(win, &x, &y);
		vec2 dragCur = vec2((GLfloat)x, (GLfloat)y);
		matDrag = calcTracball(dragStart, dragCur);
		mat4f = matDrag * matUpdate;
		matDrag = mat4(1.F);
		matUpdate = mat4f;

		break;
	default:
		break;
	}
	fflush(stdout);

}




void cullFace(int mode)
{
	glEnable(GL_CULL_FACE);

	switch (mode)
	{
	case 0:
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);
		break;
	case 1:
		glFrontFace(GL_CCW);
		glCullFace(GL_FRONT);
		break;
	case 2:
		glFrontFace(GL_CW);
		glCullFace(GL_FRONT);
		break;
	case 3:
		glFrontFace(GL_CW);
		glCullFace(GL_BACK);
		break;
	case 4:
		glFrontFace(GL_CCW);
		glCullFace(GL_FRONT_AND_BACK);
		break;
	default:
		break;
	}

}

void shadingLight(light L)
{
	GLuint locLightPos = glGetUniformLocation(prog, L.pos.uniformName);
	GLuint locLightAmbient = glGetUniformLocation(prog, L.ambient.uniformName);
	GLuint locLightDiffuse = glGetUniformLocation(prog, L.diffuse.uniformName);
	GLuint locLightSpecular = glGetUniformLocation(prog, L.specular.uniformName);
	GLuint locLightAttenuation = glGetUniformLocation(prog, L.att.uniformName);

	glUniform4fv(locLightPos, 1, value_ptr(L.pos.val));
	glUniform4fv(locLightAmbient, 1, value_ptr(L.ambient.val));
	glUniform4fv(locLightDiffuse, 1, value_ptr(L.diffuse.val));
	glUniform4fv(locLightSpecular, 1, value_ptr(L.specular.val));
	glUniform4fv(locLightAttenuation, 1, value_ptr(L.att.val));
}

void shadingMaterial(material M)
{

	GLuint locMaterialAmbient = glGetUniformLocation(prog, M.ambient.uniformName);
	GLuint locMaterialDiffuse = glGetUniformLocation(prog, M.diffuse.uniformName);
	GLuint locMaterialSpecular = glGetUniformLocation(prog, M.specular.uniformName);
	GLuint locMaterialShineness = glGetUniformLocation(prog, M.shineness.uniformName);

	glUniform4fv(locMaterialAmbient, 1, value_ptr(M.ambient.val));
	glUniform4fv(locMaterialDiffuse, 1, value_ptr(M.diffuse.val));
	glUniform4fv(locMaterialSpecular, 1, value_ptr(M.specular.val));
	glUniform1f(locMaterialShineness, M.shineness.val);
}

void shadingInit(){

	GLuint locLightBufLen = glGetUniformLocation(prog, "lightBufLen");
	glUniform1i(locLightBufLen , 1);
	
	lightBuf[0] = {
		{vec4(0.F , 0.5F , -3.F , 1.F),"vLightBuf[0].pos"},
		{vec4(0.6F , 0.6F , 0.6F , 1.F),"vLightBuf[0].ambient"},
		{vec4(0.7F , 0.7F , 0.7F , 1.F),"vLightBuf[0].diffuse"},
		{vec4(0.7F , 0.7F , 0.7F , 1.F),"vLightBuf[0].specular"},
		{vec4(1.F , 0.F , 0.F , 1.F),"vLightBuf[0].att"}
	};

	vec4 reflectivity= vec4(0.8F , 0.8F , 0.8F ,1.F);

	material m = {
		{reflectivity,"vMaterialBuf.ambient"},
		{reflectivity,"vMaterialBuf.diffuse"},
		{reflectivity,"vMaterialBuf.specular"},
		{64.F , "vMaterialBuf.shineness"}
	
	};

	shadingLight(lightBuf[0]);
	shadingLight(lightBuf[1]);
	shadingMaterial(m);

}


void update()
{

	
	matView = myLookAtRH(camPos , vec3(0.F, 0.F , 0.F) , vec3(0.F , 1.F , 0.F));

	float fovy = ((GLfloat)3.141592 / 180.0F) * 60.0F + fov;
	const GLfloat aspect = (GLfloat)WIN_W / (GLfloat)WIN_H;

	matProj = myPerspective(fovy , aspect, 0.5F, +20.F, tilt);

	GLuint locView = glGetUniformLocation(prog, "uView");
	GLuint locMatProj = glGetUniformLocation(prog, "uProj");


	
	glUniformMatrix4fv(locView,1, GL_FALSE,value_ptr(matView));
	glUniformMatrix4fv(locMatProj, 1, GL_FALSE, value_ptr(matProj));
	//myShadingInit(vec4(camPos,1.F));
	shadingInit();
		

}


void drawFunc()
{
	milliseconds duration = duration_cast<milliseconds>(curTime - lastTime);

	glEnable(GL_DEPTH_TEST);
	
	
	glClearDepth(1.F);
	glClearColor(0.F, 0.F, 0.F, 0.F);
	glDepthRange(-10.F, 10.F);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	

	GLuint locVertPos = glGetAttribLocation(prog, "vertPos");
	GLuint locColor = glGetAttribLocation(prog, "aColor");
	GLuint locNorm  = glGetAttribLocation(prog, "aNorm");
	GLuint locNormalCoord = glGetAttribLocation(prog , "aNormalCoord");
	GLuint locTexCoord = glGetAttribLocation(prog , "aTexCoord");
	GLuint locModel = glGetUniformLocation(prog, "uModel");
	GLuint locCamPos = glGetUniformLocation(prog, "uCamPos");
	GLuint locDrawMode = glGetUniformLocation(prog, "uDrawMode");
	GLuint locTexMode = glGetUniformLocation(prog, "texMode");

	glEnableVertexAttribArray(locTexCoord);
	glEnableVertexAttribArray(locNormalCoord);
	glEnableVertexAttribArray(locVertPos);
	glEnableVertexAttribArray(locColor);
	glEnableVertexAttribArray(locNorm);


	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(locVertPos, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*)0); // vboÀÇ offset À» ÁÜ
	glVertexAttribPointer(locColor, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
	glVertexAttribPointer(locNorm, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	mat4 matModelUnit = mat4f*scale(matModel,vec3(0.5F, 0.5F ,0.5F));
	matModelUnit = translate(matModelUnit , vec3(-1.5F , 0.F , 0.F));
	glUniformMatrix4fv(locModel, 1, GL_FALSE, value_ptr(matModelUnit));

	glUniform3fv(locCamPos,1,value_ptr(camPos));
	//glDrawElements(GL_TRIANGLES, sphereVertCnt, GL_UNSIGNED_INT, (GLvoid*)(0));
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	
	
	glVertexAttribPointer(locTexCoord, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glVertexAttribPointer(locNormalCoord, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glUniform1i(locDrawMode, DRAW_UNIT);
	glDrawArrays(GL_TRIANGLES, 0, sphereVertCnt);


	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	mat4 matModelMap = scale(matModel, vec3(6.F, 6.F, 6.F));
	glUniformMatrix4fv(locModel, 1, GL_FALSE, value_ptr(matModelMap));
	glVertexAttribPointer(locVertPos, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*)0); // vboÀÇ offset À» ÁÜ
	glVertexAttribPointer(locColor, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
	glVertexAttribPointer(locNorm, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glUniform1i(locDrawMode, DRAW_MAP);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glVertexAttribPointer(locTexCoord, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	GLuint locSampler = glGetUniformLocation(prog, "texSampler");

	glUniform1i(locSampler, 0);

	locSampler = glGetUniformLocation(prog, "normSampler");
	glUniform1i(locSampler, 1);

	locSampler = glGetUniformLocation(prog, "surfaceSampler");
	glUniform1i(locSampler, 2);
	glDrawArrays(GL_TRIANGLES, 0, mapVertCnt);


	glUniform1i(locTexMode, 1);


	matModelUnit = mat4(1.F);
	matModelUnit = mat4f * scale(matModelUnit, vec3(0.5F, 0.5F, 0.5F));
	matModelUnit = rotate(matModelUnit,-PI/2,vec3(1.F, 0.F ,0.F));
	matModelUnit = translate(matModelUnit,vec3(-teapotAVGpos.x + 1.5 , -teapotAVGpos.y, -teapotAVGpos.z));
	glUniformMatrix4fv(locModel, 1, GL_FALSE, value_ptr(matModelUnit));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glVertexAttribPointer(locVertPos, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*)0); // vboÀÇ offset À» ÁÜ
	glVertexAttribPointer(locColor, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
	glVertexAttribPointer(locNorm, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glUniform1i(locDrawMode, DRAW_UNIT);
	

	glDrawArrays(GL_TRIANGLES, 0, teapotSize);

	glUniform1i(locTexMode, 0);


	glFinish();

}


GLubyte* LoadBmp(const char* Path, int* Width, int* Height)
{
	HANDLE hFile;
	DWORD FileSize, dwRead;
	BITMAPFILEHEADER* fh = NULL;
	BITMAPINFOHEADER* ih;
	BYTE* pRaster;

	hFile = CreateFileA(Path, GENERIC_READ, 0, NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) {

		return NULL;

	}


	FileSize = GetFileSize(hFile, NULL);
	fh = (BITMAPFILEHEADER*)malloc(FileSize);
	ReadFile(hFile, fh, FileSize, &dwRead, NULL);
	CloseHandle(hFile);

	int len = FileSize - fh->bfOffBits;
	pRaster = (GLubyte*)malloc(len);
	memcpy(pRaster, (BYTE*)fh + fh->bfOffBits, len);

	

	ih = (BITMAPINFOHEADER*)((PBYTE)fh + sizeof(BITMAPFILEHEADER));
	*Width = ih->biWidth;
	*Height = ih->biHeight;

	free(fh);
	return pRaster;

}

char* FileLoad(const char* fileName)
{
	FILE* fp = fopen(fileName, "r");

	fseek(fp, 0, SEEK_END);
	size_t len = ftell(fp);
	char* buf = (char*)malloc(sizeof(char) * (len + 4));
	rewind(fp);
	size_t size = fread(buf, 1, len, fp);

	buf[size] = '\0';
	fclose(fp);
	return buf;
}


int main(void)
{
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(WIN_W/2, WIN_H/2, "cube_map", NULL, NULL);
	glfwMakeContextCurrent(window);
	glewInit();

	glfwSetKeyCallback(window, keyCall);
	glfwSetCursorEnterCallback(window, cursorEnterFunc);
	glfwSetCursorPosCallback(window, cursorPosFunc);
	glfwSetMouseButtonCallback(window, buttonFunc);

	const char* vertSource = FileLoad("basic_shading.vert");
	vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert, 1, &vertSource, NULL);
	glCompileShader(vert);

	const char* fragSource = FileLoad("basic_shading.frag");
	frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &fragSource, NULL);
	glCompileShader(frag);

	prog = glCreateProgram();
	glAttachShader(prog, vert);
	glAttachShader(prog, frag);
	glLinkProgram(prog);	
	glUseProgram(prog);

	vertex* teapotBuf = NULL;
	generateSphere(4);
	
	teapotSize = generateTeapot(1, teapotAVGpos);
	printf("teapot info : vert size: %d\n" , teapotSize);

	glGenBuffers(6, vbo);
	initTex();

	memcpy(sphereMapBuf, sphereBuf, sphereVertCnt * 12 * sizeof(GLfloat));
	mapVertCnt = sphereVertCnt * 12 * sizeof(GLfloat);
	prepareVBO(0,sphereBuf,sphereVertCnt * 12 * sizeof(GLfloat));
	prepareVBO(2, sphereMapBuf, mapVertCnt);
	prepareVBO(1, coordBuf, sphereVertCnt * 3 * sizeof(GLfloat));
	prepareVBO(3, coordBuf, mapVertCnt * 3 * sizeof(GLfloat));
	prepareVBO(4 , teapotBufMain , teapotSize * 12 * sizeof(GLfloat));
	prepareVBO(5, coordBufTeapot, teapotSize * 12 * sizeof(GLfloat));
	cout << "teapot init test" << endl;
	//for (int i = 0; i < teapotSize; i++)cout << to_string(teapotBufMain[i].pos) << endl;

	printf("finish vbo \n");

	initFboRbo();

	while (!glfwWindowShouldClose(window))
	{	

		curTime = system_clock::now();
		matModel = mat4(1.F);
		update();
		drawFunc();
		glfwSwapBuffers(window);
		glfwPollEvents();
		
	}
	
	glfwTerminate();

	return 0;
}

void initTex(void)
{
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	bmpFile imageCube[6];
	bmpFile normalMap;
	bmpFile surfaceMap;

	int i = 0;

	glGenTextures(3, texObj);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texObj[0]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texObj[1]);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texObj[2]);


	strcpy(imageCube[i].fileName, "mapbox_0.bmp");
	strcpy(normalMap.fileName , "normalMap.bmp");
	strcpy(surfaceMap.fileName, "metal.bmp");

	imageCube[i].buf = LoadBmp(imageCube[i].fileName, &imageCube[i].width, &imageCube[i].height);
	normalMap.buf = LoadBmp(normalMap.fileName , &normalMap.width , &normalMap.height);
	surfaceMap.buf = LoadBmp(surfaceMap.fileName, &surfaceMap.width, &surfaceMap.height);

	glTextureStorage2D(texObj[0], 1, GL_RGBA8, imageCube[0].width, imageCube[0].height);
	glTextureSubImage3D(texObj[0], 0, 0, 0,i, imageCube[i].width, imageCube[i].height,1, GL_BGR,GL_UNSIGNED_BYTE, imageCube[i].buf);

	glTextureStorage2D(texObj[1], 1, GL_RGBA8, normalMap.width , normalMap.height);
	glTextureSubImage3D(texObj[1], 0, 0,0 , i , normalMap.width , normalMap.height , 1, GL_BGR,GL_UNSIGNED_BYTE, normalMap.buf);

	glTextureStorage2D(texObj[2], 1, GL_RGBA8, surfaceMap.width, surfaceMap.height);
	glTextureSubImage3D(texObj[2], 0, 0, 0, i, surfaceMap.width, surfaceMap.height, 1, GL_BGR, GL_UNSIGNED_BYTE, surfaceMap.buf);


	for (i=1; i < 6; i++)
	{
		strcpy(imageCube[i].fileName, "mapbox_0.bmp");
		imageCube[i].fileName[7] += i;
		printf("load: %s \n", imageCube[i].fileName);
		imageCube[i].buf = LoadBmp(imageCube[i].fileName, &imageCube[i].width, &imageCube[i].height);
		glTextureSubImage3D(texObj[0], 0, 0, 0,i, imageCube[i].width, imageCube[i].height,1, GL_BGR, GL_UNSIGNED_BYTE, imageCube[i].buf);
		glTextureSubImage3D(texObj[1], 0, 0, 0, i, normalMap.width, normalMap.height, 1, GL_BGR, GL_UNSIGNED_BYTE, normalMap.buf);
		glTextureSubImage3D(texObj[2], 0, 0, 0, i, surfaceMap.width, surfaceMap.height, 1, GL_BGR, GL_UNSIGNED_BYTE, surfaceMap.buf);
	}

	glGenerateTextureMipmap(texObj[0]);
	glTexParameteri(texObj[0], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(texObj[0], GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenerateTextureMipmap(texObj[1]);
	glTexParameteri(texObj[1], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(texObj[1], GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenerateTextureMipmap(texObj[2]);
	glTexParameteri(texObj[1], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(texObj[1], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//uv mapping 

	

}


mat4 myLookAtRH(vec3 eye, vec3 at, vec3 up)
{
	vec3 p = eye;
	vec3 n = -normalize(at - eye);
	vec3 u = normalize(cross(up, n));
	vec3 v = cross(n, u);

	mat4 m = {
		vec4(u,-dot(u,p)),
		vec4(v,-dot(v,p)),
		vec4(n,-dot(n,p)),
		vec4(0.F , 0.F , 0.F ,1.F)
	};

	m = transpose(m);
	return m;

}
mat4 myFrustumShear(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat frustumNear, GLfloat frustumFar)
{
	mat4 mProj = {
		vec4((2 * frustumNear) / (right - left)  , 0.F , 0.F , 0.F),
		vec4(0.F , (2 * frustumNear) / (top - bottom) , 0.F , 0.F),
		vec4((right + left) / (right - left) , (top + bottom) / (top - bottom) , -(frustumFar + frustumNear) / (frustumFar - frustumNear) , -1.F),
		vec4(0.F , 0.F , -(2 * frustumFar * frustumNear) / (frustumFar - frustumNear) , 0.F)

	};

	return mProj;

}
mat4 myPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar, vec2 tilt)
{
	GLfloat top = zNear * tanf(fovy / 2.F);
	GLfloat right = aspect * top;
	GLfloat bottom = -top;
	GLfloat left = -right;
	return myFrustumShear(left + tilt.x, right + tilt.x, bottom + tilt.y, top + tilt.y, zNear, zFar);
}



void prepareVBO(int id , const void* data , size_t size)
{

	glBindBuffer(GL_ARRAY_BUFFER, vbo[id]);
	glBufferData(GL_ARRAY_BUFFER, 2 * size, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

}

float signArr[2] = { 1.F , -1.F };

void triSubdivision(int level, vec4 a, vec4 b, vec4 c)
{

	if (level <= 0)
	{
		sphereBuf[sphereVertCnt].pos = a;
		sphereBuf[sphereVertCnt + 1].pos = b;
		sphereBuf[sphereVertCnt + 2].pos = c;

		//vec4 color = vec4((GLfloat)(rand() % 1000) / 1000.F, (GLfloat)(rand() % 1000) / 1000.F, (GLfloat)(rand() % 1000) / 1000.F, 1.F);
		vec4 color = vec4(1.F, 1.F, 1.F, 1.F);
		sphereBuf[sphereVertCnt].color = color;
		sphereBuf[sphereVertCnt + 1].color = color;
		sphereBuf[sphereVertCnt + 2].color = color;

		sphereBuf[sphereVertCnt].norm = a;
		sphereBuf[sphereVertCnt + 1].norm = b;
		sphereBuf[sphereVertCnt + 2].norm = c;

		int idx = (int)(a.z < 0.F && b.z < 0.F && c.z < 0.F);
		float sign = signArr[idx];
		
		coordBuf[sphereVertCnt] = (vec3)a;
		coordBuf[sphereVertCnt+1] = (vec3)b;
		coordBuf[sphereVertCnt+2] = (vec3)c;

		/*
		
		coordBuf[sphereVertCnt].x = (atan2f(sign * a.x, sign * a.z)+PI*(float)idx) / ((float)PI * 2.F) ;
		coordBuf[sphereVertCnt].y = (0.5 + asinf(a.y) / PI);

		
		coordBuf[sphereVertCnt + 1].x = (atan2f(sign * b.x, sign * b.z)+PI*(float)idx) / ((float)PI * 2.F);
		coordBuf[sphereVertCnt + 1].y = (0.5 + asinf(b.y) / PI);

		coordBuf[sphereVertCnt + 2].x = (atan2f(sign * c.x, sign * c.z)+PI*(float)idx) / ((float)PI * 2.F);
		coordBuf[sphereVertCnt + 2].y = (0.5 + asinf(c.y) / PI);
		*/
		sphereVertCnt += 3;
	}
	else
	{
		vec4 ab = vec4(normalize((vec3(a) + vec3(b)) / 2.F), 1.F);
		vec4 bc = vec4(normalize((vec3(c) + vec3(b)) / 2.F), 1.F);
		vec4 ca = vec4(normalize((vec3(c) + vec3(a)) / 2.F), 1.F);



		triSubdivision(level - 1, a, ab, ca);
		triSubdivision(level - 1, b, bc, ab);
		triSubdivision(level - 1, c, ca, bc);
		triSubdivision(level - 1, ab, bc, ca);
	}
}



void generateSphere(int level)
{
	vec4 tetrahedron[6] = {
		{1.F , 0.F , 0.F , 1.F},//v0
		{0.F , 1.F , 0.F , 1.F},//v1
		{-1.F , 0.F , 0.F , 1.F},//v3
		{0.F , -1.F , 0.F , 1.F},
		{0.F , 0.F ,  1.F , 1.F},//v4
		{0.F , 0.F , -1.F , 1.F}// v5
	};

	triSubdivision(level, tetrahedron[0], tetrahedron[1], tetrahedron[4]);
	triSubdivision(level, tetrahedron[1], tetrahedron[2], tetrahedron[4]);
	triSubdivision(level, tetrahedron[2], tetrahedron[3], tetrahedron[4]);
	triSubdivision(level, tetrahedron[3], tetrahedron[0], tetrahedron[4]);
	triSubdivision(level, tetrahedron[1], tetrahedron[0], tetrahedron[5]);
	triSubdivision(level, tetrahedron[2], tetrahedron[1], tetrahedron[5]);
	triSubdivision(level, tetrahedron[3], tetrahedron[2], tetrahedron[5]);
	triSubdivision(level, tetrahedron[0], tetrahedron[3], tetrahedron[5]);

}

vec3 calcUnitVec(const vec2& raw)
{
	vec2 scr;
	scr.x = clamp(raw.x, 0.F, (float)WIN_W);
	scr.y = clamp(raw.y, 0.F, (float)WIN_H);

	const GLfloat radius = sqrtf(WIN_W * WIN_W + WIN_H * WIN_H) / 2;
	vec3 v;
	v.x = (scr.x - WIN_W / 2.F) / radius;
	v.y = (WIN_H / 2.F - scr.y) / radius;
	v.z = sqrtf(1.F - v.x * v.x - v.y * v.y);

	return v;

}

mat4 myTrackball(vec3 u, vec3 v)
{
	if (u == v) return mat4(1.F);
	vec3 n;
	GLfloat cosTheta, sinTheta, theta;
	vec4 q;
	n.x = u.y * v.z - u.z * v.y;
	n.y = u.z * v.x - u.x * v.z;
	n.z = u.x * v.y - u.y * v.x;

	cosTheta = u.x * v.x + u.y * v.y + u.z * v.z;
	sinTheta = sqrtf(n.x * n.x + n.y * n.y + n.z * n.z);

	theta = atan2f(sinTheta, cosTheta);

	n.x /= sinTheta;
	n.y /= sinTheta;
	n.z /= sinTheta;

	q.x = sinf(theta / 2.F) * n.x;
	q.y = sinf(theta / 2.F) * n.y;
	q.z = sinf(theta / 2.F) * n.z;
	q.w = cosf(theta / 2.F);

	mat4 matA;
	mat4 matB;
	matA[0][0] = q.w; matA[1][0] = q.z; matA[2][0] = -q.y; matA[3][0] = q.x;
	matA[0][1] = -q.z; matA[1][1] = q.w; matA[2][1] = q.x; matA[3][1] = q.y;
	matA[0][2] = q.y; matA[1][2] = -q.x; matA[2][2] = q.w; matA[3][2] = q.z;
	matA[0][3] = -q.x; matA[1][3] = -q.y; matA[2][3] = -q.z; matA[3][3] = q.w;

	matB[0][0] = q.w; matB[1][0] = q.z; matB[2][0] = -q.y; matB[3][0] = -q.x;
	matB[0][1] = -q.z; matB[1][1] = q.w; matB[2][1] = q.x; matB[3][1] = -q.y;
	matB[0][2] = q.y; matB[1][2] = -q.x; matB[2][2] = q.w; matB[3][2] = -q.z;
	matB[0][3] = q.x; matB[1][3] = q.y; matB[2][3] = q.z; matB[3][3] = q.w;


	matA = transpose(matA);
	matB = transpose(matB);


	return matA * matB;

}

mat4 calcTracball(const vec2& start, const vec2& cur)
{
	vec3 org = calcUnitVec(start);
	vec3 dst = calcUnitVec(cur);
	quat q = rotation(org, dst);
	mat4 m = toMat4(q);

	return myTrackball(org, dst);;
}


void cursorEnterFunc(GLFWwindow* win, int entered)
{
	printf("cursor %s the window\n", (entered == GL_FALSE) ? "leaving" : "entered");
	fflush(stdout);

}


void cursorPosFunc(GLFWwindow* win, double xscr, double yscr)
{
	if (mousePressed == GL_TRUE)
	{
		vec2 dragCur = vec2((GLfloat)xscr, (GLfloat)yscr);
		matDrag = calcTracball(dragStart, dragCur);
		mat4f = matDrag * matUpdate;

	}

}







vec4 clear = vec4(0.F, 0.F, 0.F, 0.F);
vec4 white = vec4(1.F, 1.F, 1.F, 1.F);
vec4 red = vec4(1.F, 0.F, 0.F, 1.F);
vec4 blue = vec4(0.F, 0.F, 1.F, 1.F);
vec4 green = vec4(0.F, 1.F, 0.F, 1.F);
vec4 yellow = vec4(1.F, 1.F, 0.F, 1.F);

vec3 sumVertex = vec3(0.F , 0.F ,0.F);

void mkQuad(vertex* vbuf, quad q, vec4 color)
{
	// t
	vbuf[numVertTeapotIdx].pos = vec4(q.p0, 1.F);
	vbuf[numVertTeapotIdx + 1].pos = vec4(q.p2, 1.F);
	vbuf[numVertTeapotIdx + 2].pos = vec4(q.p1, 1.F); 
	vbuf[numVertTeapotIdx + 0].color = red;
	vbuf[numVertTeapotIdx + 1].color = blue;
	vbuf[numVertTeapotIdx + 2].color = green;

	// t'
	vbuf[numVertTeapotIdx + 3].pos = vec4(q.p2, 1.F);
	vbuf[numVertTeapotIdx + 4].pos = vec4(q.p0, 1.F);
	vbuf[numVertTeapotIdx + 5].pos = vec4(q.p3, 1.F);
	vbuf[numVertTeapotIdx + 3].color = red;
	vbuf[numVertTeapotIdx + 4].color = blue;
	vbuf[numVertTeapotIdx + 5].color = green;
	
	vec3 n0 = normalize(cross(q.p1 - q.p0, q.p2 - q.p0));
	vec3 n1 = normalize(cross(q.p3 - q.p2, q.p0 - q.p2));
	vec3 n = (1.F) * normalize(cross(q.p1 - q.p2, q.p3 - q.p2));
	vec3 m = (0.25F) * (q.p0 + q.p2 + q.p1 + q.p3);
	
	

	vbuf[numVertTeapotIdx + 0].norm = vec4((q.p2 - m)+n, 1.F);
	vbuf[numVertTeapotIdx + 1].norm = vec4((q.p0 - m)+n, 1.F);
	vbuf[numVertTeapotIdx + 2].norm = vec4((q.p3 - m)+n, 1.F);
			
	vbuf[numVertTeapotIdx + 3].norm = vec4((q.p0 - m)+n, 1.F);
	vbuf[numVertTeapotIdx + 4].norm = vec4((q.p2 - m)+n, 1.F);
	vbuf[numVertTeapotIdx + 5].norm = vec4((q.p1 - m)+n, 1.F);
	
	for (int i = 0; i < 6; i++)
		sumVertex += vec3(vbuf[numVertTeapotIdx+i].pos.x , vbuf[numVertTeapotIdx + i].pos.y , vbuf[numVertTeapotIdx + i].pos.z);

	numVertTeapotIdx += 6;
}

void divCurve(const quad p, quad& l, quad& r )
{
	l.p0 = p.p0;
	r.p3 = p.p3;
	l.p1 = (p.p0 + p.p1) * 0.5F;
	r.p2 = (p.p2 + p.p3) * 0.5F;
	l.p2 = (l.p1 + (p.p1 + p.p2) * 0.5F) * 0.5F;
	r.p1 = (r.p2 + (p.p1 + p.p2) * 0.5F) * 0.5F;
	l.p3 = (l.p2 + r.p1) * 0.5F;
	r.p0 = l.p3;


}


patch4f transposePatch(patch4f p)
{
	quad qt[4] = {
		{p.patch[0].p0 , p.patch[1].p0  , p.patch[2].p0  ,p.patch[3].p0 },
		{p.patch[0].p1 , p.patch[1].p1  , p.patch[2].p1  ,p.patch[3].p1 },
		{p.patch[0].p2 , p.patch[1].p2  , p.patch[2].p2  ,p.patch[3].p2 },
		{p.patch[0].p3 , p.patch[1].p3  , p.patch[2].p3  ,p.patch[3].p3 },
	};
	patch4f pt = {
		qt[0] , qt[1] ,qt[2] , qt[3]
	};
	return pt;
}

void generatePatch(GLuint level, patch4f p, int mode)
{

	

	if (level < 1)
	{
		vec4 color = vec4(1.F, 1.F, 1.F, 1.F);
		
		for (int j = 0; j < 3; j++)
		{
			quad tempQuad0 = { p.patch[j].p0 , p.patch[j].p1 , p.patch[j+1].p1 ,  p.patch[j + 1].p0};
			quad tempQuad1 = { p.patch[j].p1 , p.patch[j].p2 , p.patch[j + 1].p2 ,  p.patch[j + 1].p1 };
			quad tempQuad2 = { p.patch[j].p2 , p.patch[j].p3 , p.patch[j + 1].p3 ,  p.patch[j + 1].p2 };
			


			mkQuad(teapotBufMain, tempQuad0, color);
			mkQuad(teapotBufMain, tempQuad1, color);
			mkQuad(teapotBufMain, tempQuad2, color);
		
		}
				
	
		
	}
	else
	{
		patch4f ul, ur, vl, vr, l1 , l2 ,r1, r2;
		// u ¹æÇâ 
		patch4f  l , r;
		for (int i = 0; i < 4; i++)
		{
			
			divCurve(p.patch[i], ul.patch[i], ur.patch[i]);
			divCurve(ul.patch[i], l1.patch[i] , r1.patch[i]);
			divCurve(ur.patch[i], l2.patch[i] , r2.patch[i]);

		}
		
		vl = transposePatch(ul);
		vr = transposePatch(ur);

		for (int i = 0; i < 4; i++)
		{

			divCurve(vl.patch[i], l1.patch[i], r1.patch[i]);
			divCurve(vr.patch[i], l2.patch[i], r2.patch[i]);

		}
		
	
		
		generatePatch(level - 1, l1, 1);
		generatePatch(level - 1, r1 , 1);
		generatePatch(level - 1, l2, 1);
		generatePatch(level - 1, r2, 1);
		
	

	}
}


int generateTeapot(int level, vec3 &avgVal)
{
	for (int i = 0; i < kTeapotNumPatches; ++i)
	{

		vec3 v[16]; // control point
		GLfloat scale = 0.5f;
		for (int j = 0; j < 16; ++j)
		{
			int idx = teapotPatchesData[i][j] - 1;
			v[j] = vec3(scale, scale, scale) * vec3(teapotVerticesData[idx][0], teapotVerticesData[idx][1], teapotVerticesData[idx][2]);
		}
		quad q[4] = {
			{v[0] , v[1] , v[2] , v[3]},
			{v[4] , v[5] , v[6] , v[7]},
			{v[8] , v[9] , v[10] , v[11]},
			{v[12] , v[13] , v[14] , v[15]},
		};

		quad qT[4] = {
			{v[0] , v[4] , v[8] , v[12]},
			{v[1] , v[5] , v[9] , v[13]},
			{v[2] , v[6] , v[10] , v[14]},
			{v[3] , v[7] , v[11] , v[15]},
		};
		patch4f cp = {
			q[0] , q[1] , q[2], q[3]
		};
		patch4f cpT = {
			qT[0] , qT[1] , qT[2], qT[3]
		};

		generatePatch(level, cp ,0);
	}

	printf("finish generate teapot:%d(size) \n", numVertTeapotIdx);
	avgVal = vec3(sumVertex.x/ numVertTeapotIdx , sumVertex.y / numVertTeapotIdx , sumVertex.z / numVertTeapotIdx);
	return numVertTeapotIdx;
}