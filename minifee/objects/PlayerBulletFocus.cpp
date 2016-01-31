#include "PlayerBulletFocus.h"
#include "..\gameState.h"
#include "PlayerBulletSpark.h"
#include "Enemy.h"

PlayerBulletFocus::PlayerBulletFocus(float x, float y, int dir)
{
	w = 8;
	h = 8;
	this->x = x - w/2;
	this->y = y - h/2;
	this->dir = dir;
	remove = false;
	timer = 0;
}

extern bool move(float& x, float& y, float w, float h, float dx, float dy, gameState& state);

void PlayerBulletFocus::update(gameState& state)
{
	timer += state.graphics->frameTime;

	float dx = 0;
	if (dir == 0) {
		dx -= state.graphics->frameTime * 160.0f;
	}
	if (dir == 1) {
		dx += state.graphics->frameTime * 160.0f;
	}

	bool hit = move(x, y, w, h, dx, 0, state);

	for (int i = 0; i < state.action.enemies.size(); i++) {
		Enemy* e = state.action.enemies[i];
		if (!hit && e->hp >= 1 && e->x < x + w && e->x + e->w > x && e->y < y + h && e->y + e->h > y) {
			e->hp -= 1;
			hit = true;
		}
	}

	if (hit) {
		remove = true;
		state.synth->playString("1d#x", synthInstrument::SQUARE_WAVE);
		state.action.playerBullets.push_back(new PlayerBulletSpark(x - 0 + dir * 8, y + h / 2));
	}

	if (timer >= 5.0f) {
		remove = true;
	}

	int& s = state.graphics->spriteCount;
	state.graphics->spritePtr[s].pos = DirectX::XMFLOAT2(x - state.action.cameraX - 4, y - state.action.cameraY - 4);
	state.graphics->spritePtr[s].sprite_color = 0;
	state.graphics->spritePtr[s].sprite_flags = 0;
	state.graphics->spritePtr[s].sprite_index = state.idx_playerBulletFocus + ((int)(timer * 30.0f) % 2) + dir * 2;
	s += 1;
}