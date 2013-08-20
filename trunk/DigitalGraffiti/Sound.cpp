#include "Sound.h"
#include <Windows.h>
#include "DigitalGraffiti.h"

Sound::Sound(void)
{
	// Find music and sound files
	std::string exeDir = DigitalGraffiti::getExeDirectory();
	DigitalGraffiti::getFileList(exeDir + "\\sound\\instructions\\*", instructionsMusicList);
	DigitalGraffiti::getFileList(exeDir + "\\sound\\cleanup\\*", cleanupMusicList);
	DigitalGraffiti::getFileList(exeDir + "\\sound\\splat\\*", splatSoundList);
	instructionsCounter = 0;
	cleanupCounter = 0;
	splatCounter = 0;
	numInstructions= instructionsMusicList.size();
	numCleanup = cleanupMusicList.size();
	numSplat = splatSoundList.size();
	if(DigitalGraffiti::DEBUG)
	{
		printf("Sound directory is: %s\n", exeDir.c_str());
		printf("\tnumInstructions = %u\n", numInstructions);
		printf("\tnumCleanup = %u\n", numCleanup);
		printf("\tnumSplat = %u\n", numSplat);
	}
}

void Sound::playInstructionsMusic(void) 
{
	if(numInstructions > 0)
	{
		if(DigitalGraffiti::DEBUG)
		{
			printf("Play %s\n", instructionsMusicList[instructionsCounter].c_str());
		}
		PlaySound(TEXT(instructionsMusicList[instructionsCounter].c_str()), NULL, SND_FILENAME | SND_ASYNC| SND_NOWAIT);
		instructionsCounter = (instructionsCounter + 1) % numInstructions;
	}
}

void Sound::playCleanupMusic(void) 
{
	if(numCleanup > 0)
	{
		if(DigitalGraffiti::DEBUG)
		{
			printf("Play %s\n", cleanupMusicList[cleanupCounter].c_str());
		}
		PlaySound(TEXT(cleanupMusicList[cleanupCounter].c_str()), NULL, SND_FILENAME | SND_ASYNC| SND_NOWAIT);
		cleanupCounter = (cleanupCounter + 1) % numCleanup;
	}
}

void Sound::playSplatSound(void) 
{
	if(numSplat > 0)
	{
		if(DigitalGraffiti::DEBUG)
		{
			printf("Play %s\n", splatSoundList[splatCounter].c_str());
		}
		PlaySound(TEXT(splatSoundList[splatCounter].c_str()), NULL, SND_FILENAME | SND_ASYNC| SND_NOWAIT);
		splatCounter = (splatCounter + 1) % numSplat;
	}
}