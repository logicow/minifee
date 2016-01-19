#include "windows.h"

#include "Game.h"
#include "winGraphics.h"
#include "winGamepad.h"
#include "Config.h"
#include "winSynth.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Config config;
	winGraphics graphics(hInstance, hPrevInstance, lpCmdLine, nCmdShow, &config);
	winGamepad gamepad;
	winSynth synth(&config);
	synth.start();
	Game game((Graphics*)&graphics, (Gamepad*)&gamepad);

	game.run();

	return EXIT_SUCCESS;
}