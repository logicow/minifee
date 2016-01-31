#pragma once

struct gameState;

class PlayerBullet
{
public:
	bool remove;

	virtual void update(gameState& state) = 0;
};