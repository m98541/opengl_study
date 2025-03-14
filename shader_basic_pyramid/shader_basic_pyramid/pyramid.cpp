#define _CRT_SECURE_NO_WARNINGS
#define GLM_ENABLE_EXPERIMENTAL

#define WIN_W 1000
#define WIN_H 1000
#define PI 3.141592F

#include <GL/glew.h>
#include <GLFW/glfw3.h>


#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")

#include <GLM/glm.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>

#include <stdio.h>
#include <iostream>
#include <chrono>

using namespace std;
using namespace glm;
using namespace chrono;

typedef struct vertex_t
{
	vec4 pos;
	vec4 color;
}vertex;

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
GLuint vbo[1];
GLuint ibo[1];
void prepareVBO()
{
	for (int i = 0; i < 18; vArr[i].pos= vertexArr[face[i]], vArr[i].color = faceColor[i], i++);



	glGenBuffers(1,vbo);
	glBindBuffer(GL_ARRAY_BUFFER , vbo[0]);
	glBufferData(GL_ARRAY_BUFFER , 2*5*8*sizeof(GLfloat) , NULL , GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER , 0, 5*8*sizeof(GLfloat), iArr);
	

	glGenBuffers(1, ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 18 * sizeof(GLuint), face, GL_STATIC_DRAW);
	

}

quat q0 = angleAxis(PI/4.F,vec3(1,0,0));
quat q1 = angleAxis(PI/4.F,vec3(0,0,1));

system_clock::time_point last_time = system_clock::now();

void update()
{
	system_clock::time_point cur_time = system_clock::now();
	milliseconds elapsedTimeMSEC = duration_cast<milliseconds>(cur_time - last_time);
	float t = (elapsedTimeMSEC.count() % 2000) / 2000.F;
	quat q = slerp(q0 , q1 , t);
	mat4f = toMat4(q);

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

	theta = atan2f(sinTheta , cosTheta);

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

	return myTrackball(org , dst);;
}




void keyCall(GLFWwindow* window , int key , int scancode ,int action, int mode)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{

	switch (key)
	{
	case 'A':
		if(yAngle < 2.F)
		yAngle += 0.01F;
		break;
	case 'D':
		if (yAngle > -2.F)
		yAngle -= 0.01F;
		break;
	case 'W':
		if (xAngle < 2.F)
		xAngle += 0.01F;
		break;
	case 'S':
		if (xAngle > -2.F)
		xAngle -= 0.01F;
		break;
	case 'Q':
		if (zAngle < 2.F)
		zAngle += 0.01F;
		break;
	case 'E':
		if (zAngle > -2.F)
		zAngle -= 0.01F;
		
		break;
	case 'R':
		cullMode = (cullMode + 1) % 5;

		break;
	default:
		break;
	}

	}
}

void cursorEnterFunc(GLFWwindow* win,int entered)
{
	printf("cursor %s the window\n",(entered == GL_FALSE) ? "leaving" : "entered");
	fflush(stdout);

}


void cursorPosFunc(GLFWwindow* win,double xscr, double yscr)
{
	if (mousePressed == GL_TRUE)
	{
		vec2 dragCur = vec2((GLfloat)xscr , (GLfloat)yscr);
		matDrag = calcTracball(dragStart , dragCur);
		mat4f = matDrag * matUpdate;

	}
	
}

void buttonFunc(GLFWwindow* win , int button , int action, int mods)
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
		glfwGetCursorPos(win,&x,&y);
		dragStart = vec2((GLfloat)x,(GLfloat)y);

		break;
	case GLFW_RELEASE:
		printf("released :"); 
		mousePressed = GL_FALSE;
		glfwGetCursorPos(win, &x, &y);
		vec2 dragCur = vec2((GLfloat)x, (GLfloat)y);
		matDrag = calcTracball(dragStart , dragCur);
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





void drawFunc()
{	
	glEnable(GL_DEPTH_TEST);
	cullFace(cullMode);
	glDepthRange(-1.F, 1.F);
	glClearDepth(1.F);
	glClearColor(0.F, 0.F, 0.F, 0.F);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLuint loc = glGetAttribLocation(prog, "vertPos");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)0); // vbo¿« offset ¿ª ¡‹

	loc = glGetAttribLocation(prog , "aColor");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(4*sizeof(GLfloat)));
	

	loc = glGetUniformLocation(prog, "uMat");
	glUniformMatrix4fv(loc, 1 ,GL_FALSE , value_ptr(mat4f));

	glDrawElements(GL_TRIANGLES,18,GL_UNSIGNED_INT,(GLvoid*)(0));
	//glDrawArrays(GL_TRIANGLES , 0 , 18);
	glFinish();
	
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
	GLFWwindow* window = glfwCreateWindow(WIN_W , WIN_H , "pyramid" , NULL , NULL);
	glfwMakeContextCurrent(window);
	glewInit();

	glfwSetKeyCallback(window, keyCall);
	
	glfwSetCursorEnterCallback(window, cursorEnterFunc);
	glfwSetCursorPosCallback(window, cursorPosFunc);
	glfwSetMouseButtonCallback(window , buttonFunc);

	const char* vertSource = FileLoad("pyramid.vert");
	vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert , 1, &vertSource , NULL);
	glCompileShader(vert);

	const char* fragSource = FileLoad("pyramid.frag");
	frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag , 1, &fragSource, NULL);
	glCompileShader(frag);

	prog = glCreateProgram();
	glAttachShader(prog , vert);
	glAttachShader(prog , frag);
	glLinkProgram(prog);
	glUseProgram(prog);

	prepareVBO();

	while (!glfwWindowShouldClose(window))
	{
		//update();
		drawFunc();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}
