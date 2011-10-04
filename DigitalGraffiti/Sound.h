class  Sound
{
public:
	Sound(void);
	void playBackgroundSound(void);
	void playSplatSound(void);
private:
	int backgroundCount, splatCount;
	const int NUM_BACKGROUND, NUM_SPLAT;
};