#include <pthread.h>
#ifdef _WIN32
#  include <windows.h>
#  include <GL/glew.h>
#  include <GL/glut.h>
//#  include <GL/gl.h>
//#  include <GL/glext.h>
//#  include <GL/wglext.h>
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
	static bool addPaint(float, float, int);
	static void setMode(int);
	void start(void);
	void stop(pthread_t &);
	bool getStop(void);
	static void* threadMaker(void* dat_s);
	static void printRybToRgb(float, float, float);
private:
	static bool kill;
	static int mode, wallWidth, wallHeight, colorIndex, splatIndex, argc;
	static const int GRAVITY_SKIP, RED_INDEX, GREEN_INDEX, BLUE_INDEX;
	static char **argv;

	static float cubicInt(float, float, float);
	static unsigned char getRed(float, float, float);
	static unsigned char getGreen(float, float, float);
	static unsigned char getBlue(float, float, float);
	static void setRybToRgb(int, int, float, float, float);
	static void *fileContents(const char *, GLint *);
	static int getIndex(int, int, int);
	static GLuint makeBuffer(GLenum, const void *, GLsizei);
	static GLuint makeProgram(GLuint, GLuint);
	static int makeResources(void);
	static GLuint makeShader(GLenum, const char *);
	static GLuint makeTextureWall();
	static void mouseButton(int, int, int, int);
	static void render(void);
	static void resetAssets(void);
	static void setup(int, char **);
	static void showInfoLog(GLuint, PFNGLGETSHADERIVPROC, PFNGLGETSHADERINFOLOGPROC);
	static void updateFadeFactor(void);
	static void updateIdle(void);
	static void updatePaint(int value);
	static void updateTextureWall(GLuint texture);
	static void test(void);
};