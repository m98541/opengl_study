#include <GL/glew.h>
#include <GLFW/glfw3.h>
//헤더 순서있음 glew.h include 후 glfw3.h include
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")

#include <stdio.h>

int main(void)
{
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(300, 300, "Hello GLEW", NULL, NULL);
	glfwMakeContextCurrent(window);

	//start glew
	glewInit();

	//mainloop
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	//done
	glfwTerminate();

	return 0;
}