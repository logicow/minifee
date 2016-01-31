#pragma once

#include "Enemy.h"

class EnemyBand : public Enemy
{
public:
	float timer;
	int type;

	EnemyBand(float x, float y, int type);
	virtual void update(gameState& state);
};