#include "Player.h"
#include "../gameState.h"
#include "PlayerBulletFocus.h"
#include "PlayerBulletSpark.h"

bool introx;
bool introy;
bool introFirst = true;

Player::Player(float x, float y)
{
	w = 16.0f;
	h = 16.0f;
	this->x = x - w / 2;
	this->y = y - h / 2;
	dir = 0;
	remove = false;
	cooldown = 0;
	dead = false;
	deadTimer = 0.0f;
	invinco = 3.0f;

	introx = true;
	introy = true;
}

bool solidAt(float x, float y, float w, float h, gameState& state)
{
	int width;
	int height;
	state.graphics->readTilemapTiles(0, width, height);

	if (x < 0 || (int)((x + w - 0.0001f)/16) >= width || y < 0 || (int)((y + h - 0.0001f)/16) >= height) {
		return true;
	}

	for (int ix = (int)(x / 16); ix <= (int)((x + w - 0.0001f) / 16); ix++) {
		for (int iy = (int)(y / 16); iy <= (int)((y + h - 0.0001f) / 16); iy++) {
			uint16_t tile = state.graphics->tilemapTilePtr[iy * state.graphics->tilemapTileLineOffset + ix];
			if (tile < 53) {
				return true;
			}
		}
	}

	return false;
}

bool move(float& x, float& y, float w, float h, float dx, float dy, gameState& state)
{
	bool hit = false;
	if (dx > 0) {
		int a = 0;
		while (a < 10 && solidAt(x + dx, y, w, h, state)) {
			a += 1;
			hit = true;
			if (x + dx + w != ((int)(x + dx + w) / 16) * 16) {
				dx = ((int)(x + dx + w) / 16) * 16 - x - w;
			}
			else {
				dx -= 16;
			}
		}
	}

	if (dx < 0) {
		int a = 0;
		while (a < 10 && solidAt(x + dx, y, w, h, state)) {
			a += 1;
			hit = true;
			if (x + dx != ((int)(x + dx) / 16) * 16) {
				dx = ((int)(x + dx) / 16) * 16 - x + 16;
			}
			else {
				dx += 16;
			}
		}
	}

	x += dx;

	if (dy > 0) {
		int a = 0;
		while (a < 10 && solidAt(x, y + dy, w, h, state)) {
			a += 1;
			hit = true;
			if (y + dy + h != ((int)(y + dy + h) / 16) * 16) {
				dy = ((int)(y + dy + h) / 16) * 16 - y - h;
			}
			else {
				dy -= 16;
			}
		}
	}

	if (dy < 0) {
		int a = 0;
		while (a < 10 && solidAt(x, y + dy, w, h, state)) {
			a += 1;
			hit = true;
			if (y + dy != ((int)(y + dy) / 16) * 16) {
				dy = ((int)(y + dy) / 16) * 16 - y + 16;
			}
			else {
				dy += 16;
			}
		}
	}

	y += dy;

	return hit;
}

void Player::update(gameState& state)
{
	float dx = 0;
	float dy = 0;

	float speed = 80.0f;

	if (state.gamepad->pressed[gamepadButton::LEFT]) {
		dx -= speed * state.graphics->frameTime;
		dir = 0;
	}
	if (state.gamepad->pressed[gamepadButton::RIGHT]) {
		dx += speed * state.graphics->frameTime;
		dir = 1;
	}
	if (state.gamepad->pressed[gamepadButton::UP]) {
		dy -= speed * state.graphics->frameTime;
	}
	if (state.gamepad->pressed[gamepadButton::DOWN]) {
		dy += speed * state.graphics->frameTime;
	}

	if (introx || introy) {
		dx = 0;
		dy = 0;
	}
	else {
		introFirst = false;
	}

	static float walking = 0;
	if (dx != 0 || dy != 0) {
		walking += state.graphics->frameTime;
	}
	else {
		walking = 0;
	}

	if (dead) {
		dx /= 2;
		dy /= 2;
	}

	move(x, y, w, h, dx, dy, state);

	

	float targetCameraX = x + w / 2 - 160;
	float targetCameraY = y + h / 2 - 100;

	if (introy) {
		if(state.action.cameraY < targetCameraY) {
			state.action.cameraY += state.graphics->frameTime * (introFirst ? 200.5f : 900.0f);
		}
		else {
			introy = false;
		}
	}
	else {
		state.action.cameraY = targetCameraY;
	}

	if (introx) {
		if (state.action.cameraX < targetCameraX) {
			state.action.cameraX += state.graphics->frameTime * (introFirst ? 46.5f : 250.0f);
		}
		else {
			introx = false;
		}
	}
	else {
		state.action.cameraX = targetCameraX;
	}

	int frame = ((int)(walking * 10.0f) % 2) + dir * 2;

	invinco -= state.graphics->frameTime;
	if (invinco > 0.0f) {
		dead = false;
	}

	if (dead) {
		if (deadTimer <= 0.0f) {
			state.synth->playString("+c-gd#cd#c-gd#gd#c-g+c-gd#cd#c-gd#gd#c", synthInstrument::SQUARE_WAVE);
		}
		frame = ((int)(deadTimer * 10.0f) % 4) + 4;
		deadTimer += state.graphics->frameTime;
		if (deadTimer >= 1.0f) {
			remove = true;
			state.action.playerBullets.push_back(new PlayerBulletSpark(x + w/2, y + h / 2));
			return;
		}
	}

	int& s = state.graphics->spriteCount;
	state.graphics->spritePtr[s].pos = DirectX::XMFLOAT2(x - state.action.cameraX, y - state.action.cameraY);
	state.graphics->spritePtr[s].sprite_color = 0;
	state.graphics->spritePtr[s].sprite_flags = 0;
	state.graphics->spritePtr[s].sprite_index = state.idx_player + frame;
	s += 1;

	static bool fired = false;

	cooldown -= state.graphics->frameTime;

	bool keyPressed = false;
	if (state.gamepad->pressed[gamepadButton::START] || state.gamepad->pressed[gamepadButton::A] || state.gamepad->pressed[gamepadButton::Y] || state.gamepad->pressed[gamepadButton::B] || state.gamepad->pressed[gamepadButton::X]) {
		keyPressed = true;
	}
	if (introx || introy) {
		keyPressed = false;
	}

	if (!dead && keyPressed && cooldown <= 0.0f) {
		if (!fired) {
			state.action.playerBullets.push_back(new PlayerBulletFocus(x - 0 + dir*16, y + h / 2, dir));
			state.synth->playString("z+c-c", synthInstrument::SQUARE_WAVE);
		}
		cooldown += 0.0;
		fired = true;
	}
	else {
		fired = false;
	}
}