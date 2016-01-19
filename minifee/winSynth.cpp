#include "winSynth.h"
#include "math.h"

winSynth::winSynth(Config *config)
{
	this->config = config;

	audioClient = nullptr;
	audioRenderClient = nullptr;
	waveFormat = nullptr;
	running = false;
}

void threadFunc(void *synthObj)
{
	winSynth* synthPtr = (winSynth*)synthObj;
	synthPtr->loop();
}

void winSynth::loop()
{
	HRESULT hr;
	UINT32 bufferSize;
	hr = audioClient->GetBufferSize(&bufferSize);

	while (running) {
		UINT32 paddingSize;
		audioClient->GetCurrentPadding(&paddingSize);

		if (paddingSize <= bufferSize / 2) {
			BYTE* buffer;
			hr = audioRenderClient->GetBuffer(bufferSize / 2, &buffer);

			if (buffer) {
				float* floatBuffer = (float*)buffer;

				fillBuffer(floatBuffer, (int)bufferSize / 2);

				audioRenderClient->ReleaseBuffer(bufferSize / 2, 0);
			}
		}
		Sleep(0);
	}
}

void winSynth::fillBuffer(float *buf, int nbSamples)
{
	static int ctr = 0;
	static float fbk = 0;
	for (int i = 0; i < nbSamples; i++)
	{
		float bla = ctr + fbk * 100;
		//float out = sin(sin(bla * 0.05f) * 3.0f + bla * 0.01f) * sin(bla * 0.0005f + 600.3f * sin(bla * 0.00003f)) * 0.1f;
		float out = sin(bla * 0.05f) * 0.0f;
		buf[i * 2 + 0] = out;
		buf[i * 2 + 1] = out;
		fbk = out;
		
		ctr += 1;
	}

}


void winSynth::start()
{
	HRESULT hr;
	hr = CoInitialize(nullptr);
	IMMDeviceEnumerator *deviceEnumerator;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, IID_PPV_ARGS(&deviceEnumerator));
	IMMDevice *device;
	hr = deviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eRender, ERole::eMultimedia, &device);

	hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&audioClient);
	
	WAVEFORMATEXTENSIBLE* waveFormatExtensible = NULL;
	hr = audioClient->GetMixFormat(&waveFormat);
	if (waveFormat->wFormatTag != WAVE_FORMAT_EXTENSIBLE) {
		OutputDebugString("Sound format isn't extensible");
	}
	
	waveFormatExtensible = (WAVEFORMATEXTENSIBLE*)waveFormat;

	if (waveFormatExtensible->SubFormat != KSDATAFORMAT_SUBTYPE_IEEE_FLOAT) {
		OutputDebugString("sound format isn't IEEE FLOAT");
	}

	REFERENCE_TIME bufferDuration = (REFERENCE_TIME)config->vars["sound_buffer_duration"];

	hr = audioClient->Initialize(AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED, 0, bufferDuration, 0, waveFormat, NULL);

	hr = audioClient->GetService(__uuidof(IAudioRenderClient), (void**)&audioRenderClient);
	
	hr = audioClient->Start();

	running = true;
	soundThread = std::thread(threadFunc, this);
}

winSynth::~winSynth()
{
	running = false;
	soundThread.join();
}
