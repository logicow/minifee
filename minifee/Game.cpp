#include "Game.h"

#include "slide.h"
#include "action.h"
#include "gameState.h"

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

	graphics->tilemapDrawPtr[0].pos = DirectX::XMFLOAT2((float)px * -2.0f, (float)py * -2.0f);
	graphics->tilemapDrawPtr[0].tilemap_index = 0;
	graphics->tilemapDrawCount = 1;

	int width, height;
	graphics->readTilemapTiles(0, width, height);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if ((x + y) & 1) {
				graphics->tilemapTilePtr[y * graphics->tilemapTileLineOffset + x] = 0;
			}
		}
	}
	graphics->writeTilemapTiles();
	
}

int Game::run()
{
	graphics->setWindow(320, 200, "Deadspooker");
	
	graphics->startLoad();
	
	gameState state;
	state.graphics = graphics;
	state.gamepad = gamepad;
	state.synth = synth;

	graphics->loadSprite("title", false);
	graphics->loadSprite("intro", false);
	graphics->loadSprite("win", false);

	for (int i = 0; i < 8; i++) {
		int idx = graphics->loadSpriteRect("sheet", 0 + i*16, 0, 16, 16, false);
		state.idx_player = idx - i;
	}

	for (int i = 0; i < 4; i++) {
		int idx = graphics->loadSpriteRect("sheet", 0 + i * 16, 16, 16, 16, false);
		state.idx_spark = idx - i;
	}

	for (int i = 0; i < 4; i++) {
		int idx = graphics->loadSpriteRect("sheet", 0 + i * 16, 32, 16, 16, false);
		state.idx_playerBulletFocus = idx - i;
	}

	for (int i = 0; i < 6; i++) {
		int idx = graphics->loadSpriteRect("sheet", 0 + i * 16, 48, 16, 16, false);
		state.idx_enemySkellinton = idx - i;
	}

	for (int i = 0; i < 6; i++) {
		int idx = graphics->loadSpriteRect("sheet", 16 * 6 + i * 32, 32, 32, 32, false);
		state.idx_enemyLargeSkellinton = idx - i;
	}

	for (int i = 0; i < 4; i++) {
		int idx = graphics->loadSpriteRect("sheet", 0 + i * 16, 64, 16, 16, false);
		state.idx_pentagram = idx - i;
	}

	for (int i = 0; i < 4; i++) {
		int idx = graphics->loadSpriteRect("sheet", 16 * 4 + i * 32, 64, 32, 32, false);
		state.idx_largePentagram = idx - i;
	}

	for (int i = 0; i < 3; i++) {
		int idx = graphics->loadSpriteRect("sheet", 0 + i * 16, 80, 16, 16, false);
		state.idx_drum = idx - i;
	}

	for (int i = 0; i < 3; i++) {
		int idx = graphics->loadSpriteRect("sheet", 0 + i * 16, 96, 16, 16, false);
		state.idx_guitar = idx - i;
	}

	for (int i = 0; i < 3; i++) {
		int idx = graphics->loadSpriteRect("sheet", 0 + i * 16, 112, 16, 16, false);
		state.idx_microphone = idx - i;
	}

	for (int i = 0; i < 1; i++) {
		int idx = graphics->loadSpriteRect("sheet", 16 * 3 + i * 16, 96, 32, 32, false);
		state.idx_bubble = idx - i;
	}

	for (int i = 0; i < 6; i++) {
		int idx = graphics->loadSpriteRect("sheet", 0 + i * 16, 128, 16, 16, false);
		state.idx_enemyDemon = idx - i;
	}

	for (int i = 0; i < 6; i++) {
		int idx = graphics->loadSpriteRect("sheet", 16 * 6 + i * 32, 112, 32, 32, false);
		state.idx_enemyLargeDemon = idx - i;
	}

	for (int i = 0; i < 4; i++) {
		int idx = graphics->loadSpriteRect("sheet", 0 + i * 6, 144, 6, 6, false);
		state.idx_enemyBulletShot = idx - i;
	}

	for (int i = 0; i < 4; i++) {
		int idx = graphics->loadSpriteRect("sheet", 24 + i * 12, 144, 12, 12, false);
		state.idx_enemyBulletLargeShot = idx - i;
	}

	graphics->loadMap("stage");

	graphics->endLoad();

	graphics->startUpdateSpriteLookup();
	for (int i = 0; i < 256; i++) {
		graphics->spriteLookupPtr[i] = i;
	}
	for (int i = 0; i < 256; i++) {
		graphics->spriteLookupPtr[256 + i] = i == 0 ? 0 : 5 - i;
	}

	synth->loadXM("YRRU");
	synth->playXM(0);

	startSlide(0, state);

	//startAction(0, state);

	int frameC = 0;
	while (!graphics->shouldExit() && state.updatePtr != nullptr)
	{
		gamepad->update();

		graphics->startUpdateSprites();
		graphics->startUpdateTilemapDraw();

		state.updatePtr(state);

		graphics->swap();
		Sleep(0);

		if (gamepad->pressed[gamepadButton::QUIT]) {
			state.updatePtr = nullptr;
		}
	}
	return 0;
}