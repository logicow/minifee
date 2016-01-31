#include "EnemyBulletLargeShot.h"
#include "..\gameState.h"
#include "Player.h"
#include "PlayerBulletSpark.h"
#include "EnemyBulletShot.h"

extern float dist(float x1, float y1, float x2, float y2);

EnemyBulletLargeShot::EnemyBulletLargeShot(float x, float y, float tx, float ty)
{
	w = 8;
	h = 8;
	this->x = x - w / 2;
	this->y = y - h / 2;
	remove = false;
	timer = 0;

	dirx = (tx - x) / dist(x, y, tx, ty);
	diry = (ty - y) / dist(x, y, tx, ty);
	speed = 80.0f;
}

extern bool move(float& x, float& y, float w, float h, float dx, float dy, gameState& state);

void EnemyBulletLargeShot::update(gameState& state)
{
	timer += state.graphics->frameTime;

	Player* player = state.action.player;

	if (player && player->x + player->w > x && player->x < x + w && player->y + player->h > y && player->y < y + h) {
		player->dead = true;
		remove = true;
		state.synth->playString("1d#x", synthInstrument::SQUARE_WAVE);
		state.action.playerBullets.push_back(new PlayerBulletSpark(x + w / 2, y + h / 2));
	}

	float dx = dirx * speed * state.graphics->frameTime;
	float dy = diry * speed * state.graphics->frameTime;

	bool hit = move(x, y, w, h, dx, dy, state);
	if (hit) {
		remove = true;
		state.synth->playString("1d#x", synthInstrument::SQUARE_WAVE);
		state.action.playerBullets.push_back(new PlayerBulletSpark(x + w / 2, y + h / 2));

		float tx = x + w / 2;
		float ty = y + h / 2;
		state.action.enemyBullets.push_back(new EnemyBulletShot(tx, ty, tx - 1.0f, ty - 1.0f));
		state.action.enemyBullets.push_back(new EnemyBulletShot(tx, ty, tx, ty - 1.0f));
		state.action.enemyBullets.push_back(new EnemyBulletShot(tx, ty, tx + 1.0f, ty - 1.0f));
		state.action.enemyBullets.push_back(new EnemyBulletShot(tx, ty, tx + 1.0f, ty));
		state.action.enemyBullets.push_back(new EnemyBulletShot(tx, ty, tx + 1.0f, ty + 1.0f));
		state.action.enemyBullets.push_back(new EnemyBulletShot(tx, ty, tx, ty + 1.0f));
		state.action.enemyBullets.push_back(new EnemyBulletShot(tx, ty, tx - 1.0f, ty + 1.0f));
		state.action.enemyBullets.push_back(new EnemyBulletShot(tx, ty, tx - 1.0f, ty));
	}

	int frame = (int)(timer * 20.0f) % 4;

	int& s = state.graphics->spriteCount;
	state.graphics->spritePtr[s].pos = DirectX::XMFLOAT2(x - state.action.cameraX - 2, y - state.action.cameraY - 2);
	state.graphics->spritePtr[s].sprite_color = 0;
	state.graphics->spritePtr[s].sprite_flags = 0;
	state.graphics->spritePtr[s].sprite_index = state.idx_enemyBulletLargeShot + frame;
	s += 1;
}