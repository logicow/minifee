#include "Game.h"

Game::Game(Graphics *aGraphics, Gamepad *aGamepad, winSynth *aSynth)
{
	graphics = aGraphics;
	gamepad = aGamepad;
	synth = aSynth;
}

void Game::update()
{
	static double px = 2;
	static double py = 2;

	if (gamepad->pressed[gamepadButton::LEFT]) {
		px -= 100 * graphics->frameTime;
	}
	if (gamepad->pressed[gamepadButton::RIGHT]) {
		px += 100 * graphics->frameTime;
	}
	if (gamepad->pressed[gamepadButton::UP]) {
		py -= 100 * graphics->frameTime;
	}
	if (gamepad->pressed[gamepadButton::DOWN]) {
		py += 100 * graphics->frameTime;
	}

	static bool previousPressed = false;
	if (gamepad->pressed[gamepadButton::A] && !previousPressed) {
		synth->playString("ic1c+ca#+c1c+cx+d#1c+ca#+c1c+cx+c1c+ca#+c1c+cx+d#1c+c+fc1c+cx", synthInstrument::MOD_SINE);
		//synth->playString("2c+c+c2d#+d#+d#2g+g+g3cd#g", synthInstrument::SQUARE_WAVE);
	}
	previousPressed = gamepad->pressed[gamepadButton::A];

	static bool previousPressed2 = false;
	if (gamepad->pressed[gamepadButton::B] && !previousPressed2) {
		//synth->playString("ic1c+ca#+c1c+cx+d#1c+ca#+c1c+cx+c1c+ca#+c1c+cx+d#1c+c+fc1c+cx", synthInstrument::SQUARE_WAVE);
		synth->playString("2c+c+c2d#+d#+d#2g+g+g3cd#g", synthInstrument::SQUARE_WAVE);
	}
	previousPressed2 = gamepad->pressed[gamepadButton::B];

	graphics->startUpdateSprites();
	static double f = 0;
	f += 200.0 * graphics->frameTime;
	if (f >= 320.0) {
		f = 0;
	}
	for (int i = 1; i < 2; i++) {
		graphics->spritePtr[i].pos = DirectX::XMFLOAT2((float)f + i*0.01f, i*0.01f);
		graphics->spritePtr[i].sprite_color = 1;
		graphics->spritePtr[i].sprite_flags = 0;
		graphics->spritePtr[i].sprite_index = 1;
	}

	graphics->spritePtr[0].pos = DirectX::XMFLOAT2((float)px, (float)py);
	graphics->spritePtr[0].sprite_color = 1;
	graphics->spritePtr[0].sprite_flags = 0;
	graphics->spritePtr[0].sprite_index = 0;

	graphics->spriteCount = 2;

	
}

#include "winSynth.h"
int Game::run()
{
	graphics->setWindow(320, 200, "Minifee");
	
	graphics->startLoad();
	graphics->loadSpriteRect("small", 0, 0, 16, 16, false);
	graphics->loadSprite("title", false);
	graphics->endLoad();

	graphics->startUpdatePalette();
	for (int i = 0; i < 256; i++) {
		graphics->palettePtr[i] = 0xFF000000 | (i * 0x123456);
	}

	graphics->startUpdateSpriteLookup();
	for (int i = 0; i < 256; i++) {
		graphics->spriteLookupPtr[i] = i;
	}
	for (int i = 0; i < 256; i++) {
		graphics->spriteLookupPtr[256 + i] = i == 0 ? 0 : 255 - i;
	}

	synth->loadXM("coolio");
	synth->playXM(0);

	int frameC = 0;
	while (!graphics->shouldExit())
	{
		gamepad->update();
		update();
		graphics->swap();
		graphics->frameTime;
		Sleep(0);
	}
	return 0;
}