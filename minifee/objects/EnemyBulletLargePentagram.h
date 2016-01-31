#pragma once

#include "EnemyBullet.h"

class EnemyBulletLargePentagram : public EnemyBullet
{
public:
	float x;
	float y;
	float w;
	float h;

	float timer;
	float blinkTimer;
	int type;
	bool open;
	int spawned;

	EnemyBulletLargePentagram(float x, float y, int type);
	virtual void update(gameState& state);
};