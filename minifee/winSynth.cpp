#include "winSynth.h"
#include "math.h"

void winSynth::start()
{
	HRESULT hr;
	IMMDeviceEnumerator *deviceEnumerator;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, IID_PPV_ARGS(&deviceEnumerator));
	IMMDevice *device;
	hr = deviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eRender, ERole::eMultimedia, &device);
	IAudioClient *audioClient;
	hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&audioClient);

	WAVEFORMATEX *waveFormat;
	WAVEFORMATEXTENSIBLE* waveFormatExtensible = NULL;
	hr = audioClient->GetMixFormat(&waveFormat);
	if (waveFormat->wFormatTag != WAVE_FORMAT_EXTENSIBLE) {
		OutputDebugString("Sound format isn't extensible");
	}
	
	waveFormatExtensible = (WAVEFORMATEXTENSIBLE*)waveFormat;

	if (waveFormatExtensible->SubFormat != KSDATAFORMAT_SUBTYPE_IEEE_FLOAT) {
		OutputDebugString("sound format isn't IEEE FLOAT");
	}

	hr = audioClient->Initialize(AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED, 0, 0, 0, waveFormat, NULL);

	IAudioRenderClient *audioRenderClient;
	hr = audioClient->GetService(__uuidof(IAudioRenderClient), (void**)&audioRenderClient);
	
	hr = audioClient->Start();

	UINT32 bufferSize;
	hr = audioClient->GetBufferSize(&bufferSize);
	
	while (true) {
		UINT32 paddingSize;
		audioClient->GetCurrentPadding(&paddingSize);

		if (paddingSize <= bufferSize / 2) {
			BYTE* buffer;
			hr = audioRenderClient->GetBuffer(bufferSize / 2, &buffer);

			if (buffer) {
				float* floatBuffer = (float*)buffer;

				for (int i = 0; i < (int)bufferSize / 2; i++) {
					for (int channel = 0; channel < waveFormat->nChannels; channel++) {
						floatBuffer[channel + i*2] = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
					}
				}

				audioRenderClient->ReleaseBuffer(bufferSize / 2, 0);
			}
		}
	}
}