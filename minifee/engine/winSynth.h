#pragma once
#include "Audioclient.h"
#include "Mmdeviceapi.h"
#include <thread>
#include "Config.h"
#include "xmplay.h"
#include <vector>

class winSynth
{
public:
	winSynth(Config* config);
	~winSynth();
	
	void start();
	void loop();

	int loadXM(std::string name);
	void playXM(int i);

private:
	IAudioClient *audioClient;
	IAudioRenderClient *audioRenderClient;
	WAVEFORMATEX *waveFormat;
	std::thread soundThread;
	bool running;
	Config *config;

	XM_AudioLock xmAudioLock;
	XM_MemoryManager xmMemoryManager;
	XM_FileIO xmFileIO;
	std::vector<XM_Song*> xmSongs;
	xm_s32 *xmMixBuffer;
	float xmVolume;

	void fillBuffer(float *bufferStereoInterleaved, int nbSamples);
};