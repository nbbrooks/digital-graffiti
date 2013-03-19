#pragma once

#include <ctime>
#include <iostream>
#include "stdafx.h"

class DigitalGraffiti {
public:
	// Application settings
	const static bool DEBUG = true;
	const static bool FULL_SCREEN = true, USE_GRAVITY = true;
	const static int PAINT_TIME = 240, CLEANUP_START_TIME = 5, CLEANUP_URL_TIME = 5;
	// Constants
	const static int RED_BALL = 0, YELLOW_BALL = 1, BLUE_BALL = 2, NUM_BALL_COLORS = 3;
	const static int MODE_PAINT = 0, MODE_CLEANUP_START = 1, MODE_CLEANUP_URL = 2, MODE_CALIBRATE = 3;

	DigitalGraffiti(int, char **);

	static bool fileExists(const std::string &);
	static std::string getExeDirectory(void);
	static void getFileList(std::string, std::vector<std::string> &);
	static bool isFile(char *);
	static void listDirectory(std::string);

private:
	const static bool CREATE_CONFIG = false;
	const static bool USE_KINECT = true;
	const static bool USE_WALL = true;
};
