#define _CRT_SECURE_NO_WARNINGS
#define GLM_ENABLE_EXPERIMENTAL

#define WIN_W 1024
#define WIN_H 720


#define OBJ_VBO_PANEL_ID 0
#define OBJ_VBO_CUBE_ID 1
#define OBJ_VBO_PYRAMID_ID 2

#define OBJ_VBO_RCUBE_ID 3
#define OBJ_VBO_RPYRAMID_ID 4

#define COLOR_ID_HL_OFF 0
#define COLOR_ID_HL_ON 1
#define COLOR_ID_HOLE_PANEL_OUT 2
#define COLOR_ID_HOLE_PANEL_IN 3


#define PI 3.141592F

#include <GL/glew.h>
#include <GLFW/glfw3.h>


#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib,"opengl32.lib")

#include <GLM/glm.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtx/string_cast.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <chrono>

using namespace std;
using namespace glm;
using namespace std::chrono;

typedef struct _vertex
{
	vec4 pos;
	vec4 color;
}vertex;

typedef struct _face
{
	GLuint face[3];
	vec4 faceColor;
}face;



vec4 vertPosPyramid[5] = {
	{ 0.0F ,  0.5F,  0.0F, 1.F},
	{ 0.5F , -0.3F,  0.0F, 1.F},
	{ 0.0F , -0.3F, -0.5F, 1.F},
	{-0.5F , -0.3F,  0.0F, 1.F},
	{ 0.0F , -0.3F,  0.5F, 1.F}
};

face facePyramid[6] = {
	{{0,1,2} , {1.F , 0.F , 0.F , 1.F}},
	{{0,2,3} , {0.F , 1.F , 0.F , 1.F}},
	{{0,3,4} , {0.F , 0.F , 1.F , 1.F}},
	{{0,4,1} , {1.F , 1.F , 0.F , 1.F}},
	{{1,4,3} , {1.F , 0.F , 1.F , 1.F}},
	{{1,3,2} , {0.F , 1.F , 1.F , 1.F}}

};

vec4 vertPosCube[8] = {
	{-0.5F , -0.5F , 0.5F , 1.F},
	{ 0.5F , -0.5F , 0.5F , 1.F},
	{ 0.5F , -0.5F ,-0.5F , 1.F},
	{-0.5F , -0.5F ,-0.5F , 1.F},

	{-0.5F ,  0.5F , 0.5F , 1.F},
	{ 0.5F ,  0.5F , 0.5F , 1.F},
	{ 0.5F ,  0.5F ,-0.5F , 1.F},
	{-0.5F ,  0.5F ,-0.5F , 1.F}
};

face faceCube[12] = {
	{{0,3,2}, {0.F ,0.F ,1.F ,1.F}},
	{{0,2,1}, {0.F ,0.F ,1.F ,1.F}},

	{{1,2,6}, {0.F ,1.F ,0.F ,1.F}},
	{{1,6,5}, {0.F ,1.F ,0.F ,1.F}},

	{{2,3,7}, {1.F ,0.F ,0.F ,1.F}},
	{{2,7,6}, {1.F ,0.F ,0.F ,1.F}},

	{{3,0,4}, {1.F ,1.F ,0.F ,1.F}},
	{{3,4,7}, {1.F ,1.F ,0.F ,1.F}},

	{{1,5,4}, {1.F ,0.F ,1.F ,1.F}},
	{{1,4,0}, {1.F ,0.F ,1.F ,1.F}},

	{{4,5,6}, {0.F ,1.F ,1.F ,1.F}},
	{{4,6,7}, {0.F ,1.F ,1.F ,1.F}}
};

vec4 panelColor= vec4(1.F);

vertex Panel[6] = {
	{{-1.F ,+1.F , 0.F , 1.F} , panelColor},
	{{+1.F ,+1.F , 0.F , 1.F} , panelColor},
	{{+1.F ,-1.F , 0.F , 1.F} , panelColor},

	{{-1.F , 1.F , 0.F , 1.F} , panelColor},
	{{+1.F ,-1.F , 0.F , 1.F} , panelColor},
	{{-1.F ,-1.F , 0.F , 1.F} , panelColor},

};


system_clock::time_point last_time; 
system_clock::time_point cur_time;
milliseconds elapsedTimeMSEC;
GLfloat globalTheta = 0.F;

GLuint vert = 0;
GLuint frag = 0;
GLuint prog = 0;

GLuint objVBOArr[10] = {0,};

vertex objCube[36];
GLuint faceCubeSize = 12;
GLuint objCubeSize = 36;

vertex objPyramid[24];
GLuint facePyramidSize = 6;
GLuint objPyramidSize = 18;


void prepareVBO(GLuint* vboArr , GLuint idx , size_t vbufSize , vertex* vbuf)
{
	glBindBuffer(GL_ARRAY_BUFFER, vboArr[idx]);
	glBufferData(GL_ARRAY_BUFFER, 2*vbufSize, NULL , GL_STATIC_DRAW );
	glBufferSubData(GL_ARRAY_BUFFER, 0, vbufSize, vbuf);
}

void initObject(vertex* obj,GLuint faceSize, vec4* pos, face* faceObj)
{
	for (GLuint idx = 0; idx < faceSize; idx++)
	{
		obj[idx*3].pos = pos[faceObj[idx].face[0]];
		obj[idx*3+1].pos = pos[faceObj[idx].face[1]];
		obj[idx*3+2].pos = pos[faceObj[idx].face[2]];

		obj[idx*3].color = faceObj[idx].faceColor;
		obj[idx*3+1].color = faceObj[idx].faceColor;
		obj[idx*3+2].color = faceObj[idx].faceColor;
	}

};



//file
GLubyte* loadBmp(const char* fileName, GLuint* width, GLuint* height)
{

	HANDLE hFile;
	DWORD fileSize, dwRead;
	BITMAPFILEHEADER* fh = NULL;
	BITMAPINFOHEADER* ih = NULL;
	BYTE* pRaster;

	hFile = CreateFileA(fileName ,  GENERIC_READ , 0 , NULL , OPEN_EXISTING , FILE_ATTRIBUTE_NORMAL , NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}

	fileSize = GetFileSize(hFile , NULL);
	fh = (BITMAPFILEHEADER*)malloc(fileSize);
	ReadFile(hFile , fh , fileSize , &dwRead , NULL);
	CloseHandle(hFile);

	int len = fileSize - fh->bfOffBits;
	pRaster = (GLubyte*)malloc(len);
	memcpy(pRaster , (BYTE*)fh + fh->bfOffBits , len);

	ih = (BITMAPINFOHEADER*)((PBYTE)fh + sizeof(BITMAPFILEHEADER));
	*width = ih->biWidth;
	*height = ih->biHeight;

	free(fh);
	return pRaster;

}

void saveBmp(GLuint width, GLuint height,GLubyte* bgra,const char* fileName)
{
	BITMAPFILEHEADER fh;
	BITMAPINFOHEADER ih;

	fh.bfType = 'MB';
	fh.bfSize = sizeof(GLubyte)*width*height + 54;
	fh.bfReserved1 = NULL;
	fh.bfReserved2 = NULL;

	ih.biSize = 40;
	ih.biWidth = width;
	ih.biHeight = height;
	ih.biPlanes = 1;
	ih.biBitCount = 32;
	ih.biCompression = 0;
	ih.biSizeImage = width * height;
	ih.biXPelsPerMeter = width;
	ih.biYPelsPerMeter = height;
	ih.biClrUsed = 0;
	ih.biClrImportant = 0;
	fh.bfOffBits = sizeof(fh) + sizeof(ih);

	HANDLE hFile;
	DWORD dwByte = 0;

	hFile = CreateFileA(fileName, GENERIC_WRITE , 0 , NULL , OPEN_ALWAYS , FILE_ATTRIBUTE_NORMAL , NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return;
	}

	WriteFile(hFile, &fh, sizeof(fh), &dwByte, NULL);
	printf("write filehead : %d \n", dwByte);
	WriteFile(hFile, &ih, sizeof(ih), &dwByte, NULL);
	printf("write infohead : %d \n", dwByte);


	WriteFile(hFile, bgra, width * height * 4, &dwByte, NULL);
	printf("write image : %d \n", dwByte);

	CloseHandle(hFile);
}

char* FileLoad(const char* fileName)
{
	FILE* fp = fopen(fileName , "r");
	fseek(fp , 0 , SEEK_END);
	size_t len = ftell(fp);
	char* buf = (char*)malloc(sizeof(char)*(len+4));

	rewind(fp);

	size_t size = fread(buf, 1 , len , fp);
	buf[size] = '\0';
	fclose(fp);
	return buf;

}


//screenshot func


bool scst = 0; // stencil area screenshot sw 

GLuint rbo_color = 0;
GLuint rbo_depth = 0;
GLuint rbo_stencil = 0;

GLuint fbo_screenShot = 0;

const GLenum screenshotAttBuffer[3] = {
	GL_COLOR_ATTACHMENT0,
	GL_DEPTH_ATTACHMENT,
	GL_STENCIL_ATTACHMENT
};
void initScreenshotFBO_RBO()
{
	glGenRenderbuffers(1, &rbo_color);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_color);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_BGRA, WIN_W, WIN_H);

	glGenRenderbuffers(1, &rbo_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIN_W, WIN_H);

	glGenRenderbuffers(1, &rbo_stencil);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_stencil);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_COMPONENTS, WIN_W, WIN_H);

	glGenFramebuffers(1, &fbo_screenShot);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_screenShot);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo_color);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_stencil);


	glDrawBuffers(3, screenshotAttBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void screnshotStencil()
{
	static GLubyte buf[WIN_H][WIN_W];
	glReadPixels(0, 0, WIN_W, WIN_H, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, (void*)buf);
	static GLubyte bmpBuf[WIN_H][WIN_W][4];// bgra

	int value = 0;
	GLubyte val = 0;

	for (int y = 0; y < WIN_H; y++)
	{
		for (int x = 0; x < WIN_W; x++)
		{
			value = buf[y][x] * 32 + 127;
			val = (value > 255) ? (GLubyte)0xFF : (GLubyte)value;

			
			bmpBuf[y][x][0] = val; bmpBuf[y][x][1] = val; bmpBuf[y][x][2] = val;
			bmpBuf[y][x][3] = 0xFF;//alpha
		}
		
	}
		

	saveBmp(WIN_W, WIN_H, (GLubyte*)bmpBuf, "stencil_screenshot.bmp");
}


mat4 matI = mat4(1.F);
mat4 matModelArr[10];
mat4 matView = matI;
mat4 matProj = matI;
mat4 matMVP = matI;
GLfloat correctionOrb = 4.299998F;
GLfloat correctionRot = 4.269998F;
void updateMVP()
{
	for (int i = 0; i < 10; matModelArr[i++] = matI);

	matModelArr[OBJ_VBO_PANEL_ID] = translate(matModelArr[OBJ_VBO_PANEL_ID], vec3(-0.2F, 0.F, 0.2F));
	matModelArr[OBJ_VBO_PANEL_ID] = rotate(matModelArr[OBJ_VBO_PANEL_ID] , -PI/4 , vec3(0.F ,1.F ,0.F));
	

	matModelArr[OBJ_VBO_CUBE_ID] = rotate(matModelArr[OBJ_VBO_CUBE_ID], globalTheta, vec3(0.F, 1.F, 0.F));
	matModelArr[OBJ_VBO_CUBE_ID] = translate(matModelArr[OBJ_VBO_CUBE_ID], vec3(+0.1F , 0.F , 0.F ));
	matModelArr[OBJ_VBO_CUBE_ID] = rotate(matModelArr[OBJ_VBO_CUBE_ID], 2*globalTheta, vec3(0.F, 1.F, 0.F));
	matModelArr[OBJ_VBO_CUBE_ID] = scale(matModelArr[OBJ_VBO_CUBE_ID], vec3(0.11F, 0.11F, 0.11F));

	matModelArr[OBJ_VBO_PYRAMID_ID] = rotate(matModelArr[OBJ_VBO_PYRAMID_ID], globalTheta, vec3(0.F, 1.F, 0.F));
	matModelArr[OBJ_VBO_PYRAMID_ID] = translate(matModelArr[OBJ_VBO_PYRAMID_ID] , vec3(-0.1F , 0.F , 0.F));
	matModelArr[OBJ_VBO_PYRAMID_ID] = rotate(matModelArr[OBJ_VBO_PYRAMID_ID], 2*globalTheta, vec3(0.F, 1.F, 0.F));
	matModelArr[OBJ_VBO_PYRAMID_ID] = scale(matModelArr[OBJ_VBO_PYRAMID_ID], vec3(0.16F, 0.16F, 0.16F));
	
	
	matModelArr[OBJ_VBO_RCUBE_ID] = translate(matModelArr[OBJ_VBO_RCUBE_ID], vec3(-0.4F, 0.F, 0.5F));
	matModelArr[OBJ_VBO_RCUBE_ID] = rotate(matModelArr[OBJ_VBO_RCUBE_ID], -globalTheta + correctionOrb, vec3(0.F, 1.F, 0.F));
	matModelArr[OBJ_VBO_RCUBE_ID] = translate(matModelArr[OBJ_VBO_RCUBE_ID], vec3(+0.1F, 0.F, 0.F));
	matModelArr[OBJ_VBO_RCUBE_ID] = rotate(matModelArr[OBJ_VBO_RCUBE_ID], 2 * -(globalTheta + correctionRot), vec3(0.F, 1.F, 0.F));
	matModelArr[OBJ_VBO_RCUBE_ID] = scale(matModelArr[OBJ_VBO_RCUBE_ID], vec3(0.11F, 0.11F, 0.11F));
	
	matModelArr[OBJ_VBO_RPYRAMID_ID] = translate(matModelArr[OBJ_VBO_RPYRAMID_ID], vec3(-0.4F, 0.F, 0.5F));
	matModelArr[OBJ_VBO_RPYRAMID_ID] = rotate(matModelArr[OBJ_VBO_RPYRAMID_ID], -globalTheta + correctionOrb, vec3(0.F, 1.F, 0.F));
	matModelArr[OBJ_VBO_RPYRAMID_ID] = translate(matModelArr[OBJ_VBO_RPYRAMID_ID], vec3(-0.1F, 0.F, 0.F));
	matModelArr[OBJ_VBO_RPYRAMID_ID] = rotate(matModelArr[OBJ_VBO_RPYRAMID_ID], 2 * -(globalTheta + correctionRot), vec3(0.F, 1.F, 0.F));
	matModelArr[OBJ_VBO_RPYRAMID_ID] = scale(matModelArr[OBJ_VBO_RPYRAMID_ID], vec3(0.16F, 0.16F, 0.16F));
	
	matView = lookAtRH(vec3(0.F , 0.F , -2.F) , vec3(0.F , 0.F , 0.F) , vec3(0.F , 1.F , 0.F));


	GLfloat fovy = ((GLfloat)PI / 180.F) * 30.F;
	GLfloat aspect = (GLfloat)WIN_W / (GLfloat)WIN_H;

	matProj = perspective(fovy ,aspect ,1.F , 3.F);

	
	
}

//draw
GLuint locPos = 0;
GLuint locColor =0;
GLuint locMVP = 0;
GLuint locColorID = 0;

void drawPanel()
{
	glBindBuffer(GL_ARRAY_BUFFER, objVBOArr[OBJ_VBO_PANEL_ID]);
	glVertexAttribPointer(locPos, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
	glVertexAttribPointer(locColor, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(sizeof(vec4)));
	glUniformMatrix4fv(locMVP, 1, GL_FALSE, value_ptr(matMVP));

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawHolePanel()
{
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_STENCIL_TEST);

	glColorMask(0,0,0,0);

	glFrontFace(GL_CCW);
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 0x01, 0xFF);
	glStencilOpSeparate(GL_FRONT , GL_KEEP, GL_KEEP, GL_INCR);
	glStencilOpSeparate(GL_BACK ,GL_KEEP, GL_KEEP, GL_DECR);
	glEnable(GL_DEPTH_TEST);
	

	mat4 panelLayer = scale(matI, vec3(0.75F, 0.5F, 1.F)) * translate(matI, vec3(0.F, 0.F, +0.02F));
	matMVP = matProj * matView * matModelArr[OBJ_VBO_PANEL_ID] * panelLayer;
	
	glUniform1i(locColorID , COLOR_ID_HOLE_PANEL_OUT);
	drawPanel();


	panelLayer = scale(matI, vec3(0.3F, 0.2F, 1.F)) * translate(matI, vec3(0.F, 0.F, +0.01F));
	matMVP = matProj * matView * matModelArr[OBJ_VBO_PANEL_ID] * panelLayer;
	
	glUniform1i(locColorID, COLOR_ID_HOLE_PANEL_IN);
	drawPanel();

	glColorMask(1, 1, 1, 1);
	
	glStencilFunc(GL_EQUAL, 0x01, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glUniform1i(locColorID, COLOR_ID_HOLE_PANEL_OUT);

	panelLayer = scale(matI, vec3(0.42, 0.3F, 1.F)) * translate(matI, vec3(0.F, 0.F, +0.F));
	matMVP = matProj * matView * matModelArr[OBJ_VBO_PANEL_ID] * panelLayer;
	drawPanel();

	glDisable(GL_STENCIL_TEST);
	
}


void drawCube()
{
	glBindBuffer(GL_ARRAY_BUFFER, objVBOArr[OBJ_VBO_CUBE_ID]);
	glVertexAttribPointer(locPos, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
	glVertexAttribPointer(locColor, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(sizeof(vec4)));

	glUniformMatrix4fv(locMVP, 1, GL_FALSE, value_ptr(matMVP));

	glDrawArrays(GL_TRIANGLES, 0, objCubeSize);
}

void drawPyramids()
{
	glBindBuffer(GL_ARRAY_BUFFER, objVBOArr[OBJ_VBO_PYRAMID_ID]);
	glVertexAttribPointer(locPos, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
	glVertexAttribPointer(locColor, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)sizeof(vec4));

	
	glUniformMatrix4fv(locMVP, 1, GL_FALSE, value_ptr(matMVP));

	glDrawArrays(GL_TRIANGLES, 0, objPyramidSize);
}

int swhl = 0;
int selectHL = 0;
void (*highLightDrawObject)();

void drawFunc()
{
	glEnable(GL_DEPTH_TEST);
	glDepthRange(-1.F , 1.F);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	glClearDepth(1.F);
	glClearStencil(0x00);
	glClearColor(0.8F , 0.8F , 0.8F ,1.F);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glViewport(0 , 0 , WIN_W , WIN_H);

	//default reg value
	locPos = glGetAttribLocation(prog, "aPos");
	locColor = glGetAttribLocation(prog, "aColor");
	glEnableVertexAttribArray(locPos);
	glEnableVertexAttribArray(locColor);
	locMVP = glGetUniformLocation(prog, "uMVP");
	locColorID = glGetUniformLocation(prog , "colorID");
	

	

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS , 0x00 , 0xFF);
	glStencilOp(GL_KEEP , GL_KEEP , GL_KEEP);
	//default value set
	glUniform1i(locColorID, COLOR_ID_HL_OFF);

	matMVP = matProj * matView * matModelArr[OBJ_VBO_RCUBE_ID];
	drawCube();
	matMVP = matProj * matView * matModelArr[OBJ_VBO_RPYRAMID_ID];
	drawPyramids();



	matMVP = matProj * matView  * matModelArr[OBJ_VBO_CUBE_ID];
	drawCube();
	matMVP = matProj * matView * matModelArr[OBJ_VBO_PYRAMID_ID];
	drawPyramids();


	drawHolePanel();
	glDisable(GL_STENCIL_TEST);
	
	

	
	if (swhl)
	{
		glColorMask(0, 0, 0, 0);
		glDisable(GL_DEPTH_TEST);
		glClearStencil(0x00);
		glClear(GL_STENCIL_BUFFER_BIT);
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0x01, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
		glEnable(GL_STENCIL_TEST);
		matMVP = matProj * matView * matModelArr[selectHL];
		glViewport(0, +4, WIN_W, WIN_H);
		highLightDrawObject();
		glViewport(0, -4, WIN_W, WIN_H);
		highLightDrawObject();
		glViewport(+4, 0, WIN_W, WIN_H);
		highLightDrawObject();
		glViewport(-4, 0, WIN_W, WIN_H);
		highLightDrawObject();

		glViewport(0, 0, WIN_W, WIN_H);
		glStencilMask(0xFF);
		glStencilFunc(GL_GREATER, 0x01, 0xFF);
		glStencilOp(GL_ZERO, GL_KEEP, GL_KEEP);
		highLightDrawObject();
		glColorMask(1, 1, 1, 1);
		glEnable(GL_DEPTH_TEST);

		glUniform1i(locColorID, COLOR_ID_HL_ON);
		glStencilMask(0xFF);
		glStencilFunc(GL_EQUAL, 0x01, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		matMVP = matProj * matView * translate(matI, vec3(0.F, 0.F, -1.F));
		drawPanel();
		glUniform1i(locColorID, COLOR_ID_HL_OFF);
		glDisable(GL_STENCIL_TEST);
	}

	
	
}



GLint depthBits = 0;
GLint stencilBits = 0;


void keyFunc(GLFWwindow* window, int key, int scancode, int action, int mods)
{

	switch (key)
	{

	case GLFW_KEY_PRINT_SCREEN:
	case 'S':
		screnshotStencil();
		break;
	case 'H':
		if (action == GLFW_PRESS)
			swhl = (swhl+1)%2;
	
		highLightDrawObject = drawPyramids;
		selectHL = OBJ_VBO_PYRAMID_ID;
		break;
	case '1':
		highLightDrawObject = drawPyramids;
		selectHL = OBJ_VBO_PYRAMID_ID;
		break;
	case '2':
		highLightDrawObject = drawCube;
		selectHL = OBJ_VBO_CUBE_ID;
		break;
	case '3':
		highLightDrawObject = drawPyramids;
		selectHL = OBJ_VBO_RPYRAMID_ID;
		break;
	case '4':
		highLightDrawObject = drawCube;
		selectHL = OBJ_VBO_RCUBE_ID;
		break;
	case 'K':
		if (action == GLFW_PRESS)
		correctionRot += 0.1;
		printf("\n correction : %f \n", correctionRot);
		break;
	case 'T':
		globalTheta += 0.01;
		break;
	default:
		break;
	}

}

//glsl debug
void debugLog(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	printf("source : %#x type : %#x id : %d severity : %#x \n", source, type, id, severity);
	printf("message : %s \n", message);
	fflush(stdout);

}

int main(void)
{
	glfwInit();

	glfwWindowHint(GLFW_DOUBLEBUFFER , GLFW_TRUE);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);

	GLFWwindow* mainWindow = glfwCreateWindow(WIN_W,WIN_H,"stencil_test",NULL , NULL);
	glfwMakeContextCurrent(mainWindow);
	glewInit();
	glfwSetKeyCallback(mainWindow , keyFunc);

	glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH, GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE, &depthBits);
	glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,GL_STENCIL , GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &stencilBits);
	

	printf("allocate stencil bit : %d , depth bit : %d \n" , stencilBits , depthBits);


	GLint status = 0;
	GLchar buf[1024];


	char* vertSource = FileLoad("basic_stencil.vert");
	printf("%s\n",vertSource);

	vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert, 1, &vertSource, NULL);
	glCompileShader(vert);

	glGetShaderiv(vert, GL_COMPILE_STATUS, &status);
	printf("vert compile_status : %d \n", status);
	glGetShaderInfoLog(vert, 1024, NULL, buf);
	printf("vert shader log : %s \n", buf);


	char* fragSource = FileLoad("basic_stencil.frag");
	printf("%s\n", fragSource);
	frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &fragSource, NULL);
	glCompileShader(frag);
	glGetShaderiv(frag, GL_COMPILE_STATUS, &status);
	printf("frag compile_status : %d \n", status);
	glGetShaderInfoLog(frag, 1024, NULL, buf);
	printf("frag shader log : %s \n", buf);

	prog = glCreateProgram();
	glAttachShader(prog, vert);
	glAttachShader(prog, frag);
	glLinkProgram(prog);
	glUseProgram(prog);

	glGetProgramiv(prog, GL_LINK_STATUS, &status);
	printf("prog link_status : %d \n", status);
	glGetProgramInfoLog(prog, 1024, NULL, buf);
	glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
	printf("prog validate_status : %d \n", status);
	printf("prog log : %s \n", buf);
	glValidateProgram(prog);


	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
	glDebugMessageCallback(debugLog, NULL);

	initObject(objCube , faceCubeSize , vertPosCube , faceCube);
	initObject(objPyramid , facePyramidSize , vertPosPyramid , facePyramid);

	glGenBuffers(10, objVBOArr);
	prepareVBO(objVBOArr, OBJ_VBO_PANEL_ID, sizeof(vertex) * 6, Panel);
	prepareVBO(objVBOArr, OBJ_VBO_CUBE_ID , sizeof(vertex)*objCubeSize, objCube);
	prepareVBO(objVBOArr, OBJ_VBO_PYRAMID_ID , sizeof(vertex)*objPyramidSize , objPyramid);
	
	last_time = system_clock::now();

	/*
	//init matRef
	vec3 p0(0.4F, 0.F, 1.F), p1(0.7, 0.F, 0.7), p2(1.F, 0.F, 0.4);
	vec3 v = normalize(cross(p1 - p0, p2 - p0));
	float d = dot(p0, v);
	matRef = mat4{
		1 - 2 * v.x * v.x , -2 * v.x * v.y , -2 * v.z * v.x , 0.F,
		 -2 * v.x * v.y ,1 - 2 * v.y * v.y , -2 * v.y * v.z , 0.F,
		 -2 * v.z * v.x , -2 * v.y * v.z ,1 - 2 * v.z * v.z , 0.F,
		  2 * v.x * d   ,  2 * v.y * d   , -2 * v.z * d   , 1.F
	};
	*/

	while (!glfwWindowShouldClose(mainWindow))
	{
		cur_time = system_clock::now();
		elapsedTimeMSEC = duration_cast<milliseconds>(cur_time - last_time);
		globalTheta = (elapsedTimeMSEC.count() / 1000.0F) * (float)3.141592F * 0.5F;
		updateMVP();
		drawFunc();

		glfwSwapBuffers(mainWindow);
		glfwPollEvents();
	}

	glfwTerminate();
	free(vertSource);
	free(fragSource);

	return 0;
}