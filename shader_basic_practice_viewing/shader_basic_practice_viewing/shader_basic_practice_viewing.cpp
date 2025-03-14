#define _CRT_SECURE_NO_WARNINGS
#define GLM_ENABLE_EXPERIMENTAL

#define WIN_W 1360
#define WIN_H 960


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#pragma comment(lib , "opengl32.lib")
#pragma comment(lib , "glew32.lib")
#pragma comment(lib , "glfw3.lib")


#include <GLM/glm.hpp>

#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtx/string_cast.hpp>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <chrono>


using namespace glm;
using namespace std::chrono;
using namespace std;

typedef struct _vertex
{
	vec4 pos;
	vec4 color;
}vertex;

typedef struct _display
{
	vec4 pos;
	vec2 coord;
}display;





vec2 tilt = vec2(0.F , 0.F);
GLfloat camAngle = 0.F;
GLfloat camAngleMap = 0.F;


GLuint frag = 0;
GLuint prog = 0;
GLuint vert = 0;
GLfloat radius = 2.F;


mat4 matI = mat4(1.F);
mat4 matPyramid = mat4(1.F);
mat4 matCube = mat4(1.F);
mat4 matCam = mat4(1.F);
mat4 matViewCam = mat4(1.F);
mat4 matViewMap = mat4(1.F);
mat4 matProj = mat4(1.F);
mat4 matProjMap = mat4(1.F);
int winCurSize_w = 0;
int winCurSize_h = 0;

vertex vertPosCam[3] = {
	{{0.F , 0.0F , -0.5F ,1.F} , {1.F , 1.F , 0.F ,1.F}},
	{{0.5F , 0.F ,  0.5F ,1.F} , {1.F , 1.F , 0.F ,1.F}},
	{{-0.5F , 0.F , 0.5F ,1.F} , {1.F , 1.F , 0.F ,1.F}}
}; 

vertex vertPosPyramid[5] = {
	{{ 0.0F ,  0.5F,  0.0F, 1.F},{1.F , 1.F ,1.F ,1.F}},
	{{ 0.5F , -0.3F,  0.0F, 1.F},{1.F , 0.F ,0.F ,1.F}},
	{{ 0.0F , -0.3F, -0.5F, 1.F},{0.F , 1.F ,0.F ,1.F}},
	{{-0.5F , -0.3F,  0.0F, 1.F},{0.F , 0.F ,1.F ,1.F}},
	{{ 0.0F , -0.3F,  0.5F, 1.F},{1.F , 1.F ,0.F ,1.F}}

};

vertex vertPosCube[8] = {
	{{-0.5F , -0.5F , 0.5F , 1.F},{1.f , 0.f , 0.f , 1.f}},
	{{ 0.5F , -0.5F , 0.5F , 1.F},{0.f , 0.f , 1.f , 1.f}},
	{{ 0.5F , -0.5F ,-0.5F , 1.F},{0.f , 1.f , 0.f , 1.f}},
	{{-0.5F , -0.5F ,-0.5F , 1.F},{1.f , 1.f , 0.f , 1.f}},
	
	{{-0.5F ,  0.5F , 0.5F , 1.F},{1.f , 0.f , 1.f , 1.f}},
	{{ 0.5F ,  0.5F ,0.5F ,  1.F},{0.f , 0.5f , 0.2f , 1.f}},
	{{ 0.5F ,  0.5F ,-0.5F , 1.F},{0.f , 1.f , 1.f , 1.f}},
	{{-0.5F ,  0.5F ,-0.5F , 1.F},{1.f , 1.f , 1.f , 1.f}},

};
GLuint faceCam[3] = { 0 ,1 ,2};

GLuint facePyramid[18] = {
	0, 1, 2, // red
	0, 2, 3, // green 
	0, 3, 4, // blue 
	0, 4, 1, // yellow
	1, 4, 3, // cyan
	1, 3, 2  // cyan
};

GLuint faceCube[36] = {
	0, 3, 2, 
	0, 2, 1, 
	1, 2, 6,

	1, 6, 5,
	2, 3, 7,
	2, 7, 6,

	3, 0, 4,
	3, 4, 7,
	1, 5, 4,
	
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
};

GLuint vbo[10];
GLuint ibo[10];


const GLfloat map_size_x = 0.5;
const GLfloat map_size_y = 0.5;


display displayVertBuf[4] = {
	{{ -map_size_x, +map_size_y, 0.0f, 1.0f }, {0.001F , 0.999F}},
	{{ +map_size_x, +map_size_y, 0.0f, 1.0f }, {0.999F, 0.999F}},
	{{ +map_size_x, -map_size_y, 0.0f, 1.0f }, {0.999F,0.001F}},
	{{ -map_size_x, -map_size_y, 0.0f, 1.0f }, {0.001F,0.001F}},
};

GLuint displayFaceBuf[6] = {
	0,1,2,
	0,2,3
};

vec2 displayTexCoord[4] = {

	{0.001F , 0.001F},
	{0.999F , 0.001F},
	{0.999F , 0.999F},
	{0.001F , 0.999F},

};

GLubyte screenShotBuf[WIN_W * WIN_H * 4];

void screenShotFunc();

void prepareiBO(GLuint idx ,GLuint vert_size ,GLuint face_size,vertex* vbuf,GLuint* fbuf)
{
	

	glBindBuffer(GL_ARRAY_BUFFER, vbo[idx]);
	glBufferData(GL_ARRAY_BUFFER, 2 * vert_size, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vert_size, vbuf);


	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[idx]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_size , fbuf, GL_STATIC_DRAW);

	
}


void prepareDisplay(GLuint idx, GLuint vert_size, GLuint face_size, display* vbuf, GLuint* fbuf)
{


	glBindBuffer(GL_ARRAY_BUFFER, vbo[idx]);
	glBufferData(GL_ARRAY_BUFFER, 2 * vert_size, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vert_size, vbuf);



	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[idx]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_size, fbuf, GL_STATIC_DRAW);


}


int cnt = 0;
mat4 myLookAtRH(vec3 eye , vec3 at, vec3 up)
{
	vec3 p = eye;
	vec3 n = -normalize(at-eye);
	vec3 u = normalize(cross(up,n));
	vec3 v = cross(n,u);

	mat4 m = {
		vec4(u,-dot(u,p)),
		vec4(v,-dot(v,p)),
		vec4(n,-dot(n,p)),
		vec4(0.F , 0.F , 0.F ,1.F)
	};
	
	m =transpose(m);
	return m;

}

mat4 myOrthoRH(GLfloat left,GLfloat right,GLfloat top,GLfloat bottom,GLfloat onear,GLfloat ofar)
{
	mat4 mProj = {
		vec4(2.F /(right-left) , 0.F ,0.F ,0.F),
		vec4(0.F , 2.F / (top-bottom) , 0.F ,0.F),
		vec4(0.F ,0.F , 2.F/(onear- ofar) ,0.F),
		vec4(-(right + left)/2.F , -(top + bottom)/2.F , -(ofar + onear)/2.F , 1.F)
	};
	return mProj;
}

mat4 myFrustum(GLfloat left, GLfloat right, GLfloat top, GLfloat bottom, GLfloat fnear, GLfloat ffar)
{
	mat4 mProj = {
		vec4(fnear / right , 0.F , 0.F , 0.F),
		vec4(0.F , fnear / top , 0.F , 0.F),
		vec4(0.F , 0.F , -(ffar + fnear)/(ffar - fnear) , -1.F),
		vec4(0.F , 0.F , -(2* ffar *fnear) / (ffar - fnear) , 0.F)

	};

	return mProj;

}

mat4 myFrustumShear(GLfloat left, GLfloat right, GLfloat bottom , GLfloat top, GLfloat fnear, GLfloat ffar)
{
	mat4 mProj = {
		vec4((2* fnear)/(right - left)  , 0.F , 0.F , 0.F),
		vec4(0.F , (2* fnear) / (top - bottom) , 0.F , 0.F),
		vec4((right + left) / (right - left) , (top+bottom)/(top-bottom) , -(ffar + fnear) / (ffar - fnear) , -1.F),
		vec4(0.F , 0.F , -(2* ffar * fnear) / (ffar - fnear) , 0.F)

	};

	return mProj;

}

mat4 myPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar, vec2 tilt)
{
	GLfloat top = zNear * tanf(fovy / 2.F);
	GLfloat right = aspect*top;
	GLfloat bottom = -top;
	GLfloat left = -right;
	return myFrustumShear(left + tilt.x , right + tilt.x, bottom + tilt.y, top + tilt.y, zNear , zFar);
}

system_clock::time_point last_time = system_clock::now();









void update(void)
{
	system_clock::time_point cur_time = system_clock::now();
	milliseconds elapsedTimeMSEC = duration_cast<milliseconds>(cur_time - last_time);
	GLfloat theta = (elapsedTimeMSEC.count() / 1000.0F) * (float)3.141592F * 0.5F;


	displayVertBuf[0].pos.x = -map_size_x - 0.2F + 0.2F * cosf(theta);
	displayVertBuf[0].pos.y = map_size_y + 0.2F * sinf(theta );

	displayVertBuf[1].pos.x = map_size_x - 0.1F + 0.1F * cosf(theta);
	displayVertBuf[1].pos.y = map_size_y + 0.1F * sinf(theta);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(displayVertBuf), displayVertBuf);


	matPyramid = mat4(1.F);
	matPyramid = translate(matPyramid, vec3(-0.4F, 0.F, 0.F));
	matPyramid = rotate(matPyramid , theta , vec3(0.F , 1.F , 0.F));
	matPyramid = scale(matPyramid, vec3(0.75F, 0.75F, 0.75F));
	
	

	matCube = mat4(1.F);
	matCube = translate(matCube, vec3(+0.4F, 0.F, 0.F));
	matCube = rotate(matCube, theta, vec3(1.F, 0.F, 0.F));
	matCube = scale(matCube, vec3(0.45F, 0.45F, 0.45F));

	
//	matView[0][0] = cosf(theta); matView[2][0] = sinf(theta);
//	matView[0][2] = sinf(theta); matView[2][2] = cosf(theta);
//	matView[3][2] = -radius;
	matViewCam = myLookAtRH(vec3(radius * sinf(theta), 0.F , radius * cosf(theta)) , vec3(0.0F, 0.0F, 0.0F) , vec3(0.0F, 1.0F, 0.0F));
	matCam = mat4(1.F);
	matCam = translate(matCam, vec3(radius * sinf(theta), 0 , radius * cosf(theta)));
	matCam = rotate(matCam , theta , vec3(0.F , 1.F , 0.F));
	matCam = scale(matCam, vec3(0.25F, 0.25F, 0.25F));
	matViewMap = myLookAtRH(vec3(0.F , 10.F , 0.F), vec3(0.0F, 0.0F, 0.0F), vec3(0.0F, 0.0F, 1.0F));


	//matProj = orthoRH(-1.F , 1.F,-0.75F, 0.75F,1.F , 3.F);
	const GLfloat zoom = 0.5F;

	float fovy = ((GLfloat)3.141592 / 180.0F) * 30.0F;
	const GLfloat aspect =(GLfloat)winCurSize_w / (GLfloat)winCurSize_h;

	matProj = myPerspective(fovy + camAngle, aspect , +0.5F , +30.F,tilt);
	matProjMap = myPerspective(fovy + camAngleMap, 1.F, +0.5F, +30.F, vec3(0.F , 0.F ,0.F));
	//matProj = myFrustumShear(-0.5F*zoom + x, 1.5F*zoom + x , -0.5F*zoom, +1.F * zoom, 1.F , 3.F);
		
	if (!cnt) {
		cout << to_string(matProj) << endl;
		cnt++;
	}
}

void drawDisplay()
{
	

	glEnable(GL_DEPTH_TEST);
	glDepthRange(-1.F, 1.F);
	glClearDepth(1.F);
	glViewport(0, 0, WIN_W , WIN_H);

	glClearColor(0.F, 0.F, 0.F, 1.F);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLuint locVert = glGetAttribLocation(prog , "vertPos");
	GLuint locColor = glGetAttribLocation(prog, "aColor");
	
	GLuint locMat = glGetUniformLocation(prog, "uMatModel");
	GLuint locMatView = glGetUniformLocation(prog, "uMatView");
	GLuint locMatProj = glGetUniformLocation(prog, "uMatProj");
	GLuint locTex = glGetAttribLocation(prog, "aTexCoord");
	GLuint locTexSampler = glGetUniformLocation(prog, "texSampler");
	
	glEnableVertexAttribArray(locVert);
	glEnableVertexAttribArray(locColor);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[3]);

	glEnableVertexAttribArray(locTex);
	
	glUniform1i(locTexSampler, 0);


	glVertexAttribPointer(locVert, 4 , GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(locTex, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));


	glUniformMatrix4fv(locMat, 1, GL_FALSE, value_ptr(matI));
	glUniformMatrix4fv(locMatView, 1, GL_FALSE, value_ptr(matI));
	glUniformMatrix4fv(locMatProj, 1, GL_FALSE, value_ptr(matI));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (GLvoid*)(0));


}

void drawFunc()
{
	glEnable(GL_DEPTH_TEST);
	glDepthRange(-1.F, 1.F);
	glClearDepth(1.F);

	glClearColor(0.8F, 0.8F, 0.8F, 1.F);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, WIN_W, WIN_H);

	GLuint locVert = glGetAttribLocation(prog, "vertPos");
	GLuint locColor = glGetAttribLocation(prog, "aColor");
	GLuint locMat = glGetUniformLocation(prog , "uMatModel");
	GLuint locMatView = glGetUniformLocation(prog , "uMatView");
	GLuint locMatProj = glGetUniformLocation(prog , "uMatProj");

	glUniformMatrix4fv(locMatView, 1, GL_FALSE, value_ptr(matViewCam));
	glUniformMatrix4fv(locMatProj, 1, GL_FALSE, value_ptr(matProj));
	glEnableVertexAttribArray(locVert);
	glEnableVertexAttribArray(locColor);
	
	
	glBindBuffer(GL_ARRAY_BUFFER , vbo[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[0]);
	glVertexAttribPointer(locVert, 4, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(locColor, 4, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
	glUniformMatrix4fv(locMat ,1 , GL_FALSE , value_ptr(matPyramid));
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, (GLvoid*)(0));
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[1]);
	glVertexAttribPointer(locVert, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0));
	glVertexAttribPointer(locColor, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
	glUniformMatrix4fv(locMat, 1, GL_FALSE, value_ptr(matCube));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLvoid*)(0));
	

	

}

void drawMiniMap()
{
	//draw mini map
	GLuint mapPos_x = (GLuint)(WIN_W * 0.8);
	GLuint mapPos_y = (GLuint)(WIN_W * 0.05);

	GLsizei mapSize = (GLsizei)(WIN_H * 0.25);
	glEnable(GL_SCISSOR_TEST);
	glScissor(mapPos_x, mapPos_y, mapSize, mapSize);
	glViewport(mapPos_x, mapPos_y, mapSize, mapSize);
	glClearColor(0.5F , 0.5F , 1.F, 1.F);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	GLuint locVert = glGetAttribLocation(prog, "vertPos");
	GLuint locColor = glGetAttribLocation(prog, "aColor");
	GLuint locMat = glGetUniformLocation(prog, "uMatModel");
	GLuint locMatView = glGetUniformLocation(prog, "uMatView");
	GLuint locMatProj = glGetUniformLocation(prog, "uMatProj");
	glUniformMatrix4fv(locMatView, 1, GL_FALSE, value_ptr(matViewMap));
	glUniformMatrix4fv(locMatProj, 1, GL_FALSE, value_ptr(matProjMap));
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[0]);
	glVertexAttribPointer(locVert, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(locColor, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
	glUniformMatrix4fv(locMat, 1, GL_FALSE, value_ptr(matPyramid));
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, (GLvoid*)(0));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[1]);
	glVertexAttribPointer(locVert, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0));
	glVertexAttribPointer(locColor, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
	glUniformMatrix4fv(locMat, 1, GL_FALSE, value_ptr(matCube));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLvoid*)(0));
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[2]);
	glVertexAttribPointer(locVert, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0));
	glVertexAttribPointer(locColor, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
	glUniformMatrix4fv(locMat, 1, GL_FALSE, value_ptr(matCam));
	glDrawElements(GL_TRIANGLES, 3 , GL_UNSIGNED_INT, (GLvoid*)(0));


	glDisable(GL_SCISSOR_TEST);

}


GLuint rbo_color = 0;
GLuint rbo_depth = 0;

GLuint fbo = 0;
GLuint texObj[10] = { 0, };
GLuint texObjDepth[10] = { 0, };


void iniTex()
{

	glGenTextures(1, texObj);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D , texObj[0]);
	glTextureStorage2D(texObj[0], 1, GL_RGBA8, WIN_W, WIN_H);
	
	glGenerateTextureMipmap(texObj[0]);
	glTextureParameterf(texObj[0], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameterf(texObj[0], GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenTextures(1, texObjDepth);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texObjDepth[0]);
	glTextureStorage2D(texObjDepth[0], 1, GL_DEPTH_COMPONENT32F, WIN_W, WIN_H);

	glGenerateTextureMipmap(texObjDepth[0]); 
	glTextureParameterf(texObjDepth[0], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameterf(texObjDepth[0], GL_TEXTURE_MAG_FILTER, GL_NEAREST);

}

const GLenum colorAttBuffer[10] = 
{
	GL_COLOR_ATTACHMENT0,
	GL_COLOR_ATTACHMENT1,
};
void initFBO()
{

	glGenRenderbuffers(1, &rbo_color);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_color);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_BGRA, WIN_W, WIN_H);

	glGenRenderbuffers(1, &rbo_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIN_W, WIN_H);


	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	
	
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_RENDERBUFFER, rbo_color);


	glActiveTexture(GL_TEXTURE0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texObj[0], 0);
	glActiveTexture(GL_TEXTURE1);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texObjDepth[0], 0);

	glDrawBuffers(2, colorAttBuffer);
	glReadBuffer(GL_COLOR_ATTACHMENT1);

}



char* FileLoad(const char* fileName)
{
	FILE* fp = fopen(fileName, "r");

	fseek(fp, 0, SEEK_END);
	size_t len = ftell(fp);
	char* buf = (char*)malloc(sizeof(char) * (len + 4));
	rewind(fp);
	size_t size = fread(buf, 1, len, fp);

	buf[size] = '\0';
	fclose(fp);
	return buf;
}

void keyFunc(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key)
	{
	case 'A':
		tilt.x -= 0.01F;
		break;
	case 'D':
		tilt.x += 0.01F;
		break;
	case 'W':
		tilt.y += 0.01F;
		break;
	case 'S':
		tilt.y -= 0.01F;
		break;
	case 'Q':
		camAngle -= ((GLfloat)3.141592/180.F)*1.F;
		break;
	case 'E':
		camAngle += ((GLfloat)3.141592 / 180.F) * 1.F;
		break;

	case 'Z':
		camAngleMap -= ((GLfloat)3.141592 / 180.F) * 1.F;
		break;
	case 'X':
		camAngleMap += ((GLfloat)3.141592 / 180.F) * 1.F;
		break;
	case 'C':
		radius -= (GLfloat)0.1F;
		break;
	case 'V':
		radius += (GLfloat)0.1F;
		break;
	case 'P':

		screenShotFunc();
	default:
		break;
	}

}



int main()
{
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(WIN_W, WIN_H, "viewing", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, keyFunc);
	glewInit();

	vert = glCreateShader(GL_VERTEX_SHADER);
	const char* vertSource = FileLoad("viewing.vert");
	printf(vertSource);
	glShaderSource(vert, 1, &vertSource, NULL);
	glCompileShader(vert);

	frag = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragSource = FileLoad("viewing.frag");
	printf(fragSource);
	glShaderSource(frag, 1, &fragSource, NULL);
	glCompileShader(frag);

	prog = glCreateProgram();
	glAttachShader(prog, vert);
	glAttachShader(prog, frag);
	glLinkProgram(prog);
	glUseProgram(prog);


	glGenBuffers(5, vbo);
	glGenBuffers(5, ibo);
	prepareiBO(0, 5 * 8 * sizeof(GLfloat), 18 * sizeof(GLuint), vertPosPyramid, facePyramid);
	prepareiBO(1, 8 * 8 * sizeof(GLfloat), 36 * sizeof(GLuint), vertPosCube, faceCube);
	prepareiBO(2, 3 * 8 * sizeof(GLfloat),  3 * sizeof(GLuint), vertPosCam, faceCam);
	prepareDisplay(3, 4 * 6 * sizeof(GLfloat),  6 * sizeof(GLuint), displayVertBuf, displayFaceBuf);

	iniTex();
	initFBO();

	GLuint locDisplayMode = glGetUniformLocation(prog,"displayMode");
	

	while (!glfwWindowShouldClose(window))
	{

		glfwGetWindowSize(window ,&winCurSize_w ,&winCurSize_h);
		update();
	
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glUniform1i(locDisplayMode , 0);
		drawFunc();
		drawMiniMap();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUniform1i(locDisplayMode, 1);
		drawDisplay();


		glFinish();
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	glfwTerminate();



	
	return 0;

}


void screenShotFunc()
{
	BITMAPFILEHEADER fh;
	BITMAPINFOHEADER ih;

	fh.bfType = 'MB';
	fh.bfSize = sizeof(screenShotBuf) + 54;
	fh.bfReserved1 = NULL;
	fh.bfReserved2 = NULL;


	ih.biSize = 40;
	ih.biWidth = WIN_W;
	ih.biHeight = WIN_H;
	ih.biPlanes = 1;
	ih.biBitCount = 32;
	ih.biCompression = 0;
	ih.biSizeImage = WIN_W * WIN_H;
	ih.biXPelsPerMeter = WIN_W;
	ih.biYPelsPerMeter = WIN_H;
	ih.biClrUsed = 0;
	ih.biClrImportant = 0;
	fh.bfOffBits = sizeof(fh) + sizeof(ih);

	HANDLE hFile;
	DWORD dwByte = 0;

	hFile = CreateFileA("screenShotTest.bmp", GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return;
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glReadBuffer(GL_COLOR_ATTACHMENT1);
	glReadPixels(0, 0,WIN_W, WIN_H, GL_BGRA, GL_UNSIGNED_BYTE, screenShotBuf);

	WriteFile(hFile, &fh, sizeof(fh), &dwByte, NULL);
	printf("write filehead : %d \n", dwByte);
	WriteFile(hFile, &ih, sizeof(ih), &dwByte, NULL);
	printf("write infohead : %d \n", dwByte);


	WriteFile(hFile, &screenShotBuf, WIN_H * WIN_W * 4, &dwByte, NULL);
	printf("write image : %d \n", dwByte);

	CloseHandle(hFile);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
