#include "winGraphics.h"
#include "dxDescriptions.h"

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
	loadShadersSprites();

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

void winGraphics::endLoad()
{
	endLoadSprites();
	loadBuffers();
	QueryPerformanceCounter((LARGE_INTEGER*)&intervalCounter);
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
