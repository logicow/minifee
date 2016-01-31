#pragma once

#include "PlayerBullet.h"

class PlayerBulletFocus : public PlayerBullet
{
public:
	float x;
	float y;
	float w;
	float h;
	int dir;
	float timer;

	PlayerBulletFocus(float x, float y, int dir);
	virtual void update(gameState& state);
};