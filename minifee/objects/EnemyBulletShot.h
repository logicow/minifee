#pragma once

#include "EnemyBullet.h"

class EnemyBulletShot : public EnemyBullet
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

	EnemyBulletShot(float x, float y, float tx, float ty);
	virtual void update(gameState& state);
};