#pragma once

#include "Graphics.h"
#include "Windows.h"
#include "d3d11.h"
#include <dxgi1_2.h>
#include <string>
#include <vector>
#include <DirectXMath.h>
#include "Config.h"

#pragma comment (lib, "d3d11.lib")

#define D3DCALL(x) if(x != S_OK) { std::string s = "D3D call fail at line " + std::to_string(__LINE__) + " in file " + std::string(__FILE__) + std::string("\n"); OutputDebugString(s.c_str()); }

int decodePNG(std::vector<unsigned char>& out_image, unsigned long& image_width, unsigned long& image_height, const unsigned char* in_png, size_t in_size, bool convert_to_rgba32 = true);

class winGraphics : Graphics
{
public:
	winGraphics(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nShow, Config *config);
	~winGraphics();
	virtual void setWindow(int width, int height, std::string name);
	virtual bool shouldExit();
	virtual void swap();
	virtual void startLoad();
	virtual void endLoad();
	virtual int loadSprite(std::string name, bool centered);
	virtual int loadSpriteRect(std::string name, int x, int y, int width, int height, bool centered);

	virtual int loadMap(std::string name);

	virtual void startUpdateSprites();
	virtual void startUpdateSpriteLookup();
	virtual void startUpdatePalette();
	virtual void startUpdateTilemaps();
private:
	HINSTANCE hInstance;
	HINSTANCE hPrevInst;
	LPSTR lpCmdLine;
	int nShow;
	Config *config;

	int backBufferWidth;
	int backBufferHeight;
	bool backBufferFullscreen;

	HWND mainWindow;

	ID3D11Device* device;
	ID3D11DeviceContext* context;
	IDXGISwapChain* swapChain;
	ID3D11Texture2D* backTex;
	ID3D11Texture2D* indexTex;
	ID3D11Texture2D* tilesTex;
	ID3D11Texture2D* tilemapTex;
	ID3D11Texture2D* spritesTex;
	ID3D11Texture2D* spriteLookupTex;
	ID3D11Texture1D* paletteTex;
	ID3D11RenderTargetView* backRTV;
	ID3D11RenderTargetView* indexRTV;
	ID3D11ShaderResourceView* indexRV;
	ID3D11ShaderResourceView* paletteRV;
	ID3D11ShaderResourceView* tilesRV;
	ID3D11ShaderResourceView* tilemapRV;
	ID3D11ShaderResourceView* spritesRV;
	ID3D11ShaderResourceView* spriteLookupRV;

	D3D11_VIEWPORT viewport;

	ID3D11DepthStencilState* depthStencilStateOff;

	ID3D11Buffer* quadsIndexBuffer;
	ID3D11Buffer* quadsVertexBuffer;

	ID3D11Buffer* spritesVertexBuffer;
	ID3D11Buffer* spritesIndexBuffer;
	ID3D11Buffer* spritesInfoBuffer;
	ID3D11Buffer* spritesConstantBuffer;

	ID3D11Buffer* tilemapsVertexBuffer;
	ID3D11Buffer* tilemapsIndexBuffer;
	ID3D11Buffer* tilemapsInfoBuffer;
	ID3D11Buffer* tilemapsConstantBuffer;

	ID3D11VertexShader* spritesVertexShader;
	ID3D11InputLayout* spritesInputLayout;
	ID3D11PixelShader* spritesPixelShader;
	
	ID3D11VertexShader* tilesVertexShader;
	ID3D11InputLayout* tilesInputLayout;
	ID3D11PixelShader* tilesPixelShader;

	ID3D11VertexShader* indexedVertexShader;
	ID3D11InputLayout* indexedInputLayout;
	ID3D11PixelShader* indexedPixelShader;

	__int64 intervalFrequency;
	__int64 intervalCounter;

	bool exitGame;

	void createWindow(int gameWidth, int gameHeight, std::string name);
	void createDevice();
	void createBackbuffer();
	void createIndexBuffer();
	void createViewport();
	void createPalette();
	
	void loadVertexShader(std::string filename, D3D11_INPUT_ELEMENT_DESC* input_element_description, int num_elements, ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout);
	void loadPixelShader(std::string filename, ID3D11PixelShader** pixel_shader);
	void loadShaders();
	void loadShadersSprites();
	void loadShadersMaps();
	void loadBuffers();
	void callSprites();
	void callTiles();
	void callIndexed();
	void getMessages();
	void mapSprites();
	void mapPalette();
	void mapLookup();
	void mapTilemaps();
	void endLoadSprites();

	void endLoadMaps();

	std::vector<std::string> spritesToLoad;
	std::vector<bool> spritesToLoadCentered;
	std::vector<int> spritesToLoadX;
	std::vector<int> spritesToLoadY;
	std::vector<int> spritesToLoadWidth;
	std::vector<int> spritesToLoadHeight;

	std::vector<std::string> mapsToLoad;
	int tilemapLoadCount;
};