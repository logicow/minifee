#include "winGraphics.h"
#include "dxDescriptions.h"

void winGraphics::loadShadersSprites()
{
	loadVertexShader("sprite_lookup_vs", spriteLookupVertexDesc, 1, &spritesVertexShader, &spritesInputLayout);
	loadPixelShader("sprite_lookup_ps", &spritesPixelShader);
}

void winGraphics::mapSprites()
{
	D3D11_MAPPED_SUBRESOURCE mappedSpritesBuffer;
	ZeroMemory(&mappedSpritesBuffer, sizeof(mappedSpritesBuffer));
	D3DCALL(context->Map(spritesInfoBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSpritesBuffer));
	spritePtr = (SpriteVertex*)mappedSpritesBuffer.pData;
	spriteCount = 0;
}

void winGraphics::callSprites()
{
	if (spritePtr) {
		context->Unmap(spritesInfoBuffer, 0);
		spritePtr = nullptr;
	}
	if (spriteLookupPtr) {
		context->Unmap(spriteLookupTex, 0);
		spriteLookupPtr = nullptr;
	}

	UINT stride = sizeof(DirectX::XMFLOAT2);
	UINT offset = 0;
	ID3D11ShaderResourceView* views[2];
	views[0] = spritesRV;
	views[1] = spriteLookupRV;
	context->PSSetShaderResources(0, 2, views);
	context->OMSetRenderTargets(1, &indexRTV, NULL);
	context->IASetIndexBuffer(spritesIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	context->IASetVertexBuffers(0, 1, &spritesVertexBuffer, &stride, &offset);
	context->IASetInputLayout(spritesInputLayout);
	context->VSSetShader(spritesVertexShader, NULL, 0);
	context->PSSetShader(spritesPixelShader, NULL, 0);
	ID3D11Buffer* buffs[2];
	buffs[0] = spritesConstantBuffer;
	buffs[1] = spritesInfoBuffer;
	context->VSSetConstantBuffers(0, 2, buffs);

	context->DrawIndexed(spriteCount * 6, 0, 0);
}

unsigned long upper_power_of_two(unsigned long v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}

void winGraphics::endLoadSprites()
{
	std::vector<std::vector<unsigned char>> spriteDataTable;
	std::vector<unsigned long> spriteWidthTable;
	std::vector<unsigned long> spriteHeightTable;
	std::vector<unsigned long> spriteMarginLeftTable;
	std::vector<unsigned long> spriteMarginRightTable;
	std::vector<unsigned long> spriteMarginTopTable;
	std::vector<unsigned long> spriteMarginBottomTable;
	std::vector<int> spriteIndexTable;

	std::string previousFile = "";
	std::vector<unsigned char> spriteData;
	unsigned long spriteWidth;
	unsigned long spriteHeight;

	for (int i = 0; i < (int)spritesToLoad.size(); i++) {
		if (spritesToLoad[i] != previousFile) {
			previousFile = spritesToLoad[i];

			std::string path = "sprites/" + spritesToLoad[i] + ".png";

			std::vector<unsigned char> pngData;
			loadFile(pngData, path);

			std::vector<uint32_t> pal;
			pal.resize(256);
			decodePNG(spriteData, spriteWidth, spriteHeight, &pngData[0], (unsigned long)pngData.size(), false, &pal[0]);
			palettes.push_back(pal);
		}
		
		if (spritesToLoadWidth[i] <= 0 || spritesToLoadWidth[i] > (int)spriteWidth - spritesToLoadX[i]) {
			spritesToLoadWidth[i] = spriteWidth - spritesToLoadX[i];
		}
		if (spritesToLoadHeight[i] <= 0 || spritesToLoadHeight[i] > (int)spriteHeight - spritesToLoadY[i]) {
			spritesToLoadHeight[i] = spriteHeight - spritesToLoadY[i];
		}

		unsigned long marginTop;
		unsigned long marginBottom;
		unsigned long marginLeft;
		unsigned long marginRight;

		bool found;
		found = false;
		marginTop = spritesToLoadY[i];
		while (!found && marginTop < spriteHeight) {
			for (unsigned long x = 0; x < spriteWidth; x++) {
				if (spriteData[marginTop * spriteWidth + x] != 0) {
					found = true;
				}
			}
			if (!found) {
				marginTop += 1;
			}
		}

		found = false;
		marginBottom = spriteHeight - spritesToLoadHeight[i] - spritesToLoadY[i];
		while (!found && marginBottom < spriteHeight) {
			for (unsigned long x = 0; x < spriteWidth; x++) {
				if (spriteData[(spriteHeight - marginBottom - 1) * spriteWidth + x] != 0) {
					found = true;
				}
			}
			if (!found) {
				marginBottom += 1;
			}
		}

		found = false;
		marginLeft = spritesToLoadX[i];
		while (!found && marginLeft < spriteWidth) {
			for (unsigned long y = 0; y < spriteHeight; y++) {
				if (spriteData[y * spriteWidth + marginLeft] != 0) {
					found = true;
				}
			}
			if (!found) {
				marginLeft += 1;
			}
		}

		found = false;
		marginRight = spriteWidth - spritesToLoadWidth[i] - spritesToLoadX[i];
		while (!found && marginRight < spriteWidth) {
			for (unsigned long y = 0; y < spriteHeight; y++) {
				if (spriteData[y * spriteWidth + (spriteWidth - marginRight - 1)] != 0) {
					found = true;
				}
			}
			if (!found) {
				marginRight += 1;
			}
		}

		if (marginLeft + marginRight >= spriteWidth || marginTop + marginBottom >= spriteHeight) {
			OutputDebugString("Can't load image, it is all zeroes.");
		}

		spriteDataTable.push_back(spriteData);
		spriteWidthTable.push_back(spriteWidth - marginLeft - marginRight);
		spriteHeightTable.push_back(spriteHeight - marginTop - marginBottom);
		spriteMarginLeftTable.push_back(marginLeft);
		spriteMarginRightTable.push_back(marginRight);
		spriteMarginTopTable.push_back(marginTop);
		spriteMarginBottomTable.push_back(marginBottom);
		spriteIndexTable.push_back(i);
	}

	//sort height
	bool done = false;
	while (!done) {
		done = true;

		for (int i = 0; i < (int)spriteIndexTable.size() - 1; i++) {
			if (spriteHeightTable[spriteIndexTable[i + 1]] > spriteHeightTable[spriteIndexTable[i]]) {
				int it;
				it = spriteIndexTable[i + 1];
				spriteIndexTable[i + 1] = spriteIndexTable[i];
				spriteIndexTable[i] = it;
				done = false;
			}
		}
	}

	std::vector<int> spriteXTable;
	std::vector<int> spriteYTable;
	std::vector<bool> spritePlacedTable;

	for (int i = 0; i < (int)spriteIndexTable.size(); i++) {
		spriteXTable.push_back(0);
		spriteYTable.push_back(0);
		spritePlacedTable.push_back(false);
	}

	int placed = 0;

	std::vector<unsigned long> fillAtX;
	std::vector<unsigned long> fillAtY;
	std::vector<unsigned long> fillWidth;
	std::vector<unsigned long> fillHeight;

	fillAtX.push_back(0);
	fillAtY.push_back(0);
	fillWidth.push_back(1024);
	fillHeight.push_back(16384);

	unsigned long realHeight = 0;

	while (placed < (int)spriteIndexTable.size()) {
		bool fit = false;
		int index = 0;

		int lastFill = fillAtX.size() - 1;

		if (lastFill < 0) {
			OutputDebugString("can't fit all pictures in atlas");
		}

		while (!fit && index < (int)spriteIndexTable.size())
		{
			if (!spritePlacedTable[index] && spriteHeightTable[spriteIndexTable[index]] <= fillHeight[lastFill] && spriteWidthTable[spriteIndexTable[index]] <= fillWidth[lastFill]) {
				fit = true;
				placed += 1;

				spritePlacedTable[index] = true;
				spriteXTable[spriteIndexTable[index]] = fillAtX[lastFill];
				spriteYTable[spriteIndexTable[index]] = fillAtY[lastFill];

				fillAtX.push_back(fillAtX[lastFill] + spriteWidthTable[spriteIndexTable[index]]);
				fillAtY.push_back(fillAtY[lastFill]);
				fillWidth.push_back(fillWidth[lastFill] - spriteWidthTable[spriteIndexTable[index]]);
				fillHeight.push_back(spriteHeightTable[spriteIndexTable[index]]);

				fillAtY[lastFill] += spriteHeightTable[spriteIndexTable[index]];
				fillHeight[lastFill] -= spriteWidthTable[spriteIndexTable[index]];

				if (realHeight < spriteYTable[spriteIndexTable[index]] + spriteHeightTable[spriteIndexTable[index]]) {
					realHeight = spriteYTable[spriteIndexTable[index]] + spriteHeightTable[spriteIndexTable[index]];
				}
			}
			index += 1;
		}

		if (!fit) {
			fillAtX.pop_back();
			fillAtY.pop_back();
			fillWidth.pop_back();
			fillHeight.pop_back();
		}
	}

	realHeight = upper_power_of_two(realHeight);

	std::vector<unsigned char> sprTexData;
	sprTexData.resize((size_t)1024 * realHeight);
	ZeroMemory((void*)&sprTexData[0], 1024 * realHeight);

	for (int i = 0; i < (int)spriteIndexTable.size(); i++) {
		for (int y = 0; y < (int)spriteHeightTable[i]; y++) {
			for (int x = 0; x < (int)spriteWidthTable[i]; x++) {
				int realWidth = spriteWidthTable[i] + spriteMarginLeftTable[i] + spriteMarginRightTable[i];
				sprTexData[(spriteYTable[i] + y) * 1024 + spriteXTable[i] + x] =
					spriteDataTable[i][(y + spriteMarginTopTable[i]) * realWidth + x + spriteMarginLeftTable[i]];
			}
		}
	}

	D3DCALL(device->CreateTexture2D(&getSpritesTexDesc(realHeight), &getDataDesc(sprTexData, 1024), &spritesTex));
	D3DCALL(device->CreateShaderResourceView(spritesTex, &getSpritesRVDesc(), &spritesRV));

	D3DCALL(device->CreateTexture2D(&getSpriteLookupTexDesc(), NULL, &spriteLookupTex));
	D3DCALL(device->CreateShaderResourceView(spriteLookupTex, &getSpriteLookupRVDesc(), &spriteLookupRV));

	std::vector<unsigned char> atlasBufferData;
	atlasBufferData.resize((size_t)1024 * sizeof(SpriteConstant));
	void* bufferDataPtr = (void*)&atlasBufferData[0];

	for (int i = 0; i < (int)spriteIndexTable.size(); i++) {
		((SpriteConstant*)bufferDataPtr)[i].tc_size[0] = (uint32_t)spriteWidthTable[i];
		((SpriteConstant*)bufferDataPtr)[i].tc_size[1] = (uint32_t)spriteHeightTable[i];
		((SpriteConstant*)bufferDataPtr)[i].tc_start[0] = (uint32_t)spriteXTable[i];
		((SpriteConstant*)bufferDataPtr)[i].tc_start[1] = (uint32_t)spriteYTable[i];
		float offsetX = -(float)spritesToLoadX[i];
		float offsetY = -(float)spritesToLoadY[i];
		if (spritesToLoadCentered[i]) {
			float width = (float)(spriteWidthTable[i] + spriteMarginLeftTable[i] + spriteMarginRightTable[i]);
			float height = (float)(spriteHeightTable[i] + spriteMarginTopTable[i] + spriteMarginBottomTable[i]);
			offsetX -= width*0.5f;
			offsetY -= height*0.5f;
		}
		((SpriteConstant*)bufferDataPtr)[i].tc_offset = DirectX::XMFLOAT2(spriteMarginLeftTable[i] + offsetX, spriteMarginTopTable[i] + offsetY);
		((SpriteConstant*)bufferDataPtr)[i].padding[0] = 0;
		((SpriteConstant*)bufferDataPtr)[i].padding[1] = 0;
	}

	D3DCALL(device->CreateBuffer(&getSpritesAtlasBufferDesc(), &getDataDesc(atlasBufferData), &spritesConstantBuffer));

	D3DCALL(device->CreateBuffer(&getSpritesInfoBufferDesc(), NULL, &spritesInfoBuffer));

	std::vector<unsigned char> bufferData;
	getSpritesVertexData(bufferData);
	D3DCALL(device->CreateBuffer(&getSpritesVertexBufferDesc(), &getDataDesc(bufferData), &spritesVertexBuffer));

	getSpritesIndexData(bufferData);
	D3DCALL(device->CreateBuffer(&getSpritesIndexBufferDesc(), &getDataDesc(bufferData), &spritesIndexBuffer));

	spritesToLoad.clear();
	spritesToLoadCentered.clear();
}

void winGraphics::startUpdateSpriteLookup()
{
	if (spriteLookupPtr) {
		return;
	}
	mapLookup();
}

void winGraphics::startUpdateSprites()
{
	if (spritePtr) {
		return;
	}
	mapSprites();
}

int winGraphics::loadSprite(std::string name, bool centered)
{
	int spriteIndex = spritesToLoad.size();
	spritesToLoad.push_back(name);
	spritesToLoadCentered.push_back(centered);
	spritesToLoadX.push_back(0);
	spritesToLoadY.push_back(0);
	spritesToLoadWidth.push_back(-1);
	spritesToLoadHeight.push_back(-1);
	return spriteIndex;
}

int winGraphics::loadSpriteRect(std::string name, int x, int y, int width, int height, bool centered)
{
	int spriteIndex = spritesToLoad.size();
	spritesToLoad.push_back(name);
	spritesToLoadCentered.push_back(centered);
	spritesToLoadX.push_back(x);
	spritesToLoadY.push_back(y);
	spritesToLoadWidth.push_back(width);
	spritesToLoadHeight.push_back(height);
	return spriteIndex;
}
