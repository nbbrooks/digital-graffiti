#pragma once

#include <pthread.h>
#ifdef _WIN32
#  include <windows.h>
#  include <GL/glew.h>
#  include <GL/glut.h>
#endif
#ifdef __linux
#  include <GL/glut.h>
#  include <GL/glew.h>
#  include "util.h"
#  include <unistd.h>
#endif
#ifdef __APPLE__
#  include <GLUT/glut.h>
#endif

class Wall
{
public:
	Wall(int, int, int, char **);
	~Wall();
	void (Wall::*func())()
	{
		return &Wall::start;
	}
	bool getStop(void);
	void start(void);
	void stop(pthread_t &);

	static std::string codeShape, codeLetter, codeNumber, imageCode;
	static const std::string SHAPE_LOOKUP[];

	static bool addPaint(float, float, int);
	static std::string getImageCode();
	static void printRybToRgb(float, float, float);
	static void saveImage(std::string);
	static void setMode(int);
	static void* threadMaker(void* dat_s);
	static void updateImageCode();

private:
	static bool kill;
	static int mode, wallWidth, wallHeight, colorIndex, splatIndex, argc;
	static const int GRAVITY_SKIP, RED_INDEX, GREEN_INDEX, BLUE_INDEX, TEXT_LEFT, TEXT_CENTER, TEXT_RIGHT;
	static char **argv;
	static time_t modeStart;

	static float cubicInt(float, float, float);
	static void *fileContents(const char *, GLint *);
	static int getIndex(int, int, int);
	static unsigned char getRed(float, float, float);
	static unsigned char getGreen(float, float, float);
	static unsigned char getBlue(float, float, float);
	static GLuint makeBuffer(GLenum, const void *, GLsizei);
	static GLuint makeProgram(GLuint, GLuint);
	static int makeResources(void);
	static GLuint makeShader(GLenum, const char *);
	static GLuint makeTextureWall();
	static GLuint makeTexture(const char *);
	static void mouseButton(int, int, int, int);
	static void printStringBitmap(std::string, int, float);
	static void printStringStroke(std::string, int, float, float);
	static void render(void);
	static void resetAssets();
	static void setRybToRgb(int, int, float, float, float);
	static void setup(int, char **);
	static void showInfoLog(GLuint, PFNGLGETSHADERIVPROC, PFNGLGETSHADERINFOLOGPROC);
	static void updateFadeFactor(void);
	static void updateIdle(int);
	static void updatePaint(int);
	static void updateTextureWall(GLuint);
	static void updateWallImage(int);
};