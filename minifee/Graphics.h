#pragma once
#include <string>
#include <DirectXMath.h>

struct SpriteVertex
{
	DirectX::XMFLOAT2 pos;
	uint32_t sprite_index;
	uint8_t sprite_color;
	uint8_t sprite_flags;
	uint16_t sprite_padding;
};

class Graphics
{
public:
	SpriteVertex *spritePtr;
	uint32_t *palettePtr;
	int spriteCount;
	uint8_t *spriteLookupPtr;
	double frameTime;

	virtual void setWindow(int width, int height, std::string name) = 0;
	virtual bool shouldExit() = 0;
	virtual void swap() = 0;
	virtual void startLoad() = 0;
	virtual void endLoad() = 0;

	virtual int loadSprite(std::string name, bool centered) = 0;

	virtual void startUpdateSprites() = 0;
	virtual void startUpdateSpriteLookup() = 0;
	virtual void startUpdatePalette() = 0;
};