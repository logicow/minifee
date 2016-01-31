#include "EnemyLargeSkellinton.h"
#include "..\gameState.h"
#include "PlayerBulletSpark.h"
#include "player.h"
#include "EnemySkellinton.h"

EnemyLargeSkellinton::EnemyLargeSkellinton(float x, float y)
{
	w = 24;
	h = 24;
	this->x = x - w / 2;
	this->y = y - h / 2;
	this->dir = 0;
	remove = false;
	timer = 0;
	walkTimer = ((float)rand() / RAND_MAX) * 5.0f;
	walking = true;
	hp = 6;
	hitHp = hp;
	hitTimer = 0.0f;
	previousHp = hp;

	dramaticDeathTimer = 0.0f;
	deathSparkCooldown = 0.0f;
}

extern bool move(float& x, float& y, float w, float h, float dx, float dy, gameState& state);

void EnemyLargeSkellinton::update(gameState& state)
{
	if (hp < hitHp && hitTimer > 0.0f) {
		hp = hitHp;
	}

	hitTimer -= state.graphics->frameTime;
	if (hp < previousHp && hp > 0) {
		hitTimer = 2.0f;
		state.synth->playString("a#e-a#e-a#e-a#e", synthInstrument::SQUARE_WAVE);
		hitHp = hp;

		int spawns = 4 + (6 - hp) * 5;
		for (int i = 0; i < spawns; i++) {
			float rx = ((float)rand() / RAND_MAX - 0.5f)*16.0f;
			float ry = ((float)rand() / RAND_MAX - 0.5f)*16.0f;
			state.action.enemies.push_back(new EnemySkellinton(x + w / 2 + rx, y + h / 2 + ry));
		}
	}
	previousHp = hp;

	if (hp <= 0 && dramaticDeathTimer <= 0.0f) {
		//lol double it for double volume
		state.synth->playString("-sg-g+f-f+d#-d#+c-ca#-a#2c-c", synthInstrument::SQUARE_WAVE);
		state.synth->playString("-sg-g+f-f+d#-d#+c-ca#-a#2c-c", synthInstrument::SQUARE_WAVE);
		state.action.playerBullets.push_back(new PlayerBulletSpark(x + h / 2, y + h / 2));

		deathSparkCooldown = 0.0f;
		dramaticDeathTimer = 0.0f;
		hp = -42;
	}

	if (hp <= 0) {
		dramaticDeathTimer += state.graphics->frameTime;
		deathSparkCooldown -= state.graphics->frameTime;

		if (dramaticDeathTimer >= 5.0f) {
			remove = true;
			state.action.bossKills += 1;
			return;
		}

		if (deathSparkCooldown <= 0.0f) {
			deathSparkCooldown += 0.1f;
			float rx = ((float)rand() / RAND_MAX - 0.5f)*64.0f;
			float ry = ((float)rand() / RAND_MAX - 0.5f)*64.0f;
			state.action.playerBullets.push_back(new PlayerBulletSpark(x + h / 2 + rx, y + h / 2 + ry));
		}
	}

	if (hp > 0) {
		timer += state.graphics->frameTime;
		walkTimer -= state.graphics->frameTime;
	}

	if (walkTimer < 0) {
		walking = !walking;
		walkTimer = ((float)rand() / RAND_MAX) * (walking ? 5.0f : 3.0f);
	}

	Player* player = state.action.player;

	if (hp > 0 && player && player->x + player->w > x && player->x < x + w && player->y + player->h > y && player->y < y + h) {
		player->dead = true;
	}

	bool goLeft = false;
	bool goRight = false;
	bool goUp = false;
	bool goDown = false;

	if (player && walking) {
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
	}

	float dx = 0;
	float dy = 0;

	if (goLeft) {
		dx -= state.graphics->frameTime * 30.0f;
	}
	if (goRight) {
		dx += state.graphics->frameTime * 30.0f;
	}
	if (goUp) {
		dy -= state.graphics->frameTime * 30.0f;
	}
	if (goDown) {
		dy += state.graphics->frameTime * 30.0f;
	}

	if (hp > 0) {
		move(x, y, w, h, dx, dy, state);
	}

	int frame = (walking ? ((int)(timer * 4.0f) % 2) + 1 : 0) + dir * 3;

	int& s = state.graphics->spriteCount;
	state.graphics->spritePtr[s].pos = DirectX::XMFLOAT2(x - state.action.cameraX - 4, y - state.action.cameraY - 4);
	state.graphics->spritePtr[s].sprite_color = hitTimer > 0.0f ? (int)(hitTimer * 10.0f + 1.0f) % 2 : 0;
	if (hp <= 0) {
		state.graphics->spritePtr[s].sprite_color = 1;
	}
	state.graphics->spritePtr[s].sprite_flags = 0;
	state.graphics->spritePtr[s].sprite_index = state.idx_enemyLargeSkellinton + frame;
	s += 1;
}