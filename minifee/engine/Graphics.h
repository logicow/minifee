#pragma once
#include <string>
#include <DirectXMath.h>
#include <vector>

#define MAX_SPRITES 4096

struct SpriteConstant
{
	uint32_t tc_start[2];
	uint32_t tc_size[2];
	DirectX::XMFLOAT2 tc_offset;
	uint32_t padding[2];
};

struct TilemapConstant
{
	uint32_t tc_start[2];
	uint32_t tc_size[2];
};

struct SpriteVertex
{
	DirectX::XMFLOAT2 pos;
	uint32_t sprite_index;
	uint8_t sprite_color;
	uint8_t sprite_flags;
	uint16_t sprite_padding;
};

struct TilemapVertex
{
	DirectX::XMFLOAT2 pos;
	uint32_t tilemap_index;
	uint32_t padding;
};

class Graphics
{
public:
	SpriteVertex *spritePtr;
	uint32_t *palettePtr;
	int spriteCount;
	uint8_t *spriteLookupPtr;
	double frameTime;
	TilemapVertex *tilemapDrawPtr;
	int tilemapDrawCount;
	uint16_t *tilemapTilePtr;
	int tilemapTileLineOffset;

	std::vector<std::vector<uint32_t>> palettes;

	virtual void setWindow(int width, int height, std::string name) = 0;
	virtual bool shouldExit() = 0;
	virtual void swap() = 0;
	virtual void startLoad() = 0;
	virtual void endLoad() = 0;

	virtual int loadSprite(std::string name, bool centered) = 0;
	virtual int loadSpriteRect(std::string name, int x, int y, int width, int height, bool centered) = 0;

	virtual int loadMap(std::string name) = 0;

	virtual void startUpdateSprites() = 0;
	virtual void startUpdateSpriteLookup() = 0;
	virtual void startUpdatePalette() = 0;
	virtual void startUpdateTilemapDraw() = 0;
	virtual void readTilemapTiles(int tilemap, int& width, int& height) = 0;
	virtual void writeTilemapTiles() = 0;
};