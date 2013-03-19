#include "Sound.h"
#include <Windows.h>
#include "DigitalGraffiti.h"

Sound::Sound(void)
{
	// Find music and sound files
	std::string exeDir = DigitalGraffiti::getExeDirectory();
	DigitalGraffiti::getFileList(exeDir + "\\sound\\cleanup\\*", cleanupMusicList);
	DigitalGraffiti::getFileList(exeDir + "\\sound\\splat\\*", splatSoundList);
	cleanupCounter = 0;
	splatCounter = 0;
	numCleanup = cleanupMusicList.size();
	numSplat = splatSoundList.size();
	if(DigitalGraffiti::DEBUG)
	{
		printf("numCleanup = %u\n", numCleanup);
		printf("numSplat = %u\n", numSplat);
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
		PlaySound(cleanupMusicList[cleanupCounter].c_str(), NULL, SND_FILENAME | SND_ASYNC| SND_NOWAIT);
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
		PlaySound(splatSoundList[splatCounter].c_str(), NULL, SND_FILENAME | SND_ASYNC| SND_NOWAIT);
		splatCounter = (splatCounter + 1) % numSplat;
	}
}