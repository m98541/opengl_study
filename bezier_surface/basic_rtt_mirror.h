#pragma once
#ifndef GLOBAL_H
#define GLOBAL_H
#include "global.h"
#endif // !GLOBAL_H



#ifndef FBO_ID
#define FBO_ID
#define	PHYSICAL_FBO 0 //default 
#define	MIRROR_FBO 1// MIRROR_FBO=>texture memory
#define MIRROR_TETURE_COLOR GL_TEXTURE0
#define MIRROR_TETURE_DEPTH GL_TEXTURE0

#define MIRROR_TBO 0

#endif // !FBO_ID

#ifndef BO_TYPE
#define BO_TYPE
typedef struct tbo_ {
	GLuint colorBuffer;
	GLuint depthBuffer;
	GLuint stencilBuffer;
}tbo_t;

#endif // !BO_TYPE


#ifndef FBO_DATA	
#define FBO_DATA
GLuint fboArr[10];
GLuint fboArrSize = 10;
tbo_t tboArr[10];
GLuint tboArrSize = 10;

#endif // !FBO_DATA
//아래 부터는 basic_rtt_mirror에서 단독으로 사용되는 코드 
