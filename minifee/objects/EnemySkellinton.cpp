#include "EnemySkellinton.h"
#include "..\gameState.h"
#include "PlayerBulletSpark.h"
#include "player.h"

EnemySkellinton::EnemySkellinton(float x, float y)
{
	w = 8;
	h = 8;
	this->x = x - w / 2;
	this->y = y - h / 2;
	this->dir = 0;
	remove = false;
	timer = 0;
	walkTimer = ((float)rand() / RAND_MAX) * 5.0f;
	walking = true;
	hp = 1;
}

extern bool move(float& x, float& y, float w, float h, float dx, float dy, gameState& state);

void EnemySkellinton::update(gameState& state)
{
	if (hp <= 0) {
		state.synth->playString("1a+a+a", synthInstrument::SQUARE_WAVE);
		state.action.playerBullets.push_back(new PlayerBulletSpark(x + h / 2, y + h / 2));
		remove = true;
		return;
	}

	timer += state.graphics->frameTime;
	walkTimer -= state.graphics->frameTime;

	if (walkTimer < 0) {
		walking = !walking;
		walkTimer = ((float)rand() / RAND_MAX) * (walking ? 5.0f : 3.0f);
	}

	Player* player = state.action.player;
	
	if (player && player->x + player->w > x && player->x < x + w && player->y + player->h > y && player->y < y + h) {
		player->dead = true;
	}

	bool goLeft = false;
	bool goRight = false;
	bool goUp = false;
	bool goDown = false;
	
	if (player && walking) {
		if (player->x + player->w/2 + 4 < x + w/2) {
			goLeft = true;
			dir = 0;
		}
		if (player->x + player->w/2 - 4 > x + w/2) {
			goRight = true;
			dir = 1;
		}
		if (player->y + player->h / 2 + 4 < y + h / 2) {
			goUp = true;
		}
		if (player->y + player->h / 2 - 4 > y + h / 2) {
			goDown = true;
		}
	}

	float dx = 0;
	float dy = 0;

	if (goLeft) {
		dx -= state.graphics->frameTime * 20.0f;
	}
	if (goRight) {
		dx += state.graphics->frameTime * 20.0f;
	}
	if (goUp) {
		dy -= state.graphics->frameTime * 20.0f;
	}
	if (goDown) {
		dy += state.graphics->frameTime * 20.0f;
	}

	for (int i = 0; i < state.action.enemies.size(); i++) {
		Enemy* e = state.action.enemies[i];
		if (e != this) {
			if (e->x < x + w && e->x + e->w > x && e->y < y + h && e->y + e->h > y) {
				if (e->x + e->w / 2 > x + w / 2) {
					dx -= 30.0f * state.graphics->frameTime;
				}
				else {
					dx += 30.0f * state.graphics->frameTime;
				}
				if (e->y + e->h / 2 > y + h / 2) {
					dy -= 30.0f * state.graphics->frameTime;
				}
				else {
					dy += 30.0f * state.graphics->frameTime;
				}
			}
		}
	}

	move(x, y, w, h, dx, dy, state);

	int frame = (walking ? ((int)(timer * 4.0f) % 2) + 1 : 0) + dir * 3;

	int& s = state.graphics->spriteCount;
	state.graphics->spritePtr[s].pos = DirectX::XMFLOAT2(x - state.action.cameraX - 4, y - state.action.cameraY - 4);
	state.graphics->spritePtr[s].sprite_color = 0;
	state.graphics->spritePtr[s].sprite_flags = 0;
	state.graphics->spritePtr[s].sprite_index = state.idx_enemySkellinton + frame;
	s += 1;
}