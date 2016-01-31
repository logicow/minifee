#include "EnemyLargeDevil.h"
#include "..\gameState.h"
#include "PlayerBulletSpark.h"
#include "player.h"
#include "EnemyBulletLargeShot.h"

EnemyLargeDevil::EnemyLargeDevil(float x, float y)
{
	w = 24;
	h = 24;
	this->x = x - w / 2;
	this->y = y - h / 2;
	this->dir = 0;
	remove = false;
	timer = 0;
	attackTimer = ((float)rand() / RAND_MAX) * 3.0f;
	attacking = false;

	goDown = false;
	goUp = false;
	goLeft = false;
	goRight = false;

	cooldown = 0.0f;

	hp = 6;
	hitHp = hp;
	hitTimer = 0.0f;
	previousHp = hp;

	dramaticDeathTimer = 0.0f;
	deathSparkCooldown = 0.0f;
}

extern bool move(float& x, float& y, float w, float h, float dx, float dy, gameState& state);

void EnemyLargeDevil::update(gameState& state)
{
	if (hp < hitHp && hitTimer > 0.0f) {
		hp = hitHp;
	}

	hitTimer -= state.graphics->frameTime;
	if (hp < previousHp && hp > 0) {
		hitTimer = 2.0f;
		state.synth->playString("a#e-a#e-a#e-a#e", synthInstrument::SQUARE_WAVE);
		hitHp = hp;
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

	previousHp = hp;

	if (hp > 0) {
		timer += state.graphics->frameTime;
		attackTimer -= state.graphics->frameTime;
	}

	Player* player = state.action.player;

	if (attackTimer < 0) {
		attacking = !attacking;
		attackTimer = ((float)rand() / RAND_MAX) * (attacking ? 1.5f : 1.5f) + (6 - hp) * -0.06f + 0.5f;

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
			cooldown = 0.0f;

			if (player->x + player->w / 2 + 4 < x + w / 2) {
				dir = 0;
			}
			if (player->x + player->w / 2 - 4 > x + w / 2) {
				dir = 1;
			}
		}
	}

	cooldown -= state.graphics->frameTime;
	if (cooldown < -0.2f) {
		cooldown = -0.2f;
	}
	if (hp > 0 && player && attacking && cooldown <= 0.0f) {
		cooldown += 0.8f - (6 - hp) * 0.12f;
		state.synth->playString("zc-c-c", synthInstrument::SQUARE_WAVE);
		float rx = ((float)rand() / RAND_MAX - 0.5f)*80.0f;
		float ry = ((float)rand() / RAND_MAX - 0.5f)*80.0f;
		state.action.enemyBullets.push_back(new EnemyBulletLargeShot(x + w / 2, y + h / 2, player->x + player->w / 2 + rx, player->y + player->h / 2 + ry));
	}

	if (hp > 0 && player && player->x + player->w > x && player->x < x + w && player->y + player->h > y && player->y < y + h) {
		player->dead = true;
	}


	float dx = 0;
	float dy = 0;

	float speed = 30.0f + (6 - hp) * 20.0f;

	if (goLeft) {
		dx -= state.graphics->frameTime * speed;
	}
	if (goRight) {
		dx += state.graphics->frameTime * speed;
	}
	if (goUp) {
		dy -= state.graphics->frameTime * speed;
	}
	if (goDown) {
		dy += state.graphics->frameTime * speed;
	}

	if (hp > 0) {
		move(x, y, w, h, dx, dy, state);
	}

	int frame = (attacking ? ((int)(timer * 4.0f) % 2) + 1 : 0) + dir * 3;

	int& s = state.graphics->spriteCount;
	state.graphics->spritePtr[s].pos = DirectX::XMFLOAT2(x - state.action.cameraX - 4, y - state.action.cameraY - 4);
	state.graphics->spritePtr[s].sprite_color = hitTimer > 0.0f ? (int)(hitTimer * 10.0f + 1.0f) % 2 : 0;
	if (hp <= 0) {
		state.graphics->spritePtr[s].sprite_color = 1;
	}
	state.graphics->spritePtr[s].sprite_flags = 0;
	state.graphics->spritePtr[s].sprite_index = state.idx_enemyLargeDemon + frame;
	s += 1;
}