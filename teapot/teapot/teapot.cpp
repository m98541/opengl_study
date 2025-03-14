
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <windows.h>
#include <string.h>

using namespace std;
using namespace glm;
using namespace std::chrono;

int numVertTeapotIdx = 0;
vertex teapotBuf[102400];
typedef struct _quad 
{
	vec3 p0;
	vec3 p1;
	vec3 p2;
	vec3 p3;
}quad;

typedef struct _tri
{
	vec3 p0;
	vec3 p1;
	vec3 p2;
}tri;

typedef struct _patch4f {
	quad patch[4];
}patch4f;


void mkQuad(vertex* vbuf, quad q , vec4 color)
{
	vbuf[numVertTeapotIdx].pos	= vec4(q.p0, 1.F);
	vbuf[numVertTeapotIdx +1].pos = vec4(q.p3, 1.F);
	vbuf[numVertTeapotIdx +2].pos = vec4(q.p2, 1.F);
	vbuf[numVertTeapotIdx +3].pos = vec4(q.p3, 1.F);
	vbuf[numVertTeapotIdx +4].pos = vec4(q.p0, 1.F);
	vbuf[numVertTeapotIdx +5].pos = vec4(q.p1, 1.F);
	
	vbuf[numVertTeapotIdx].color = color;
	vbuf[numVertTeapotIdx + 1].color = color;
	vbuf[numVertTeapotIdx + 2].color = color;
	vbuf[numVertTeapotIdx + 3].color = color;
	vbuf[numVertTeapotIdx + 4].color = color;
	vbuf[numVertTeapotIdx + 5].color = color;

	vbuf[numVertTeapotIdx].norm = color;
	vbuf[numVertTeapotIdx + 1].norm = color;
	vbuf[numVertTeapotIdx + 2].norm = color;
	vbuf[numVertTeapotIdx + 3].norm = color;
	vbuf[numVertTeapotIdx + 4].norm = color;
	vbuf[numVertTeapotIdx + 5].norm = color;

	numVertTeapotIdx += 6;
}

void divCurve(const quad p, quad& l, quad& r)
{	
	l.p0 = p.p0;
	r.p3 = p.p3;
	l.p1 = (p.p0 + p.p1) * 0.5F;
	r.p2 = (p.p2 + p.p3) * 0.5F;
	l.p2 = (l.p1 + (p.p1 + p.p2) * 0.5F) * 0.5F;
	r.p1 = (r.p2 + (p.p1 + p.p2) * 0.5F) * 0.5F;
	l.p3 = (l.p2 + r.p1) * 0.5F;
	r.p0 = l.p3;


}


void generatePatch(GLuint level, patch4f p)
{
	if (level < 1)
	{
		vec4 color = vec4(1.F, 1.F, 1.F, 1.F);
		mkQuad(teapotBuf , p.patch[0] , color);
		mkQuad(teapotBuf,  p.patch[1], color);
		mkQuad(teapotBuf,  p.patch[2], color);
		mkQuad(teapotBuf,  p.patch[3], color);
		
	}
	else
	{
		patch4f ul, ur, wu_l, wu_r , wv_l , wv_r;
		// u 방향 
		for (int i = 0; i < 4; i++)
			divCurve(p.patch[i] , ul.patch[i], ur.patch[i]);
		
		//u에 v방향 적용한 u seq
		for(int i = 0; i < 4; i++)
			divCurve(ul.patch[i], wu_l.patch[i] , wu_r.patch[i]);

		//u에 v방향 적용한 v seq
		for (int i = 0; i < 4; i++)
			divCurve(ur.patch[i], wv_l.patch[i], wv_r.patch[i]);
		printf("start recursive depth level : %d \n", level);
		generatePatch(level - 1 , wu_l);
		generatePatch(level - 1,  wu_r);
		generatePatch(level - 1,  wv_l);
		generatePatch(level - 1,  wv_r);
		printf("finish recursive depth level : %d \n" , level);	

	}
}


int generateTeapot(int level)
{
	for (int i = 0; i < kTeapotNumPatches; ++i)
	{
		
		vec3 v[16]; // control point

		printf("\nstart patch\n");
		GLfloat scale = 0.5f;
		for (int j = 0; j < 16; ++j)
		{
			int idx = teapotPatchesData[i][j] - 1;
			printf("input teapot patches Data : %d\n" , idx);
			v[j] = vec3(scale , scale, scale) * vec3(teapotVerticesData[idx][0], teapotVerticesData[idx][1], teapotVerticesData[idx][2]);
		}
		quad q[4] = {
			{v[0] , v[1] , v[2] , v[3]},
			{v[4] , v[5] , v[6] , v[7]},
			{v[8] , v[9] , v[10] , v[11]},
			{v[12] , v[13] , v[14] , v[15]},
		};
		patch4f cp = {
			q[0] , q[1] , q[2], q[3]
		};

		generatePatch(level , cp);
		printf("finish patch\n\n");
	}

	memcpy(globalTeapotBuf , teapotBuf , sizeof(vertex)* numVertTeapotIdx);
	printf("finish generate teapot:%d(size) \n", numVertTeapotIdx);
	return numVertTeapotIdx;
}