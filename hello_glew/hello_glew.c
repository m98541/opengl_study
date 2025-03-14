#include <GL/glew.h>
#include <GLFW/glfw3.h>
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glfw3.lib")
#include <stdio.h>

int main(void)
{
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(300, 300, "Hello GLEW",NULL , NULL);
	glfwMakeContextCurrent(window);

	//start glew
	glewInit();

	//mainloop
	while (!glfwWindowShouldClose(window))
	{
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	//done
	glfwTerminate();

	return 0;
}