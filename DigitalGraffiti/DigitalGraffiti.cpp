#include "DigitalGraffiti.h"
#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <mmsystem.h>
#include <time.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h> 
#include <errno.h>

#ifndef S_ISDIR
#define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#endif

#ifndef S_ISREG
#define S_ISREG(mode)  (((mode) & S_IFMT) == S_IFREG)
#endif

bool collision = false, success = false;
int xx = -1;
int yy = -1;
int color = -1;
bool played = false;

#pragma region GLOBAL_VARIABLES
time_t startT		= 0;
time_t endT			= 0;
#pragma endregion

#pragma comment(lib, "winmm.lib")

const std::string DigitalGraffiti::DEBUG_SETTINGS_FILE = "graffiti-settings.txt";
bool DigitalGraffiti::useKinect = true;
bool DigitalGraffiti::useWall = true;
int DigitalGraffiti::instructionsTime, DigitalGraffiti::paintTime, DigitalGraffiti::cleanupStartTime, DigitalGraffiti::cleanupUrlTime;

/**
* Structure for passing a thread an object pointer and function pointer
*/
struct thread_args 
{
	Wall* e;
	void (Wall::*func)();
};

int main(int argc, char** argv) 
{
	DigitalGraffiti digitalGraffiti(argc, argv);
}


DigitalGraffiti::DigitalGraffiti(int argc, char** argv) 
{
	std::vector<int> hits, feedbackOld, feedbackNew;
	std::ifstream settings;
	std::string line;
	settings.open(DEBUG_SETTINGS_FILE);
	if(settings.is_open())
	{
		if(settings.good())
		{
			getline(settings, line);
			if(line.compare("false") == 0)
			{
				useKinect = false;
			}
		}
		if(settings.good())
		{
			getline(settings, line);
			if(line.compare("false") == 0)
			{
				useWall = false;
			}
		}
		if(settings.good())
		{
			getline(settings, line);
			paintTime = atoi(line.c_str());
		}
		if(settings.good())
		{
			getline(settings, line);
			cleanupStartTime = atoi(line.c_str());
		}
		if(settings.good())
		{
			getline(settings, line);
			cleanupUrlTime = atoi(line.c_str());
			success = true;
		}
		if(settings.good())
		{
			getline(settings, line);
			instructionsTime = atoi(line.c_str());
		}
		if(DigitalGraffiti::DEBUG)
		{
			printf("useKinect = %s\nuseWall = %s\npaintTime = %u\ncleanupStartTime = %u\ncleanupUrlTime = %u\ninstructionsTime = %u\n", (useKinect ? "true" : "false"), (useWall ? "true" : "false"), paintTime, cleanupStartTime, cleanupUrlTime, instructionsTime);
		}
	}
	settings.close();
	if(!success)
	{
		fprintf(stderr, "Could not find debug settings file: %s\n", DEBUG_SETTINGS_FILE);
		exit(-1);
	}

	/////////
	// KINECT
	/////////
	Kinect *obj;
	if(useKinect) 
	{
		if(DEBUG)
		{
			printf("Creating Kinect object\n");
		}
		obj = new Kinect();
		obj->initialize(CREATE_CONFIG);
	}

	/////////
	// SERVER
	/////////
	//Server server;

	////////
	// SOUND
	////////
	if(DEBUG)
	{
		printf("Creating Sound object\n");
	}
	Sound sound;

	///////
	// WALL
	///////
	if(DEBUG)
	{
		printf("Creating Wall object\n");
	}
	int width = 1000;
	int height = 800;
	pthread_t launchThread;
	printf("create wall start\n");
	Wall wall(obj->getWidth(), obj->getHeight(), argc, argv);
	printf("create wall start end\n");
	if(useWall) 
	{
		thread_args v;
		v.e = &wall;
		v.func = wall.func();
		// Start thread with pointer to Wall object and pointer to Wall start function
		pthread_create(&launchThread, NULL, &Wall::threadMaker, static_cast<void*>(&v));
		// Wait for GLUT to finish setting up the window
		Sleep(2000);
	}


	int mode;
	if(instructionsTime != 0) 
	{
		if(DEBUG)
		{
			printf("Start in instruction mode\n");
		}
		mode = MODE_INSTRUCTIONS;
		endT = time(NULL) + instructionsTime;
		sound.playInstructionsMusic();
	}
	else
	{
		if(DEBUG)
		{
			printf("Start in paint mode\n");
		}
		mode = MODE_PAINT;
		endT = time(NULL) + paintTime;
		sound.playSplatSound();
	}
	if(useWall)
	{
		wall.setMode(mode);
	}
	while(1)
	{
		switch(mode) 
		{
		case(MODE_CALIBRATE):
			break;
		case(MODE_INSTRUCTIONS):
			// Display instructions.tga
			if(difftime(endT, time(NULL)) <= 0.0) 
			{
				// Switch to cleanup url mode
				if(DEBUG)
				{
					printf("Switch to paint mode\n");
				}
				mode = MODE_PAINT;
				endT = time(NULL) + paintTime;
				sound.playSplatSound();
				if(useWall) 
				{
					wall.setMode(mode);
				}
			}
			break;
		case(MODE_PAINT):
			// Poll the Kinect
			if(useKinect) 
			{
				color = -1;
				played = false;
				obj->pollFrames(Kinect::DISCRETE);
				if(obj->detectMultiFeedback(&feedbackOld, &hits, &feedbackNew)) 
				{
					// If there are collisions, trigger visualizations
					for(unsigned int ii = 0; ii < hits.size(); ii+=3)
					{
						if(DEBUG) 
						{	
							std::cout << hits[ii] << "," << hits[ii + 1] << ": " << hits[ii + 2] << std::endl;
						}
						if(useWall) 
						{
							if(wall.addPaint(hits[ii], hits[ii + 1], hits[ii + 2]) && !played) 
							{
								// If there are visualizations, trigger sounds
								sound.playSplatSound();
								played = true;
							}
						}
					}
				}

				feedbackOld=feedbackNew;
				feedbackNew.clear();
				hits.clear();
			}
			if(difftime(endT, time(NULL)) <= 0.0) 
			{
				// Clear old collision data
				if(DEBUG)
				{
					printf("Switch to cleanup start mode\n");
				}
				feedbackOld.clear();
				feedbackNew.clear();
				hits.clear();

				// Save the wall image
				wall.updateImageCode();
				wall.saveImage(Wall::getImageCode());

				// Switch to cleanup start mode
				mode = MODE_CLEANUP_START;
				if(useWall) 
				{
					wall.setMode(mode);
				}
				// Start cleanup music
				sound.playCleanupMusic();
				// Calculate new endTime
				endT = time(NULL) + cleanupStartTime;
			}
			break;
		case(MODE_CLEANUP_START):
			if(difftime(endT, time(NULL)) <= 0.0) 
			{
				// Switch to cleanup url mode
				if(DEBUG)
				{
					printf("Switch to cleanup url mode\n");
				}
				mode = MODE_CLEANUP_URL;
				if(useWall) 
				{
					wall.setMode(mode);
				}
				// Calculate new endTime
				endT = time(NULL) + cleanupUrlTime;
			}
			break;
		case(MODE_CLEANUP_URL):
			if(difftime(endT, time(NULL)) <= 0.0) 
			{
				if(instructionsTime == 0)
				{
					if(DEBUG)
					{
						printf("Switch to paint mode\n");
					}
					mode = MODE_PAINT;
					endT = time(NULL) + paintTime;
					sound.playSplatSound();
				}
				else
				{
					if(DEBUG)
					{
						printf("Switch to instructions mode\n");
					}
					mode = MODE_INSTRUCTIONS;
					endT = time(NULL) + instructionsTime;
					sound.playInstructionsMusic();
				}
				if(useWall) 
				{
					wall.setMode(mode);
				}
			}
			break;
		}
	}
}

bool DigitalGraffiti::fileExists(const std::string& filename)
{
	struct stat buf;
	if (stat(filename.c_str(), &buf) != -1)
	{
		return true;
	}
	return false;
}

void DigitalGraffiti::listDirectory(std::string absolutePath)
{
	bool isFile;
	int success;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	printf("Path is %s\n", absolutePath);
	hFind = FindFirstFile(absolutePath.c_str(), &FindFileData);
	std::stringstream absoluteFilename;
	absoluteFilename << absolutePath.substr(0, absolutePath.size() - 1);
	absoluteFilename << FindFileData.cFileName;
	isFile = DigitalGraffiti::isFile(absoluteFilename.str());
	if (hFind == INVALID_HANDLE_VALUE) 
	{
		fprintf (stderr, "FindFirstFile failed (%d)\n", GetLastError());
		return;
	} 
	else 
	{
		printf("The first item found is %s;\tIs file? %s\n", FindFileData.cFileName, isFile ? "true":"false");
		success = FindNextFile(hFind, &FindFileData);
		while(success != 0)
		{
			printf("The next item found is %s;\tIs file? %s\n", FindFileData.cFileName, isFile ? "true":"false");
			success = FindNextFile(hFind, &FindFileData);
			absoluteFilename.str(std::string());
			absoluteFilename << absolutePath.substr(0, absolutePath.size() - 1);
			absoluteFilename << FindFileData.cFileName;
			isFile = DigitalGraffiti::isFile(absoluteFilename.str());
		}
		FindClose(hFind);
	}
}

void DigitalGraffiti::getFileList(std::string absolutePath, std::vector<std::string>& returnList)
{
	bool isFile;
	int success;
	HANDLE hFind;
	WIN32_FIND_DATA FindFileData;

	hFind = FindFirstFile(absolutePath.c_str(), &FindFileData);
	std::stringstream absoluteFilename;
	absoluteFilename << absolutePath.substr(0, absolutePath.size() - 1);
	absoluteFilename << FindFileData.cFileName;
	isFile = DigitalGraffiti::isFile(absoluteFilename.str());
	if (hFind == INVALID_HANDLE_VALUE) 
	{
		fprintf (stderr, "FindFirstFile failed (%d)\n", GetLastError());
		return;
	} 
	else 
	{
		if(isFile) {
			returnList.push_back(absoluteFilename.str());
		}
		success = FindNextFile(hFind, &FindFileData);
		while(success != 0)
		{
			absoluteFilename.str(std::string());
			absoluteFilename << absolutePath.substr(0, absolutePath.size() - 1);
			absoluteFilename << FindFileData.cFileName;
			isFile = DigitalGraffiti::isFile(absoluteFilename.str());
			if(isFile) {
				returnList.push_back(absoluteFilename.str());
			}
			success = FindNextFile(hFind, &FindFileData);
		}
		FindClose(hFind);
	}
}

bool DigitalGraffiti::isFile(std::string absoluteFilename)
{
	struct stat st;
	int ret = stat(absoluteFilename.c_str(), &st);
	if(ret != 0 || S_ISDIR(st.st_mode))
	{
		return false;
	}
	return true;
}

std::string DigitalGraffiti::getExeDirectory(void)
{
	TCHAR NPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, NPath);
	std::string path = NPath;
	return path;
}