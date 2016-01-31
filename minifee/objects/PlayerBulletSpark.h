#pragma once

#include "PlayerBullet.h"

class PlayerBulletSpark : public PlayerBullet
{
public:
	float x;
	float y;
	float w;
	float h;

	float timer;

	PlayerBulletSpark(float x, float y);
	virtual void update(gameState& state);
};