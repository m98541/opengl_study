#pragma once
#ifndef GLOBAL_LIB_H
#define GLOBAL_LIB_H
#include "global_lib.h"
#endif // !GLOBAL_LIB_H
using namespace glm;
using namespace std;
using namespace std::chrono;


typedef struct _vertex {
	vec4 vertPos;
	vec4 vertColor;
	vec4 vertNorm;
	vec3 vertTexCoord;
}vertex;


typedef struct _light {
	vec4 pos;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 att;
}light;

typedef struct _material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shineness;

}material;
