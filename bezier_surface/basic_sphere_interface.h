#pragma once
#ifndef GLOBAL_H
#define GLOBAL_H
#include "global.h"
#endif // !GLOBAL_H

void SphereTriTessellation(int level, mat3x4 tri, vertex* buf, GLuint& bufOffSet);
void GenerateSphere(int level, vertex* buf, GLuint& vertexCnt);

