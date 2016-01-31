#include "EnemyDevil.h"
#include "..\gameState.h"
#include "PlayerBulletSpark.h"
#include "player.h"
#include "EnemyBulletShot.h"

EnemyDevil::EnemyDevil(float x, float y)
{
	w = 8;
	h = 8;
	this->x = x - w / 2;
	this->y = y - h / 2;
	this->dir = 0;
	remove = false;
	timer = 0;
	attackTimer = ((float)rand() / RAND_MAX) * 0.15f;
	attacking = false;
	hp = 5;
	previousHp = hp;
	hitTimer = 0.0f;

	goDown = false;
	goUp = false;
	goLeft = false;
	goRight = false;

	cooldown = 0.0f;
}

extern bool move(float& x, float& y, float w, float h, float dx, float dy, gameState& state);

void EnemyDevil::update(gameState& state)
{
	if (hp <= 0) {
		state.synth->playString("1a+a+a", synthInstrument::SQUARE_WAVE);
		state.action.playerBullets.push_back(new PlayerBulletSpark(x + h / 2, y + h / 2));
		remove = true;
		return;
	}

	if (hp < previousHp && hp > 0) {
		hitTimer = 0.1f;
		state.synth->playString("c-zgd#c-gd#c", synthInstrument::SQUARE_WAVE);
	}
	previousHp = hp;

	timer += state.graphics->frameTime;
	attackTimer -= state.graphics->frameTime;
	hitTimer -= state.graphics->frameTime;

	Player* player = state.action.player;

	if (attackTimer < 0) {
		attacking = !attacking;
		attackTimer = ((float)rand() / RAND_MAX) * (attacking ? 0.5f : 8.0f) + 1.0f;

		if (player && !attacking) {
			if (player->x + player->w / 2 + 4 < x + w / 2) {
				goLeft = true;
				dir = 0;
			}
			if (player->x + player->w / 2 - 4 > x + w / 2) {
				goRight = true;
				dir = 1;
			}
			if (player->y + player->h / 2 + 4 < y + h / 2) {
				goUp = true;
			}
			if (player->y + player->h / 2 - 4 > y + h / 2) {
				goDown = true;
			}

			float r = (float)rand() / RAND_MAX;
			if (r < 0.2f) {
				goLeft = true;
				goRight = false;
				dir = 0;
			}
			else if (r < 0.4f) {
				goRight = true;
				goLeft = false;
				dir = 1;
			}
			else if (r < 0.6f) {
				goUp = true;
				goDown = false;
			}
			else if (r < 0.8f) {
				goDown = true;
				goUp = false;
			}
		}

		if (player && attacking) {
			cooldown = 0.5f;
			goUp = false;
			goDown = false;
			goLeft = false;
			goRight = false;

			if (player->x + player->w / 2 + 4 < x + w / 2) {
				dir = 0;
			}
			if (player->x + player->w / 2 - 4 > x + w / 2) {
				dir = 1;
			}
		}
	}

	cooldown -= state.graphics->frameTime;
	if (cooldown < -0.05f) {
		cooldown = -0.05f;
	}
	if (player && attacking && cooldown <= 0.0f && attackTimer > 0.5f) {
		cooldown += 0.05f;
		state.synth->playString("zc-c-c", synthInstrument::SQUARE_WAVE);
		float rx = ((float)rand() / RAND_MAX - 0.5f)*64.0f;
		float ry = ((float)rand() / RAND_MAX - 0.5f)*64.0f;
		state.action.enemyBullets.push_back(new EnemyBulletShot(x + w / 2, y + h / 2, player->x + player->w / 2 + rx, player->y + player->h / 2 + ry));
	}

	if (player && player->x + player->w > x && player->x < x + w && player->y + player->h > y && player->y < y + h) {
		player->dead = true;
	}


	float dx = 0;
	float dy = 0;

	if (goLeft) {
		dx -= state.graphics->frameTime * 6.0f;
	}
	if (goRight) {
		dx += state.graphics->frameTime * 6.0f;
	}
	if (goUp) {
		dy -= state.graphics->frameTime * 6.0f;
	}
	if (goDown) {
		dy += state.graphics->frameTime * 6.0f;
	}

	for (int i = 0; i < state.action.enemies.size(); i++) {
		Enemy* e = state.action.enemies[i];
		if (e != this) {
			if (e->x < x + w && e->x + e->w > x && e->y < y + h && e->y + e->h > y) {
				if (e->x + e->w / 2 > x + w / 2) {
					dx -= 100.0f * state.graphics->frameTime;
				}
				else {
					dx += 100.0f * state.graphics->frameTime;
				}
				if (e->y + e->h / 2 > y + h / 2) {
					dy -= 100.0f * state.graphics->frameTime;
				}
				else {
					dy += 100.0f * state.graphics->frameTime;
				}
			}
		}
	}

	move(x, y, w, h, dx, dy, state);

	int frame = (attacking ? ((int)(timer * 4.0f) % 2) + 1 : 0) + dir * 3;

	int& s = state.graphics->spriteCount;
	state.graphics->spritePtr[s].pos = DirectX::XMFLOAT2(x - state.action.cameraX - 4, y - state.action.cameraY - 4);
	state.graphics->spritePtr[s].sprite_color = hitTimer > 0.0f ? 1 : 0;
	state.graphics->spritePtr[s].sprite_flags = 0;
	state.graphics->spritePtr[s].sprite_index = state.idx_enemyDemon + frame;
	s += 1;
}