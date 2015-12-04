#include "Game.h"

Game::Game(Graphics *aGraphics, Gamepad *aGamepad)
{
	graphics = aGraphics;
	gamepad = aGamepad;
}

void Game::update()
{
	static double px = 100;
	static double py = 100;

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

	graphics->startUpdateSprites();
	static double f = 0;
	f += 200.0 * graphics->frameTime;
	if (f >= 320.0) {
		f = 0;
	}
	for (int i = 1; i < 40; i++) {
		graphics->spritePtr[i].pos = DirectX::XMFLOAT2((float)f + i*0.01f, i*0.01f);
		graphics->spritePtr[i].sprite_color = 1;
		graphics->spritePtr[i].sprite_flags = 0;
		graphics->spritePtr[i].sprite_index = 1;
	}

	graphics->spritePtr[0].pos = DirectX::XMFLOAT2((float)px, (float)py);
	graphics->spritePtr[0].sprite_color = 1;
	graphics->spritePtr[0].sprite_flags = 0;
	graphics->spritePtr[0].sprite_index = 0;

	graphics->spriteCount = 40;

	
}

#include "winSynth.h"
int Game::run()
{
	graphics->setWindow(320, 200, "Minifee");
	
	graphics->startLoad();
	graphics->loadSprite("small", false);
	graphics->loadSprite("title", false);
	graphics->endLoad();

	winSynth synth;
	synth.start();

	graphics->startUpdatePalette();
	for (int i = 0; i < 256; i++) {
		graphics->palettePtr[i] = 0xFFFF0000 | ((i * 0xFF) & 0xFF);
	}

	graphics->startUpdateSpriteLookup();
	for (int i = 0; i < 256; i++) {
		graphics->spriteLookupPtr[i] = i;
	}
	for (int i = 0; i < 256; i++) {
		graphics->spriteLookupPtr[256 + i] = i == 0 ? 0 : 255 - i;
	}

	int frameC = 0;
	while (!graphics->shouldExit())
	{
		gamepad->update();
		update();
		graphics->swap();
		graphics->frameTime;
	}
	return 0;
}