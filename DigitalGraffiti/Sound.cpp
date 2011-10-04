#include "Sound.h"
#include <Windows.h>

Sound::Sound(void) : NUM_BACKGROUND(2), NUM_SPLAT(2) {
	backgroundCount = 0;
	splatCount = 0;
	//Sound::NUM_BACKGROUND = 2;
	//Sound::NUM_COUNT = 2;
}

void Sound::playBackgroundSound(void) {
	switch(backgroundCount) {
	case(0):
		PlaySound(TEXT("Jumper.wav"), NULL, SND_FILENAME | SND_ASYNC| SND_NOWAIT);
		break;
	case(1):
		PlaySound(TEXT("Summer.wav"), NULL, SND_FILENAME | SND_ASYNC| SND_NOWAIT);
		break;
	}
	backgroundCount = (backgroundCount + 1) % NUM_BACKGROUND;
}
void Sound:: playSplatSound(void) {
	switch(splatCount) {
	case(0):
		PlaySound(TEXT("Splat1.wav"), NULL, SND_FILENAME | SND_ASYNC| SND_NOWAIT);
		break;
	case(1):
		PlaySound(TEXT("Splat2.wav"), NULL, SND_FILENAME | SND_ASYNC| SND_NOWAIT);
		break;
	}
	splatCount = (splatCount + 1) % NUM_SPLAT;
}