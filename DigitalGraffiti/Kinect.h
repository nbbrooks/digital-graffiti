#pragma once

#include "stdafx.h"

class Kinect
{
public:
	Kinect();
	~Kinect();
	int getWidth(void);
	int getHeight(void);
	void pollFrames(int select);
	void initialize(bool preload);
	bool detectSingle(int *xx, int *yy, int *color);
	bool detectMulti(vector<int> *hits);
	bool detectMultiFeedback(vector<int> *feedbackOld, vector<int> *hits, vector<int> *feedbackNew);

	static const int WIDTH = 640;
	static const int HEIGHT = 480;
	static const int AREA = 307200;
	static const int K_BAW = 1;
	static const int K_RGB = 3;
	static const int BUFFER = 150;
	static const int NO_CONVERT = -1;
	static const int DISCRETE	= 0;
	static const int CONTINUOUS	= 1;
	static const int CONTOUR_WIDTH = 20;
	static const int CONTOUR_HEIGHT = 20;
	static const int COLOR_DIST_THRESH = 300;
	static const int FEEDBACK_DIST_THRESH = 200;
	static const int BUFFER_LENGTH = 8;
	static const int BALL_BOX = 4;
	static const char* SAMPLE_XML_PATH;
	static const float KINECT_WAIT;
	static const std::string CONFIG_FILE;

private:
	int minVal, maxVal;
	int avg_Red[3], avg_Yellow[3], avg_Blue[3];
	short rawDepthData[AREA];
	unsigned char rawColorData[AREA * 3], rgbDepthData[AREA * 3];
	CvFont myFont;
	CvPoint tl, br;
	CvRect roi;
	IplImage *colorFrame, *depthFrame;
	xn::Context g_context;
	xn::DepthGenerator g_depth;
	xn::ImageGenerator g_image;
	xn::DepthMetaData g_depthMD;
	xn::ImageMetaData g_imageMD;
	XnStatus rc;

	static void avgColor(unsigned char *rgbs, int xx_min, int yy_min, int xx_max, int yy_max, int color[3]);
	static void num2rgb(short *nums, unsigned char *rgbs, int numlen, int min, int max, int select);
	static void on_mouse(int click_event, int xx, int yy, int flags, void *param);
	static int squareDistance(int x1, int x2, int y1, int y2, int z1 = 0, int z2 = 0);
};