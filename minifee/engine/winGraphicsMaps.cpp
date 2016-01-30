#include "winGraphics.h"
#include "dxDescriptions.h"
#include "RapidXML\rapidxml.hpp"

void winGraphics::loadShadersMaps()
{
	loadVertexShader("tilemaps_vs", tilemapsVertexDesc, 1, &tilesVertexShader, &tilesInputLayout);
	loadPixelShader("tilemaps_ps", &tilesPixelShader);
}

int winGraphics::loadMap(std::string name)
{
	mapsToLoad.push_back(name);

	int initialCount = tilemapLoadCount;

	std::string path = "../data/maps/" + name + ".tmx";

	std::vector<unsigned char> tmxData;
	loadFile(tmxData, path);
	tmxData.push_back(0);

	rapidxml::xml_document<> doc;
	doc.parse<0>((char*)&tmxData[0]);

	int layers = 0;
	rapidxml::xml_node<>* layerNode = doc.first_node("map")->first_node("layer");
	while (layerNode) {
		layers += 1;
		layerNode = layerNode->next_sibling("layer");
	}

	tilemapLoadCount += layers;

	return initialCount;
}

extern unsigned long upper_power_of_two(unsigned long v);

void winGraphics::endLoadMaps()
{
	std::vector<std::string> tilesetFilename;
	std::vector<int> tilesetStart;
	std::vector<int> tilesetSize;

	int tileWidth = 1;
	int tileHeight = 1;

	std::vector<std::vector<uint16_t>> tilemapTiles;
	std::vector<int> tilemapIndexTable;

	for (int i = 0; i < (int)mapsToLoad.size(); i++) {

		std::string path = "../data/maps/" + mapsToLoad[i] + ".tmx";

		std::vector<unsigned char> tmxData;
		loadFile(tmxData, path);
		tmxData.push_back(0);

		rapidxml::xml_document<> doc;
		doc.parse<0>((char*)&tmxData[0]);
		
		std::string tileset = doc.first_node("map")->first_node("tileset")->first_node("image")->first_attribute("source")->value();
		
		int tileStart;

		bool found = false;
		for (int t = 0; t < (int)tilesetFilename.size(); t++) {
			if (tilesetFilename[t] == tileset) {
				found = true;
				tileStart = tilesetStart[t];
			}
		}

		if (!found) {
			tileWidth = atoi(doc.first_node("map")->first_node("tileset")->first_attribute("tilewidth")->value());
			tileHeight = atoi(doc.first_node("map")->first_node("tileset")->first_attribute("tileheight")->value());
			
			int tileCount = atoi(doc.first_node("map")->first_node("tileset")->first_attribute("tilecount")->value());
			
			if (tilesetStart.size() == 0) {
				tileStart = 1;
			}
			else {
				tileStart = tilesetStart[tilesetStart.size() - 1] + tilesetSize[tilesetSize.size() - 1];
			}
			
			tilesetFilename.push_back(tileset);
			tilesetStart.push_back(tileStart);
			tilesetSize.push_back(tileCount);
		}

		int layers = 0;
		rapidxml::xml_node<>* layerNode = doc.first_node("map")->first_node("layer");
		while (layerNode) {
			layers += 1;

			int width = atoi(layerNode->first_attribute("width")->value());
			int height = atoi(layerNode->first_attribute("height")->value());

			rapidxml::xml_node<> *tileNode = layerNode->first_node("data")->first_node("tile");

			std::vector<uint16_t> layerTiles;

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					int tile = tileNode ? atoi(tileNode->first_attribute("gid")->value()) : 0;
					uint16_t utile = tile <= 0 ? 0 : tile + tileStart - 1;
					layerTiles.push_back(utile);

					tileNode = tileNode->next_sibling("tile");
				}
			}

			tilemapWidth.push_back(width);
			tilemapHeight.push_back(height);
			tilemapTiles.push_back(layerTiles);
			tilemapIndexTable.push_back(tilemapIndexTable.size());

			layerNode = layerNode->next_sibling("layer");
		}
	}

	//sort height
	bool done = false;
	while (!done) {
		done = true;

		for (int i = 0; i < (int)tilemapIndexTable.size() - 1; i++) {
			if (tilemapHeight[tilemapIndexTable[i + 1]] > tilemapHeight[tilemapIndexTable[i]]) {
				int it;
				it = tilemapHeight[i + 1];
				tilemapIndexTable[i + 1] = tilemapIndexTable[i];
				tilemapIndexTable[i] = it;
				done = false;
			}
		}
	}

	std::vector<bool> atlasPlacedTable;

	for (int i = 0; i < (int)tilemapIndexTable.size(); i++) {
		tilemapAtlasXTable.push_back(0);
		tilemapAtlasYTable.push_back(0);
		atlasPlacedTable.push_back(false);
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

	while (placed < (int)tilemapIndexTable.size()) {
		bool fit = false;
		int index = 0;

		int lastFill = fillAtX.size() - 1;

		if (lastFill < 0) {
			OutputDebugString("can't fit all pictures in atlas");
		}

		while (!fit && index < (int)tilemapIndexTable.size())
		{
			if (!atlasPlacedTable[index] && tilemapHeight[tilemapIndexTable[index]] <= fillHeight[lastFill] && tilemapWidth[tilemapIndexTable[index]] <= fillWidth[lastFill]) {
				fit = true;
				placed += 1;

				atlasPlacedTable[index] = true;
				tilemapAtlasXTable[tilemapIndexTable[index]] = fillAtX[lastFill];
				tilemapAtlasYTable[tilemapIndexTable[index]] = fillAtY[lastFill];

				fillAtX.push_back(fillAtX[lastFill] + tilemapWidth[tilemapIndexTable[index]]);
				fillAtY.push_back(fillAtY[lastFill]);
				fillWidth.push_back(fillWidth[lastFill] - tilemapWidth[tilemapIndexTable[index]]);
				fillHeight.push_back(tilemapHeight[tilemapIndexTable[index]]);

				fillAtY[lastFill] += tilemapHeight[tilemapIndexTable[index]];
				fillHeight[lastFill] -= tilemapWidth[tilemapIndexTable[index]];

				if (realHeight < tilemapAtlasYTable[tilemapIndexTable[index]] + tilemapHeight[tilemapIndexTable[index]]) {
					realHeight = tilemapAtlasYTable[tilemapIndexTable[index]] + tilemapHeight[tilemapIndexTable[index]];
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
	if (realHeight == 0) {
		realHeight = 1;
	}

	std::vector<uint16_t> tilemapTexData;
	tilemapTexData.resize((size_t)1024 * realHeight * sizeof(uint16_t));
	ZeroMemory((void*)&tilemapTexData[0], 1024 * realHeight * sizeof(uint16_t));

	for (int i = 0; i < (int)tilemapIndexTable.size(); i++) {
		for (int y = 0; y < (int)tilemapHeight[i]; y++) {
			for (int x = 0; x < (int)tilemapWidth[i]; x++) {
				tilemapTexData[(tilemapAtlasYTable[i] + y) * 1024 + tilemapAtlasXTable[i] + x] =
					tilemapTiles[i][y * tilemapWidth[i] + x];
			}
		}
	}

	int tileCount;
	if (tilesetStart.size() == 0) {
		tileCount = 1;
	}
	else {
		tileCount = tilesetStart[tilesetStart.size() - 1] + tilesetSize[tilesetSize.size() - 1];
	}

	int tileTexWidth;
	if (tileCount < 256) {
		tileTexWidth = upper_power_of_two(tileCount * tileWidth);
	}
	else {
		tileTexWidth = upper_power_of_two(256 * tileWidth);
	}
	int tileTexHeight = upper_power_of_two(((tileCount - 1) / 256 + 1) * tileHeight);

	std::vector<unsigned char> tilesTexData;
	tilesTexData.resize((size_t)tileTexWidth * tileTexHeight);
	ZeroMemory((void*)&tilesTexData[0], tileTexWidth * tileTexHeight);

	for (int i = 0; i < tilesetFilename.size(); i++) {

		std::string path = "../data/maps/" + tilesetFilename[i];

		std::vector<unsigned char> pngData;
		loadFile(pngData, path);

		std::vector<unsigned char> tilemapData;
		unsigned long tilemapWidth;
		unsigned long tilemapHeight;

		std::vector<uint32_t> pal;
		pal.resize(256);
		decodePNG(tilemapData, tilemapWidth, tilemapHeight, &pngData[0], (unsigned long)pngData.size(), false, &pal[0]);
		palettes.push_back(pal);

		int srcCols = tilemapWidth / tileWidth;
		int srcRows = tilemapHeight / tileHeight;
		int destAdd = tilesetStart[i];

		for (int t = 0; t < tilesetSize[i]; t++) {
			for (int y = 0; y < tileHeight; y++) {
				for (int x = 0; x < tileWidth; x++) {
					int srcRow = t / srcCols;
					int srcCol = t % srcCols;
					int d = t + destAdd;
					tilesTexData[    ((d / 256)*tileHeight + y) * tileTexWidth + x + (d % 256) * tileWidth] = 
						tilemapData[ (y + srcRow*tileHeight)    * tilemapWidth + x + srcCol    * tileWidth];
				}
			}
		}
	}

	D3DCALL(device->CreateTexture2D(&getTilemapTexDesc(1024, realHeight), &getDataDesc16(tilemapTexData, 1024), &tilemapTex));
	D3DCALL(device->CreateShaderResourceView(tilemapTex, &getTilemapRVDesc(), &tilemapRV));

	D3DCALL(device->CreateTexture2D(&getTilesTexDesc(tileTexWidth, tileTexHeight), &getDataDesc(tilesTexData, tileTexWidth), &tilesTex));
	D3DCALL(device->CreateShaderResourceView(tilesTex, &getTilesRVDesc(), &tilesRV));

	std::vector<unsigned char> atlasBufferData;
	atlasBufferData.resize((size_t)1024 * sizeof(TilemapConstant));
	void* bufferDataPtr = (void*)&atlasBufferData[0];

	for (int i = 0; i < (int)tilemapIndexTable.size(); i++) {
		((TilemapConstant*)bufferDataPtr)[i].tc_size[0] = (uint32_t)tilemapWidth[i];
		((TilemapConstant*)bufferDataPtr)[i].tc_size[1] = (uint32_t)tilemapHeight[i];
		((TilemapConstant*)bufferDataPtr)[i].tc_start[0] = (uint32_t)tilemapAtlasXTable[i];
		((TilemapConstant*)bufferDataPtr)[i].tc_start[1] = (uint32_t)tilemapAtlasYTable[i];
	}

	D3DCALL(device->CreateBuffer(&getTilemapAtlasBufferDesc(), &getDataDesc(atlasBufferData), &tilemapsConstantBuffer));

	D3DCALL(device->CreateBuffer(&getTilemapInfoBufferDesc(), NULL, &tilemapsInfoBuffer));

	std::vector<unsigned char> bufferData;
	getTilemapVertexData(bufferData);
	D3DCALL(device->CreateBuffer(&getTilemapVertexBufferDesc(), &getDataDesc(bufferData), &tilemapsVertexBuffer));

	getTilemapIndexData(bufferData);
	D3DCALL(device->CreateBuffer(&getTilemapIndexBufferDesc(), &getDataDesc(bufferData), &tilemapsIndexBuffer));

	D3DCALL(device->CreateTexture2D(&getCopyTexDesc(1024, realHeight), nullptr, &copyTex));

	mapsToLoad.clear();
}

void winGraphics::callTiles()
{
	if (tilemapDrawPtr) {
		context->Unmap(tilemapsInfoBuffer, 0);
		tilemapDrawPtr = nullptr;
	}

	if (tilemapTilePtr) {
		context->Unmap(copyTex, 0);
		D3D11_BOX region;
		region.back = 1;
		region.front = 0;
		region.left = tilemapAtlasXTable[mappedTilemap];
		region.right = tilemapAtlasXTable[mappedTilemap] + tilemapWidth[mappedTilemap];
		region.top = tilemapAtlasYTable[mappedTilemap];
		region.bottom = tilemapAtlasYTable[mappedTilemap] + tilemapHeight[mappedTilemap];
		context->CopySubresourceRegion(tilemapTex, 0, region.left, region.top, 0, copyTex, 0, &region);

		tilemapDrawPtr = nullptr;
	}

	UINT stride = sizeof(DirectX::XMFLOAT2);
	UINT offset = 0;
	ID3D11ShaderResourceView* views[2];
	views[0] = tilesRV;
	views[1] = tilemapRV;
	context->PSSetShaderResources(0, 2, views);
	context->OMSetRenderTargets(1, &indexRTV, NULL);
	context->IASetIndexBuffer(tilemapsIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	context->IASetVertexBuffers(0, 1, &tilemapsVertexBuffer, &stride, &offset);
	context->IASetInputLayout(tilesInputLayout);
	context->VSSetShader(tilesVertexShader, NULL, 0);
	context->PSSetShader(tilesPixelShader, NULL, 0);
	ID3D11Buffer* buffs[2];
	buffs[0] = tilemapsConstantBuffer;
	buffs[1] = tilemapsInfoBuffer;
	context->VSSetConstantBuffers(0, 2, buffs);

	context->DrawIndexed(tilemapDrawCount * 6, 0, 0);
}

void winGraphics::mapTilemaps()
{
	D3D11_MAPPED_SUBRESOURCE mappedTilemapsBuffer;
	ZeroMemory(&mappedTilemapsBuffer, sizeof(mappedTilemapsBuffer));
	D3DCALL(context->Map(tilemapsInfoBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedTilemapsBuffer));
	tilemapDrawPtr = (TilemapVertex*)mappedTilemapsBuffer.pData;
	tilemapDrawCount = 0;
}

void winGraphics::startUpdateTilemapDraw()
{
	if (tilemapDrawPtr) {
		return;
	}
	mapTilemaps();
}

void winGraphics::mapTilemapsTex(int x, int y, int w, int h)
{
	D3D11_BOX region;
	region.back = 1;
	region.front = 0;
	region.left = x;
	region.right = x + w;
	region.top = y;
	region.bottom = y + h;
	context->CopySubresourceRegion(copyTex, 0, x, y, 0, tilemapTex, 0, &region);

	D3D11_MAPPED_SUBRESOURCE mappedTilemapTexBuffer;
	ZeroMemory(&mappedTilemapTexBuffer, sizeof(mappedTilemapTexBuffer));
	D3DCALL(context->Map(copyTex, 0, D3D11_MAP_READ_WRITE, 0, &mappedTilemapTexBuffer));
	tilemapTilePtr = (uint16_t*)mappedTilemapTexBuffer.pData;
}

void winGraphics::startUpdateTilemapTiles(int tilemap, int& width, int& height)
{
	width = tilemapWidth[tilemap];
	height = tilemapHeight[tilemap];

	if (tilemapTilePtr) {
		context->Unmap(copyTex, 0);
		D3D11_BOX region;
		region.back = 1;
		region.front = 0;
		region.left = tilemapAtlasXTable[mappedTilemap];
		region.right = tilemapAtlasXTable[mappedTilemap] + tilemapWidth[mappedTilemap];
		region.top = tilemapAtlasYTable[mappedTilemap];
		region.bottom = tilemapAtlasYTable[mappedTilemap] + tilemapHeight[mappedTilemap];
		context->CopySubresourceRegion(tilemapTex, 0, region.left, region.top, 0, copyTex, 0, &region);

		tilemapDrawPtr = nullptr;
	}
	mapTilemapsTex(tilemapAtlasXTable[tilemap], tilemapAtlasYTable[tilemap], width, height);
	mappedTilemap = tilemap;
	tilemapTileLineOffset = 1024;
}
