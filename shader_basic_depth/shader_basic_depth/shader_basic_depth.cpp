#define _CRT_SECURE_NO_WARNINGS
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#pragma comment(lib , "glew32.lib" )
#pragma comment(lib , "opengl32.lib")
#pragma comment(lib , "glfw3.lib")

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>

int vert = 0;
int frag = 0;
int prog = 0;

using namespace glm;



vec4 vertRec[] = {
	{-0.1F , -0.5F, 0.0F , 1.F},
	{ 0.1F , -0.5F, 0.0F , 1.F},
	{ 0.1F ,  0.5F, 0.0F , 1.F},

	{ 0.1F ,  0.5F, 0.0F , 1.F},
	{-0.1F ,  0.5F, 0.0F , 1.F},
	{-0.1F , -0.5F, 0.0F , 1.F},
};


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

void drawFunc()
{
	glEnable(GL_DEPTH_TEST);
	glDepthRangef(0.0F, 1.0F);
	glClearDepth(1.F);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClear(GL_COLOR_BUFFER_BIT);
	GLuint loc = glGetAttribLocation(prog , "vertPos");
	GLuint uColorloc = glGetUniformLocation(prog, "uColor");
	GLuint uMoveloc = glGetUniformLocation(prog, "uMove");
	GLuint uAngleloc = glGetUniformLocation(prog, "uAngle");
	glEnableVertexAttribArray(loc);

	glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, vertRec);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);
	glUniform4f(uColorloc, 1.F, 0.F, 0.F, 1.F);
	glUniform1f(uAngleloc, 3.141592F * 0);
	glUniform4f(uMoveloc, 0.3F, 0.F, 0.4F, 0.F);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glUniform4f(uColorloc, 0.F, 1.F, 0.F, 0.7F);
	glUniform1f(uAngleloc, 3.141592F * 0.5);
	glUniform4f(uMoveloc, 0.F, 0.3F, 0.3F, 0.F);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glUniform4f(uColorloc, 0.F, 0.F, 1.F, 0.5F);
	glUniform1f(uAngleloc, 3.141592F * 1.5);
	glUniform4f(uMoveloc, 0.F, -0.3F, 0.2F, 0.F);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glUniform4f(uColorloc, 1.F, 1.F, 0.F, 0.3F);
	glUniform1f(uAngleloc, 3.141592F * 1);
	glUniform4f(uMoveloc, -0.3F, 0.F, 0.1F, 0.F);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisable(GL_BLEND);

	glFinish();
}

int main(void)
{
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(1000, 1000 , "depth" , NULL , NULL);
	glfwMakeContextCurrent(window);
	glewInit();

	const char* vertSource = FileLoad("depth.vert");
	vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert,1 , &vertSource ,NULL );
	glCompileShader(vert);


	const char* fragSource = FileLoad("depth.frag");
	frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag , 1 ,&fragSource , NULL);
	glCompileShader(frag);

	prog = glCreateProgram();
	glAttachShader(prog, vert);
	glAttachShader(prog, frag);
	glLinkProgram(prog);
	glUseProgram(prog);



	
	glClearColor(1.F ,1.F , 1.F ,1.F);

	
	while (!glfwWindowShouldClose(window))
	{

		drawFunc();
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	glfwTerminate();


	return 0;
}
