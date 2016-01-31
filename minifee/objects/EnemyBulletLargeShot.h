#pragma once

#include "EnemyBullet.h"

class EnemyBulletLargeShot : public EnemyBullet
{
public:
	float x;
	float y;
	float w;
	float h;
	float dirx;
	float diry;
	float speed;

	float timer;

	EnemyBulletLargeShot(float x, float y, float tx, float ty);
	virtual void update(gameState& state);
};