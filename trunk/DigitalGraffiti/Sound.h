#pragma once

#include <string>
#include <vector>

class  Sound
{
public:
	Sound(void);
	void playCleanupMusic(void);
	void playSplatSound(void);

private:
	int cleanupCounter, splatCounter, numCleanup, numSplat;
	std::vector<std::string> cleanupMusicList;
	std::vector<std::string> splatSoundList;
};