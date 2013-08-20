#pragma once

#include <string>
#include <vector>

class  Sound
{
public:
	Sound(void);
	void playInstructionsMusic(void);
	void playCleanupMusic(void);
	void playSplatSound(void);

private:
	int instructionsCounter, cleanupCounter, splatCounter, numInstructions, numCleanup, numSplat;
	std::vector<std::string> instructionsMusicList;
	std::vector<std::string> cleanupMusicList;
	std::vector<std::string> splatSoundList;
};