#pragma once
#include "Audioclient.h"
#include "Mmdeviceapi.h"
#include <thread>
#include "Config.h"

class winSynth
{
public:
	winSynth(Config* config);
	~winSynth();
	void start();
	void loop();
	

private:
	void fillBuffer(float *bufferStereoInterleaved, int nbSamples);
	
	IAudioClient *audioClient;
	IAudioRenderClient *audioRenderClient;
	WAVEFORMATEX *waveFormat;
	std::thread soundThread;
	bool running;
	Config *config;


};