#include "winGraphics.h"
#include "dxDescriptions.h"
#include <iostream>
#include <fstream>

winGraphics::winGraphics(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nShow, Config *config)
{
	this->hInstance = hInstance;
	this->hPrevInst = hPrevInst;
	this->lpCmdLine = lpCmdLine;
	this->nShow = nShow;
	this->config = config;

	exitGame = false;
	spritePtr = nullptr;
	palettePtr = nullptr;
	spriteLookupPtr = nullptr;
	
	frameTime = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&intervalFrequency);
}

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return(0);
	}

	return(DefWindowProc(hwnd, msg, wParam, lParam));
}

HWND createAndAdjustWindow(HINSTANCE hInstance, std::string name, int width, int height, bool fullscreen)
{
	RegisterClassEx(&getWindowClassEx(WndProc, hInstance));
	RECT rect = getRect(0, 0, width, height);
	if (!fullscreen) {
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	}
	return CreateWindow("DHCLASS", name.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, NULL);
}

void winGraphics::createWindow(int gameWidth, int gameHeight, std::string name)
{
	int width = 0;
	int height = 0;
	bool fullscreen = false;
	if (config->vars["fullscreen"]) {
		width = (int)config->vars["fullscreen_x"];
		height = (int)config->vars["fullscreen_y"];
		fullscreen = true;
	}
	else {
		int xMax = (int)config->vars["window_x_max"];
		int yMax = (int)config->vars["window_y_max"];
		if (config->vars["window_integer"]) {
			int x_mult = 1;
			while ((x_mult + 1) * gameWidth <= xMax) {
				x_mult += 1;
			}

			int y_mult = 1;
			while ((y_mult + 1) * gameHeight <= yMax) {
				y_mult += 1;
			}

			if (config->vars["window_keep_aspect"]) {
				if (x_mult < y_mult) {
					y_mult = x_mult;
				}
				else if (y_mult < x_mult) {
					x_mult = y_mult;
				}
			}
			width = x_mult * gameWidth;
			height = y_mult * gameHeight;
		}
		else {
			if (config->vars["window_keep_aspect"]) {
				int fullHorizWidth = (int)config->vars["window_x_max"];
				int fullHorizHeight = fullHorizWidth * gameHeight / gameWidth;
				int fullVertHeight = (int)config->vars["window_y_max"];
				int fullVertWidth = fullVertHeight * gameWidth / gameHeight;
				if (fullHorizHeight > fullVertHeight) {
					width = fullVertWidth;
					height = fullVertHeight;
				}
				else {
					width = fullHorizWidth;
					height = fullHorizHeight;
				}
			}
			else {
				width = (int)config->vars["window_x_max"];
				height = (int)config->vars["window_y_max"];
			}
		}
	}
	if (width <= 0) {
		width = gameWidth;
	}
	if (height <= 0) {
		height = gameHeight;
	}

	backBufferWidth = width;
	backBufferHeight = height;
	backBufferFullscreen = fullscreen;

	mainWindow = createAndAdjustWindow(hInstance, name, width, height, fullscreen);
	ShowWindow(mainWindow, nShow);
	UpdateWindow(mainWindow);
}

void winGraphics::createDevice()
{
	D3D_FEATURE_LEVEL reqLevel = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL level;

	DXGI_MODE_DESC bufferDesc = getSwapChainBufferDesc(backBufferWidth, backBufferHeight, getSwapChainRefreshRate());
	DXGI_SWAP_CHAIN_DESC swapChainDesc = getSwapChainDesc(bufferDesc, mainWindow, getSwapChainSampleDesc(), !backBufferFullscreen);

	D3DCALL(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, &reqLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, &level, &context));
}

void winGraphics::createBackbuffer()
{
	D3DCALL(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backTex));
	D3DCALL(device->CreateRenderTargetView(backTex, &getBackbufferRTDesc(), &backRTV));
	D3DCALL(device->CreateDepthStencilState(&getDepthStencilDesc(), &depthStencilStateOff));
}

void winGraphics::createIndexBuffer()
{
	D3DCALL(device->CreateTexture2D(&getIndexTexDesc(backBufferWidth, backBufferHeight), NULL, &indexTex));
	D3DCALL(device->CreateRenderTargetView(indexTex, &getIndexRTVDesc(), &indexRTV));
	D3DCALL(device->CreateShaderResourceView(indexTex, &getIndexRVDesc(), &indexRV));
}

void winGraphics::createPalette()
{
	D3DCALL(device->CreateTexture1D(&getPaletteTexDesc(), NULL, &paletteTex));
	D3DCALL(device->CreateShaderResourceView(paletteTex, &getPaletteRVDesc(), &paletteRV));
}

void winGraphics::createTilemap()
{
	D3DCALL(device->CreateTexture2D(&getTilemapTexDesc(), NULL, &tilemapTex));
	D3DCALL(device->CreateShaderResourceView(tilemapTex, &getTilemapRVDesc(), &tilemapRV));
}

void winGraphics::createTiles()
{
	D3DCALL(device->CreateTexture2D(&getTilesTexDesc(), NULL, &tilesTex));
	D3DCALL(device->CreateShaderResourceView(tilesTex, &getTilesRVDesc(), &tilesRV));
}

void loadFile(std::vector<unsigned char>& buffer, const std::string& filename) //designed for loading files from hard disk in an std::vector
{
	std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
	std::streamsize size = 0;
	if (file.seekg(0, std::ios::end).good()) size = file.tellg();
	if (file.seekg(0, std::ios::beg).good()) size -= file.tellg();

	if (size <= 0) {
		OutputDebugString("file not found");
		buffer.clear();
		return;
	}
	buffer.resize((size_t)size);
	file.read((char*)(&buffer[0]), size);
}

void winGraphics::loadVertexShader(std::string filename, D3D11_INPUT_ELEMENT_DESC* input_element_description, int num_elements, ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout)
{
	std::string path = "../data/shaders/" + filename + ".cso";
	std::vector<unsigned char> fileData;
	loadFile(fileData, path);
	D3DCALL(device->CreateVertexShader(&fileData[0], (SIZE_T)fileData.size(), NULL, vertex_shader));
	D3DCALL(device->CreateInputLayout(input_element_description, num_elements, &fileData[0], (SIZE_T)fileData.size(), input_layout));
}


void winGraphics::loadPixelShader(std::string filename, ID3D11PixelShader** pixel_shader)
{
	std::string path = "../data/shaders/" + filename + ".cso";
	std::vector<unsigned char> fileData;
	loadFile(fileData, path);
	D3DCALL(device->CreatePixelShader(&fileData[0], (SIZE_T)fileData.size(), NULL, pixel_shader));
}

void winGraphics::loadShaders()
{
	loadVertexShader("sprite_lookup_vs", spriteLookupVertexDesc, 1, &spritesVertexShader, &spritesInputLayout);
	loadPixelShader("sprite_lookup_ps", &spritesPixelShader);

	loadVertexShader("indexed_to_rgba_vs", indexedVertexDesc, 1, &indexedVertexShader, &indexedInputLayout);
	loadPixelShader("indexed_to_rgba_ps", &indexedPixelShader);

	loadVertexShader("tilemaps_vs", tilemapsVertexDesc, 4, &tilesVertexShader, &tilesInputLayout);
	loadPixelShader("tilemaps_ps", &tilesPixelShader);
}

void winGraphics::loadBuffers()
{
	std::vector<unsigned char> bufferData;
	getQuadsIndexData(bufferData);
	D3DCALL(device->CreateBuffer(&getQuadsIndexBufferDesc(), &getDataDesc(bufferData), &quadsIndexBuffer));

	getQuadsVertexData(bufferData);
	D3DCALL(device->CreateBuffer(&getQuadsVertexBufferDesc(), &getDataDesc(bufferData), &quadsVertexBuffer));

	getSpritesVertexData(bufferData);
	D3DCALL(device->CreateBuffer(&getSpritesVertexBufferDesc(), &getDataDesc(bufferData), &spritesVertexBuffer));
	
	getSpritesIndexData(bufferData);
	D3DCALL(device->CreateBuffer(&getSpritesIndexBufferDesc(), &getDataDesc(bufferData), &spritesIndexBuffer));
}

void winGraphics::createViewport()
{
	ZeroMemory(&viewport, sizeof(viewport));
	viewport.Width = (float)backBufferWidth;
	viewport.Height = (float)backBufferHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MaxDepth = 1.0f;
	viewport.MinDepth = 0.0f;

	context->ClearState();
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->OMSetDepthStencilState(depthStencilStateOff, 0);
	context->RSSetViewports(1, &viewport);
}


void winGraphics::setWindow(int width, int height, std::string name)
{
	createWindow(width, height, name);
	createDevice();
	createBackbuffer();
	createIndexBuffer();
	createTilemap();
	createTiles();
	loadShaders();
	createViewport();
	createPalette();
}

bool winGraphics::shouldExit()
{
	return exitGame;
}

void winGraphics::mapSprites()
{
	D3D11_MAPPED_SUBRESOURCE mappedSpritesBuffer;
	ZeroMemory(&mappedSpritesBuffer, sizeof(mappedSpritesBuffer));
	D3DCALL(context->Map(spritesInfoBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSpritesBuffer));
	spritePtr = (SpriteVertex*)mappedSpritesBuffer.pData;
	spriteCount = 0;
}

void winGraphics::mapPalette()
{
	D3D11_MAPPED_SUBRESOURCE mapped_palette;
	context->Map(paletteTex, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mapped_palette);
	
	palettePtr = (uint32_t*)mapped_palette.pData;
}

void winGraphics::mapLookup()
{
	D3D11_MAPPED_SUBRESOURCE mapped_lookup;
	context->Map(spriteLookupTex, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mapped_lookup);

	spriteLookupPtr = (uint8_t*)mapped_lookup.pData;
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

void winGraphics::callTiles()
{

}

void winGraphics::callIndexed()
{
	UINT stride = sizeof(DirectX::XMFLOAT2);
	UINT offset = 0;
	
	if (palettePtr) {
		context->Unmap(paletteTex, 0);
		palettePtr = nullptr;
	}

	context->OMSetRenderTargets(1, &backRTV, NULL);
	context->IASetIndexBuffer(quadsIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	context->IASetVertexBuffers(0, 1, &quadsVertexBuffer, &stride, &offset);
	context->IASetInputLayout(indexedInputLayout);
	context->VSSetShader(indexedVertexShader, NULL, 0);
	context->PSSetShader(indexedPixelShader, NULL, 0);
	context->VSSetConstantBuffers(0, 0, NULL);
	ID3D11ShaderResourceView* views[2];
	views[0] = indexRV;
	views[1] = paletteRV;
	context->PSSetShaderResources(0, 2, views);

	context->DrawIndexed(6, 0, 0);
}

void winGraphics::swap()
{
	const float color[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	//context->ClearRenderTargetView(backRTV, color);

	const float c1[1] = { 128.0f };
	context->ClearRenderTargetView(indexRTV, c1);

	callSprites();
	//callTiles();
	callIndexed();

	D3DCALL(swapChain->Present(0, 0));
	getMessages();

	__int64 newIntervalCounter;
	QueryPerformanceCounter((LARGE_INTEGER*)&newIntervalCounter);
	frameTime = (double)(newIntervalCounter - intervalCounter) / intervalFrequency;
	intervalCounter = newIntervalCounter;
}

void winGraphics::startLoad()
{
	spritesToLoad.clear();
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

void winGraphics::endLoad()
{
	std::vector<std::vector<unsigned char>> spriteDataTable;
	std::vector<unsigned long> spriteWidthTable;
	std::vector<unsigned long> spriteHeightTable;
	std::vector<unsigned long> spriteMarginLeftTable;
	std::vector<unsigned long> spriteMarginRightTable;
	std::vector<unsigned long> spriteMarginTopTable;
	std::vector<unsigned long> spriteMarginBottomTable;
	std::vector<int> spriteIndexTable;

	for (int i = 0; i < (int)spritesToLoad.size(); i++)	{
		std::string path = "../data/sprites/" + spritesToLoad[i] + ".png";

		std::vector<unsigned char> pngData;
		loadFile(pngData, path);

		unsigned long spriteWidth;
		unsigned long spriteHeight;

		std::vector<unsigned char> spriteData;
		decodePNG(spriteData, spriteWidth, spriteHeight, &pngData[0], (unsigned long)pngData.size(), false);

		unsigned long marginTop;
		unsigned long marginBottom;
		unsigned long marginLeft;
		unsigned long marginRight;

		bool found;
		found = false;
		marginTop = 0;
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
		marginBottom = 0;
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
		marginLeft = 0;
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
		marginRight = 0;
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
	sprTexData.resize((size_t) 1024 * realHeight);
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
		float offsetX = 0.0f;
		float offsetY = 0.0f;
		if (spritesToLoadCentered.at(i)) {
			float width = (float) (spriteWidthTable[i] + spriteMarginLeftTable[i] + spriteMarginRightTable[i]);
			float height = (float) (spriteHeightTable[i] + spriteMarginTopTable[i] + spriteMarginBottomTable[i]);
			offsetX -= width*0.5f;
			offsetY -= height*0.5f;
		}
		((SpriteConstant*)bufferDataPtr)[i].tc_offset = DirectX::XMFLOAT2(spriteMarginLeftTable[i] + offsetX, spriteMarginTopTable[i] + offsetY);
		((SpriteConstant*)bufferDataPtr)[i].padding[0] = 0;
		((SpriteConstant*)bufferDataPtr)[i].padding[1] = 0;
	}

	D3DCALL(device->CreateBuffer(&getSpritesAtlasBufferDesc(), &getDataDesc(atlasBufferData), &spritesConstantBuffer));

	D3DCALL(device->CreateBuffer(&getSpritesInfoBufferDesc(), NULL, &spritesInfoBuffer));

	spritesToLoad.clear();
	spritesToLoadCentered.clear();

	loadBuffers();

	QueryPerformanceCounter((LARGE_INTEGER*)&intervalCounter);
}

int winGraphics::loadSprite(std::string name, bool centered)
{
	int spriteIndex = spritesToLoad.size();
	spritesToLoad.push_back(name);
	spritesToLoadCentered.push_back(centered);
	return spriteIndex;
}

void winGraphics::getMessages()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT) {
			exitGame = true;
		}
	}
}

void winGraphics::startUpdatePalette()
{
	if (palettePtr) {
		return;
	}
	mapPalette();
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