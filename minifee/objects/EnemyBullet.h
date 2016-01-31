#pragma once

struct gameState;

class EnemyBullet
{
public:
	bool remove;

	virtual void update(gameState& state) = 0;
};