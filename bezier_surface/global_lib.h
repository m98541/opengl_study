#pragma once
#ifndef  OPENGLWINSYS_H
#define OPENGLWINSYS_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#pragma comment(lib , "opengl32.lib")
#pragma comment(lib , "glew32.lib")
#pragma comment(lib , "glfw3.lib")
#include <windows.h>

#endif // !OPENGLWINSYS_H 

#ifndef  GLM_H
#define GLM_H
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>

#endif // !GLM_H

#ifndef STD_H
#define STD_H
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <iostream>


#endif // !STD_H

#ifndef TIME_H
#define TIME_H
#include <chrono>

#endif // !TIME_H
