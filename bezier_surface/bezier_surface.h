#pragma once
#ifndef GLOBAL_H
#define GLOBAL_H
#include "global.h"
#endif // !GLOBAL_H

//struct type : 타입이랑 함수 선언이랑 코드분리 실패ㅠㅠ
#ifndef BEZIER_SURFACE_DATA_TYPE
#define	BEZIER_SURFACE_DATA_TYPE
	typedef struct _patch4f {
		mat4x3 patch[4];
	}patch4x3x4f;
#endif //!BEZIER_SURFACE_DATA_TYPE

//func
#ifndef BEZIER_SURFACE_FILE_FUNC
#define BEZIER_SURFACE_FILE_FUNC
	char* LoadFile(const char* fileName);
	
#endif // !BEZIER_SURFACE_FILE_FUNC

#ifndef GLSL_FUNC
#define GLSL_FUNC
	// 클래스로 glsl 기능함수 랑 필수? 변수랑 캡슐화 하면... 다중 prog glsl 기능 동시 구현할때 유연하게 사용할 수 있을거 같은데...
	GLuint vert, frag, prog;
	GLuint fs_prog, vs_prog;

	void InitGLSL(const char* vertFileName,const char* fragFileName);
	GLuint* vbo;
	void PrepareVBO(GLuint id ,const void* data , size_t size);
	void BindVBO(GLuint id);

	void keyFunc(GLFWwindow * window , int key , int scancode ,int action, int mods);


	// 내부적으로는 3x4로 전환돼 연산하지만 코드가독성을 위해 4x3 vec3 4개 로 적어듐
	void DivCurve(const mat4x3 p, mat4x3&l , mat4x3&r);
	GLuint GenPatch(vertex* vbuf, GLuint depth, patch4x3x4f basisPatch);
	void MkQuad(vertex* vbuf, mat4x3 controlPoint, GLuint& idxOffset);
	GLuint surfaceVertIdxCnt = 0;
	void MkPatch(mat4x3 quad, patch4x3x4f& patch);
	//light shading
	void ShadingWhiteLight(vec4 lightPos, GLfloat ambient, GLfloat diffuse, GLfloat specular);
#endif // !GLSL_FUNC

//이거 basic_sphere.h 로 옮기는거 어케함?? => 애초에 main.cpp를 따로 설계하지 않아 sphereVertBuf를 surface에서써야됨
// 다음 설계부터는 mainProg<->lib_interface.h<->lib.cpp<-lib.h 구조로 분리필요함 그리고 현재 프로그램에서 쓰는 global은 mainProg.h 에서 관리 필요
#ifndef BASIC_SPHERE_DATA
#define BASIC_SPHERE_DATA
	vec4 sphereLightPos = vec4(0.F, 1.F, 0.F, 1.F);
	vertex sphereVertBuf[999999];
	GLuint sphereBufSize = 0;
#endif // !BASIC_SPHERE_DATA


#ifndef BEZIER_SURFACE_DATA
#define BEZIER_SURFACE_DATA
	#define SQ_SURFACE_VERTEX_ID 0

	//아 객체지향 필요해~~~~~~
	mat4 camModel = mat4(1.F);
	vec4 basisCamPos = vec4(0.4F , 0.4F , 0.4F, 1.F);
	vertex surfaceVertBuf[999999];
	GLuint surfaceBufSize = 0;

	mat4 matL = { 
		{1.F	, 0.F	, 0.F	, 0.F	},
		{0.5F	, 0.5F	, 0.F	, 0.F	},
		{0.25F	, 0.5F	, 0.25F	, 0.F	},
		{0.125F , 0.375F, 0.375F, 0.125F},
	};

	mat4 matR = {
		{0.125F , 0.375F, 0.375F, 0.125F},
		{0.F	, 0.25F , 0.5F	, 0.25F	},
		{0.F	, 0.F	, 0.5F	, 0.5F	},
		{0.F	, 0.F	, 0.F	, 1.F	}
	};
	
	mat4x3 basisQaud = {
		{  0.5F,0.F, -0.5F},
		{  0.5F,0.F,  0.5F},
		{ -0.5F,0.F, -0.5F},
		{ -0.5F,0.F,  0.5F},
	};

	mat4 matI4f = mat4(1.F);

#endif // !BEZIER_SURFACE_DATA


