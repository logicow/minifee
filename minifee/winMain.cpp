#include "windows.h"

#include "Game.h"
#include "winGraphics.h"
#include "winGamepad.h"
#include "Config.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Config config;
	winGraphics graphics(hInstance, hPrevInstance, lpCmdLine, nCmdShow, &config);
	winGamepad gamepad;
	Game game((Graphics*)&graphics, (Gamepad*)&gamepad);

	game.run();

	return EXIT_SUCCESS;
}