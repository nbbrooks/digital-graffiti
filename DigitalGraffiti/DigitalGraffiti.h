#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <ctime>
//#include <omp.h>

#include "cv.h"
#include "highgui.h"
#include "cxcore.h"
#include "cvaux.h"
#include "CLNUIDevice.h"
#include "Collision.h"
#include "Sound.h"

class DigitalGraffiti {
public:
	const static int RED_BALL = 0, YELLOW_BALL = 1, BLUE_BALL = 2, NUM_BALL_COLORS = 3;
	const static int MODE_PAINT = 0, MODE_CLEANUP_START = 1, MODE_CLEANUP_URL = 2;

	DigitalGraffiti(int, char **);
private:
	const static int PAINT_TIME = 60;
	const static int CLEANUP_START_TIME = 30;
	const static int CLEANUP_URL_TIME = 30;
};
