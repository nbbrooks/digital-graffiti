#include "DigitalGraffiti.h"
#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <mmsystem.h>
#include <time.h>
#include <pthread.h>
#include "Wall.h"

bool DEBUG = false;
bool collision = false, success = false;
float xx = -1.0;
float yy = -1.0;
int color = -1;

#pragma region GLOBAL_VARIABLES
time_t startT		= 0;
time_t endT			= 0;
#pragma endregion

#pragma comment(lib, "winmm.lib")

using namespace std;
// a simple class declaration part

/**
* Structure for passing a thread an object pointer and function pointer
*/
struct thread_args {
	Wall* e;
	void (Wall::*func)();
};

int main(int argc, char** argv) {
	DigitalGraffiti digitalGraffiti(argc, argv);
}


DigitalGraffiti::DigitalGraffiti(int argc, char** argv) {
	bool USE_KINECT = false;

	/////////
	// KINECT
	/////////
	Collision *obj;
	if(USE_KINECT) {
		cout << "Creating Collision object" << endl;
		obj = new Collision();
		obj->initialize();
	}

	////////
	// SERVER
	////////
	//Server server;

	////////
	// SOUND
	////////
	cout << "Creating Sound object" << endl;
	Sound sound;

	///////
	// WALL
	///////
	cout << "Creating Wall object" << endl;
	int width = 1024;
	int height = 768;
	pthread_t launchThread;
	Wall wall(width, height, argc, argv);
	thread_args v;
	v.e = &wall;
	v.func = wall.func();
	// Start thread with pointer to Wall object and pointer to Wall start function
	pthread_create(&launchThread, NULL, &Wall::threadMaker, static_cast<void*>(&v));
	// Wait for GLUT to finish setting up the window
	Sleep(2000);
	if(!USE_KINECT) {
		Sleep(60000);
		return;
	}

	std::cout << "This is a program for Children's Museum" << std::endl;

	int mode = MODE_PAINT;
	wall.setMode(mode);
	endT = time(0) + PAINT_TIME;
	while(1)
	{
		switch(mode) {
		case(MODE_PAINT):
			// Poll the Kinect
			if(USE_KINECT) {
				color = -1;
				obj->pollColorFrame();
				obj->pollDepthFrame(obj->SPECTRUM);
				if(obj->detect(5, &xx, &yy, &color)) {
					// If there are collisions, trigger visualizations
					printf("Collision of color %d at [%d, %d]\n", color, xx, yy);
					if(wall.addPaint(xx, yy, color)) {
						cout << "Visulization" << endl;
						// If there are visualizations, trigger sounds
						sound.playSplatSound();
					}
				}
			}
			if(DEBUG) {	
				cvShowImage("Depth", obj->depthFrame);
				cvMoveWindow("Depth", 0, 0);
				cvShowImage("Color", obj->colorFrame);
				cvMoveWindow("Color", 700, 0);
			}
			if(difftime(endT, time(0)) <= 0.0) {
				printf("Switch to cleanup start mode\n");
				// Switch to cleanup start mode
				mode = MODE_CLEANUP_START;
				wall.setMode(mode);
				// Start cleanup music
				sound.playBackgroundSound();
				// Calculate new endTime
				endT = time(0) + CLEANUP_START_TIME;
			}
			break;
		case(MODE_CLEANUP_START):
			if(difftime(endT, time(0)) <= 0.0) {
				printf("Switch to cleanup url mode\n");
				// Switch to cleanup url mode
				mode = MODE_CLEANUP_URL;
				wall.setMode(mode);
				// Calculate new endTime
				endT = time(0) + CLEANUP_URL_TIME;
			}
			break;
		case(MODE_CLEANUP_URL):
			if(difftime(endT, time(0)) <= 0.0) {
				printf("Switch to paint mode\n");
				// Switch to paint mode
				mode = MODE_PAINT;
				wall.setMode(mode);
				// Play splat sound
				sound.playSplatSound();
				// Calculate new endTime
				endT = time(0) + PAINT_TIME;
			}
			break;
		}
	}
}
