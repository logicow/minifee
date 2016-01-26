#include "winSynth.h"
#include "math.h"
#include "xmplay.c"

FILE *xmFile;
xm_bool xmFileBigEndian;

void lockAudio()
{
	//wat do
}

void unlockAudio()
{
	//wat do
}

void* allocMem(uint32_t size, XM_MemoryAllocType allocType)
{
	return malloc(size);
}

void freeMem(void* ptr, XM_MemoryAllocType allocType)
{
	free(ptr);
}

xm_bool fileIOInUse()
{
	return xmFile ? xm_true : xm_false;
}

XM_FileIOError fileIOOpen(const char* fileName, xm_bool bigEndianMode)
{
	if (xmFile) {
		return XM_FILE_ERROR_IN_USE;
	}

	errno_t err = fopen_s(&xmFile, fileName, "rb");

	if (!xmFile) {
		return XM_FILE_ERROR_CANT_OPEN;
	}

	xmFileBigEndian = bigEndianMode;
	return XM_FILE_OK;
}

xm_u8 fileIOGetu8()
{
	xm_u8 b;
	fread(&b, 1, 1, xmFile);
	return b;
}

xm_u16 fileIOGetu16()
{
	xm_u8 a, b;

	a = fileIOGetu8();
	b = fileIOGetu8();

	return ((xm_u16)b << 8) | a;
}

xm_u32 fileIOGetu32()
{
	xm_u16 a, b;
	
	a = fileIOGetu16();
	b = fileIOGetu16();

	return ((xm_u32)b << 16) | a;
}

void fileIOGetByteArray(xm_u8 *dest, xm_u32 count)
{
	fread(dest, count, 1, xmFile);
}

void fileIOSeekPos(xm_u32 offset)
{
	fseek(xmFile, offset, SEEK_SET);
}

xm_u32 fileIOGetPos()
{
	return ftell(xmFile);
}

xm_bool fileIOEOFReached()
{
	return feof(xmFile) ? xm_true : xm_false;
}
void fileIOClose()
{
	fclose(xmFile);
}

winSynth::winSynth(Config *config)
{
	this->config = config;

	audioClient = nullptr;
	audioRenderClient = nullptr;
	waveFormat = nullptr;
	running = false;
	xmFile = nullptr;
	xmMixBuffer = nullptr;
	xmVolume = 1.0f;
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

	xmMixBuffer = (xm_s32*)malloc(sizeof(xm_s32) * bufferSize * 2);

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

	free(xmMixBuffer);
}

void winSynth::fillBuffer(float *buf, int nbSamples)
{
	static int ctr = 0;
	static float fbk = 0;
	
	for (int i = 0; i < nbSamples; i++)
	{
		xmMixBuffer[i * 2 + 0] = 0;
		xmMixBuffer[i * 2 + 1] = 0;
	}

	xm_software_mix_to_buffer(xmMixBuffer, nbSamples);

	for (int i = 0; i < nbSamples; i++)
	{
		//float bla = ctr + fbk * 100;
		//float out = sin(sin(bla * 0.05f) * 3.0f + bla * 0.01f) * sin(bla * 0.0005f + 600.3f * sin(bla * 0.00003f)) * 0.1f;
		//float out = sin(bla * 0.05f) * 0.1f;
		float out = sin(ctr * 0.05f) * 0.0f;

		buf[i * 2 + 0] = out + (float)xmMixBuffer[i * 2 + 0] / UINT_MAX;
		buf[i * 2 + 1] = out + (float)xmMixBuffer[i * 2 + 1] / UINT_MAX;
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

	xmAudioLock.lock = lockAudio;
	xmAudioLock.unlock = unlockAudio;

	xmMemoryManager.alloc = allocMem;
	xmMemoryManager.free = freeMem;

	xmFileIO.open = fileIOOpen;
	xmFileIO.in_use = fileIOInUse;
	xmFileIO.close = fileIOClose;
	xmFileIO.eof_reached = fileIOEOFReached;
	xmFileIO.get_byte_array = fileIOGetByteArray;
	xmFileIO.get_pos = fileIOGetPos;
	xmFileIO.get_u16 = fileIOGetu16;
	xmFileIO.get_u32 = fileIOGetu32;
	xmFileIO.get_u8 = fileIOGetu8;
	xmFileIO.seek_pos = fileIOSeekPos;

	xm_set_audio_lock(&xmAudioLock);
	xm_set_memory_manager(&xmMemoryManager);
	xm_loader_set_fileio(&xmFileIO);

	xm_create_software_mixer(waveFormat->nSamplesPerSec, (xm_u8)32);

	running = true;
	soundThread = std::thread(threadFunc, this);
}

winSynth::~winSynth()
{
	running = false;
	soundThread.join();
}

int winSynth::loadXM(std::string name)
{
	std::string path = "../data/xm/" + name + ".xm";

	XM_Song* songPtr;

	songPtr = xm_song_alloc();

	xm_loader_open_song(path.c_str(), songPtr);

	xmSongs.push_back(songPtr);
	return xmSongs.size() - 1;
}

void winSynth::playXM(int i)
{
	xm_player_set_song(xmSongs[i]);
	xm_player_play();
}