#define _CRT_SECURE_NO_WARNINGS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>

#pragma comment(lib , "glew32.lib")
#pragma comment(lib , "opengl32.lib")
#pragma comment(lib , "glfw3.lib")

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <math.h>

using namespace std;
using namespace glm;
using namespace std::chrono;

GLfloat r = 0.F;
GLfloat g = 0.F;
GLfloat b = 0.F;
GLfloat a = 1.F;

GLint vert = 0;
GLint frag = 0;
GLint prog = 0;

GLfloat X_pos = 0.F;
GLfloat Y_pos = 0.F;
vec4 curPos(0.F , 0.F , 0.F, 1.F);

GLfloat Angle = 0.0F;
float elapsedTime;
vec4 VertexArr[] = {
	{0.F , 0.F , 0.F , 1.0F},
	{0.25F , 0.F , 0.F , 1.0F},
	{0.F , +0.25F , 0.F , 1.0F}
};

vec4 ColorArr[] = {
	{1.F , 0.5F , 0.5F , 1.F},
	{0.5F , 1.F , 0.5F , 1.F},
	{0.5F , 0.F , 1.F , 1.F}
};


void updateFunc()
{
	Angle = elapsedTime/1000.0F * (GLfloat)(3.141592F/4.F);
} 

char* FileLoad(const char* fileName) 
{
	FILE* fp = fopen(fileName , "r");

	fseek(fp,0, SEEK_END);
	size_t len = ftell(fp);
	char* buf = (char*)malloc(sizeof(char) * (len + 4));
	rewind(fp);
	size_t size = fread(buf,1, len,fp);

	buf[size] = '\0';
	fclose(fp);
	return buf;
}

void keyboardCallback(GLFWwindow* win, int key , int scancode , int action , int mods )
{

	switch (key)
	{
	case 'W':
		curPos.y += 0.1F;
		break;
	case 'S':
		curPos.y -= 0.1F;
		break;
	case 'A':
		curPos.x -= 0.1F;
		break;
	case 'D':
		curPos.x += 0.1F;
		break;
	case 'R':
		curPos.x = curPos.y = 0.F;
		glfwSetTime(0.0F);
		break;

	default:
		break;
	}


}



void drawFunc(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	GLuint loc = glGetAttribLocation(prog , "vertexPos");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, value_ptr(VertexArr[0]));

	loc = glGetAttribLocation(prog, "aColor");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, value_ptr(ColorArr[0]));

	loc = glGetUniformLocation(prog, "uAngle");
	GLuint locAlpha = glGetUniformLocation(prog, "uAlpha");
	float w = Angle;
	float alpha = 0;
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR,GL_ONE,GL_ZERO);
	glHint(GL_LINE_SMOOTH_HINT , GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT , GL_NICEST);
	for (int i = 0; i < 8; i++)
	{
	
		w -= float(i)*(3.141592/(180*6));
		alpha = 1.F / ((float)i + 1.F);
		glBlendColor(alpha, alpha , alpha, 1.F);
	
		glUniform1f(loc, w);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	
	}

	
	glDisable(GL_BLEND);

	glFinish();

}


int main(void)
{
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(1000, 1000 , "shader_basic",NULL, NULL);
	glfwMakeContextCurrent(window);
	glewInit();
	glfwSetKeyCallback(window, keyboardCallback);
	glClearColor(0.5F, 0.5F, 0.5F, 1.0F);





	vert = glCreateShader(GL_VERTEX_SHADER);
	const char* vertSource = FileLoad("shader_basic_vert.vert");
	printf(vertSource);
	glShaderSource(vert , 1 , &vertSource, NULL);
	glCompileShader(vert);

	frag = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragSource = FileLoad("shader_basic_frag.frag");
	printf(fragSource);
	glShaderSource(frag , 1, &fragSource , NULL);
	glCompileShader(frag);

	prog = glCreateProgram();
	glAttachShader(prog, vert);
	glAttachShader(prog, frag);
	glLinkProgram(prog);
	glUseProgram(prog);

	system_clock::time_point lastTime = system_clock::now();

	while (!glfwWindowShouldClose(window))
	{	
		
		drawFunc();
		updateFunc();
		glfwSwapBuffers(window);
		glfwPollEvents();
		system_clock::time_point curTime = system_clock::now();
		milliseconds duration = duration_cast<milliseconds>(curTime - lastTime);
		elapsedTime = (float)duration.count();
	}
	
	glfwTerminate();

	return 0;

}