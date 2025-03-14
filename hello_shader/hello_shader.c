#define _CRT_SECURE_NO_WARNINGS
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#pragma comment (lib ,"opengl32.lib")
#pragma comment (lib ,"glew32.lib")
#pragma comment (lib ,"glfw3.lib")
#include <stdio.h>
#include <stdlib.h>

GLfloat vertPos[] = {
	0.F , 0.F , 0.F , 1.0F,
	+0.5F , 0.F , 0.F , 1.0F,
	0.F , +0.5F , 0.F , 1.0F,
};



GLfloat vertColor[] = {
	1.F , 0.F , 0.F , 1.F,
	0.F , 1.F , 0.F , 1.F,
	0.F , 0.F , 1.F , 1.F,
};

GLuint vert = 0;
GLuint frag = 0;
GLuint prog = 0;

void debugLog(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	printf("source : %#x type : %#x id : %d severity : %#x \n",source , type , id , severity );
	printf("message : %s \n" , message);
	fflush(stdout);

}

char* readShaderFile(const char* file)
{
	
	FILE* fp = fopen(file, "r");


	if (fp == NULL)
	{
		printf("%s 쉐이더 파일을 불러오는데 실패하였습니다.", file);
		return NULL;
	}
	
	fseek(fp,0,SEEK_END);
	
	size_t len  = ftell(fp);
	
	char* shaderBuffer = (char*)malloc(sizeof(char) * (len+4));
	rewind(fp);
	size_t size = fread(shaderBuffer, sizeof(char), len, fp);

	fclose(fp);
	shaderBuffer[size] = '\0';
	return (const char*)shaderBuffer;

}

void drawFunc(void)
{
	

	glClear(GL_COLOR_BUFFER_BIT);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		printf("Opengl Error %#x \n" , error);
		fflush(stdout);
	}
	GLuint locPos = glGetAttribLocation(prog, "aPos");
	glEnableVertexAttribArray(locPos);
	glVertexAttribPointer(locPos, 4, GL_FLOAT, GL_FALSE, 0, vertPos);

	GLuint locColor = glGetAttribLocation(prog, "aColor");
	glEnableVertexAttribArray(locColor);
	glVertexAttribPointer(locColor, 4, GL_FLOAT, GL_FALSE ,0, vertColor);
	
	GLuint locMove = glGetUniformLocation(prog , "uMove");
	glUniform4f(locMove,-0.5F , -0.5F , 0.F ,0.F);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glUniform4f(locMove, 0.F, 0.F, 0.F, 0.F);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glFinish();

}

int main(void)
{

	glfwInit();
	GLFWwindow* window = glfwCreateWindow(1000, 1000, "hello_shader", NULL, NULL);

	glfwMakeContextCurrent(window);
	glewInit();

	glClearColor(0.5F, 0.5F, 0.5F, 1.0F);
	//Shader Init 
	const char* VertexSource = readShaderFile("uniform.vert");
	const char* FragSource = readShaderFile("uniform.frag");

	//vert
	GLint status = 0;
	GLchar* buf[1024];
	vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert , 1, &VertexSource , NULL);
	glCompileShader(vert);
	glGetShaderiv(vert , GL_COMPILE_STATUS, &status);
	printf("vert compile_status : %d \n" , status);
	glGetShaderInfoLog(vert , 1024 , NULL , buf);
	printf("vert shader log : %s \n", buf);

	//frag
	frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag , 1,&FragSource , NULL);
	glCompileShader(frag);
	glGetShaderiv(frag, GL_COMPILE_STATUS, &status);
	printf("frag compile_status : %d \n", status);
	glGetShaderInfoLog(frag, 1024, NULL, buf);
	printf("frag shader log : %s \n", buf);

	//prog
	prog = glCreateProgram();
	glAttachShader(prog, vert);
	glAttachShader(prog, frag);
	glLinkProgram(prog);
	glUseProgram(prog);
	glGetProgramiv(prog , GL_LINK_STATUS , &status);
	printf("prog link_status : %d \n", status);
	glGetProgramInfoLog(prog , 1024 , NULL , buf);
	glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
	printf("prog validate_status : %d \n", status);
	printf("prog log : %s \n", buf);
	glValidateProgram(prog);

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageControl(GL_DONT_CARE , GL_DONT_CARE , GL_DONT_CARE , 0 , NULL , GL_TRUE );
	glDebugMessageCallback(debugLog , NULL);

	while (!glfwWindowShouldClose(window))
	{
		drawFunc();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	free(VertexSource);
	free(FragSource);
	glfwTerminate(window);

	return 0;

}