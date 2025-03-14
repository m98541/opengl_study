
#include "basic_rtt_mirror_interface.h"
#include "basic_rtt_mirror.h"

void InitFBOTBO()
{
	glGenTextures(tboArrSize*3 , (GLuint*)tboArr);
	glGenFramebuffers(fboArrSize, fboArr);
}


void InitMirrorTex()
{
	glActiveTexture(MIRROR_TETURE_COLOR);
	glBindTexture(GL_TEXTURE_2D , tboArr[MIRROR_TBO].colorBuffer);
	glTextureStorage2D(tboArr[MIRROR_TBO].colorBuffer, 1 , GL_RGBA8 , MAIN_H , MAIN_W );
	glGenerateMipmap(tboArr[MIRROR_TBO].colorBuffer);
	glTextureParameterf(tboArr[MIRROR_TBO].colorBuffer, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameterf(tboArr[MIRROR_TBO].colorBuffer, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, tboArr[MIRROR_TBO].depthBuffer);
	glTextureStorage2D(tboArr[MIRROR_TBO].depthBuffer, 1, GL_DEPTH_COMPONENT32F, MAIN_H, MAIN_W);
	glGenerateMipmap(tboArr[MIRROR_TBO].depthBuffer);
	glTextureParameterf(tboArr[MIRROR_TBO].depthBuffer, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameterf(tboArr[MIRROR_TBO].depthBuffer, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

}

void InitMirrorFBO()
{	
	glBindFramebuffer(GL_FRAMEBUFFER, fboArr[MIRROR_FBO]);
	glActiveTexture(MIRROR_TETURE_COLOR);
	glFramebufferTexture2D(GL_FRAMEBUFFER , GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D ,tboArr[MIRROR_TBO].colorBuffer , 0);
	glActiveTexture(MIRROR_TETURE_DEPTH);
	glFramebufferTexture2D(GL_FRAMEBUFFER , GL_DEPTH_ATTACHMENT , GL_TEXTURE_2D, tboArr[MIRROR_TBO].depthBuffer  ,0);
}
//mat4x3 SetMirror(vec3 pos, vec3 norm, GLfloat size_w,GLfloat size_h)

 