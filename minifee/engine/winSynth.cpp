#include "winSynth.h"
#include "math.h"
#include "xmplay.c"
#include <algorithm>

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
	xmVolume = 2.0f;
	noteVolume = 0.02f;
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
	for (int i = 0; i < nbSamples; i++)
	{
		xmMixBuffer[i * 2 + 0] = 0;
		xmMixBuffer[i * 2 + 1] = 0;
	}

	xm_software_mix_to_buffer(xmMixBuffer, nbSamples);

	for (int i = 0; i < nbSamples; i++) {
		buf[i * 2 + 0] = (float)xmMixBuffer[i * 2 + 0] * xmVolume / UINT_MAX;
		buf[i * 2 + 1] = (float)xmMixBuffer[i * 2 + 1] * xmVolume / UINT_MAX;
	}

	for (int n = 0; n < notes.size(); n++) {
		synthNote &note = notes[n];

		int soundEnd = note.duration - note.time;
		if (soundEnd > nbSamples) {
			soundEnd = nbSamples;
		}
		if (soundEnd < 0) {
			soundEnd = 0;
		}

		int soundStart = -note.time;
		if (soundStart < 0) {
			soundStart = 0;
		}

		int t = ((note.time + soundStart) * (int)note.frequency);

		if (note.instrument == synthInstrument::SQUARE_WAVE) {
			t %= waveFormat->nSamplesPerSec;
			for (int i = soundStart; i < soundEnd; i++) {
				float f = (float)t / waveFormat->nSamplesPerSec;
				buf[i * 2 + 0] += noteVolume * (f > 0.5f ? 1.0f : -1.0f);
				buf[i * 2 + 1] += noteVolume * (f > 0.5f ? 1.0f : -1.0f);
				t = (t + (int)note.frequency) % waveFormat->nSamplesPerSec;
			}
		}

		else if (note.instrument == synthInstrument::MOD_SINE) {
			float env = (float)(note.time + soundStart) * 18.0f / waveFormat->nSamplesPerSec;
			float envAdd = (float) 18.0f / waveFormat->nSamplesPerSec;
			for (int i = soundStart; i < soundEnd; i++) {
				float f = (float)t / waveFormat->nSamplesPerSec;
				if (env > 1.0f) {
					env = 1.0f;
				}
				buf[i * 2 + 0] += env * noteVolume * sin(f * 2.0f * 3.141569 + 2.25f * sin(f * .25f * 2.0f * 3.141569));
				buf[i * 2 + 1] += env * noteVolume * sin(f * 2.0f * 3.141569 + 2.25f * sin(f * .25f * 2.0f * 3.141569));
				t = (t + (int)note.frequency);
				env += envAdd;
			}
			float env1 = 1.0f - (float)(note.time + soundEnd - note.duration) * 32.0f / waveFormat->nSamplesPerSec;
			float env2 = 0.5f - (float)(note.time + soundEnd - note.duration) * 2.0f / waveFormat->nSamplesPerSec;
			float envAdd1 = -32.0f / waveFormat->nSamplesPerSec;
			float envAdd2 = -1.0f / waveFormat->nSamplesPerSec;

			for (int i = soundEnd; i < nbSamples; i++) {
				float f = (float)t / waveFormat->nSamplesPerSec;
				float env = env1 > env2 ? env1 : env2;
				if (env < 0.0f) {
					env = 0.0f;
				}
				buf[i * 2 + 0] += env * noteVolume * sin(f * 2.0f * 3.141569 + 2.25f * sin(f * .25f * 2.0f * 3.141569));
				buf[i * 2 + 1] += env * noteVolume * sin(f * 2.0f * 3.141569 + 2.25f * sin(f * .25f * 2.0f * 3.141569));
				t = (t + (int)note.frequency);
				env1 += envAdd1;
				env2 += envAdd2;
			}
		}

		note.time += nbSamples;

		if (note.time >= note.duration + 1.0f * waveFormat->nSamplesPerSec) {
			notes[n] = notes[notes.size() - 1];
			notes.pop_back();
			n -= 1;
		}
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
	std::string path = "xm/" + name + ".xm";

	XM_Song* songPtr;

	songPtr = xm_song_alloc();

	//xm_loader_set_recompress_all_samples(xm_true);

	xm_loader_open_song(path.c_str(), songPtr);

	xmSongs.push_back(songPtr);
	return xmSongs.size() - 1;
}

void winSynth::playXM(int i)
{
	if (i < 0 || i > xmSongs.size()) {
		xm_player_stop();
		return;
	}

	xm_player_set_song(xmSongs[i]);
	xm_player_play();
}

void winSynth::playString(std::string string, synthInstrument instrument)
{
	std::transform(string.begin(), string.end(), string.begin(), ::tolower);

	int octave = 3;
	int note = -1;
	float duration = 1.0f / 32.0f;
	float start = 0.0f;

	for (int i = 0; i < string.size(); i++)
	{
		bool play = false;
		char c = string[i];
		if (c == 'a') {
			note = 0;
			play = true;
		}
		if (c == 'b') {
			note = 2;
			play = true;
		}
		if (c == 'c') {
			note = -9;
			play = true;
		}
		if (c == 'd') {
			note = -7;
			play = true;
		}
		if (c == 'e') {
			note = -5;
			play = true;
		}
		if (c == 'f') {
			note = -4;
			play = true;
		}
		if (c == 'g') {
			note = -2;
			play = true;
		}
		if (c == '0') {
			octave = 0;
		}
		if (c == '1') {
			octave = 1;
		}
		if (c == '2') {
			octave = 2;
		}
		if (c == '3') {
			octave = 3;
		}
		if (c == '4') {
			octave = 4;
		}
		if (c == '5') {
			octave = 5;
		}
		if (c == '6') {
			octave = 6;
		}
		if (c == '-') {
			octave = octave > 0 ? octave - 1 : 0;
		}
		if (c == '+') {
			octave = octave < 6 ? octave + 1 : 6;
		}
		if (c == 'x') {
			note = -1;
			play = true;
		}
		if (c == 'z') {
			duration = 1.0f / 64.0f;
		}
		if (c == 't') {
			duration = 1.0f / 32.0f;
		}
		if (c == 's') {
			duration = 1.0f / 16.0f;
		}
		if (c == 'i') {
			duration = 1.0f / 8.0f;
		}
		if (c == 'q') {
			duration = 1.0f / 4.0f;
		}
		if (c == 'h') {
			duration = 1.0f / 2.0f;
		}
		if (c == 'w') {
			duration = 1.0f;
		}
		if (c == '.') {
			duration *= 1.5f;
		}
		if (c == '!') {
			duration /= 3.0f;
		}

		if (play && note == -1) {
			play = false;
			start += duration;
		}

		if (play && (i + 1 < string.length())) {
			char c = string[i + 1];
			if (c == '#') {
				note += 1;
			}
			if (c == '$') {
				note -= 1;
			}
		}

		if (play) {
			synthNote newNote;
			newNote.time = (int)(-start * 1.2f * waveFormat->nSamplesPerSec);
			newNote.duration = (int)(duration * 1.2f * waveFormat->nSamplesPerSec);
			newNote.instrument = instrument;
			newNote.frequency = (float)(440.0 * pow(2, (double)note/12.0 + octave - 3.0));
			notes.push_back(newNote);

			start += duration;
		}
	}
}