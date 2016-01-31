#pragma once
#include <vector>

struct gameState;
class Player;
class PlayerBullet;
class Enemy;
class EnemyBullet;


struct actionState
{
	Player* player;
	std::vector<PlayerBullet*> playerBullets;
	std::vector<Enemy*> enemies;
	std::vector<EnemyBullet*> enemyBullets;

	float cameraX;
	float cameraY;
	float introTimer;
	float fadeTimer;
	int level;
	bool fadingIn;
	bool fadingOut;
	int bandLeft;
	float winTimer;

	int bossKills;
};

void startAction(int level, gameState& state);

void updateAction(gameState& state);