#pragma once
#include "graphicsTypes.h"
#include "Windows.h"
#include "d3d11.h"
#include <dxgi1_2.h>
#include <string>
#include <vector>
#include <DirectXMath.h>

WNDCLASSEX getWindowClassEx(WNDPROC WndProc, HINSTANCE hInstance);
RECT getRect(int x, int y, int width, int height);
DXGI_RATIONAL getSwapChainRefreshRate();
DXGI_MODE_DESC getSwapChainBufferDesc(int width, int height, DXGI_RATIONAL refreshRate);
DXGI_SAMPLE_DESC getSwapChainSampleDesc();
DXGI_SWAP_CHAIN_DESC getSwapChainDesc(DXGI_MODE_DESC bufferDesc, HWND mainWindow, DXGI_SAMPLE_DESC sampleDesc, bool windowed);
D3D11_RENDER_TARGET_VIEW_DESC getBackbufferRTDesc();
D3D11_DEPTH_STENCIL_DESC getDepthStencilDesc();
D3D11_TEXTURE2D_DESC getIndexTexDesc(int width, int height);
D3D11_RENDER_TARGET_VIEW_DESC getIndexRTVDesc();
D3D11_TEXTURE2D_DESC getTilesTexDesc();
D3D11_TEXTURE2D_DESC getTilemapTexDesc();
D3D11_TEXTURE1D_DESC getPaletteTexDesc();
D3D11_TEXTURE2D_DESC getSpritesTexDesc(int height);
D3D11_TEXTURE2D_DESC getSpriteLookupTexDesc();
D3D11_SHADER_RESOURCE_VIEW_DESC getIndexRVDesc();
D3D11_SHADER_RESOURCE_VIEW_DESC getPaletteRVDesc();
D3D11_SHADER_RESOURCE_VIEW_DESC getTilesRVDesc();
D3D11_SHADER_RESOURCE_VIEW_DESC getTilemapRVDesc();
D3D11_SHADER_RESOURCE_VIEW_DESC getSpritesRVDesc();
D3D11_SHADER_RESOURCE_VIEW_DESC getSpriteLookupRVDesc();
D3D11_INPUT_ELEMENT_DESC spriteLookupVertexDesc[];
D3D11_INPUT_ELEMENT_DESC indexedVertexDesc[];
D3D11_INPUT_ELEMENT_DESC tilemapsVertexDesc[];
D3D11_BUFFER_DESC getQuadsIndexBufferDesc();
D3D11_BUFFER_DESC getQuadsVertexBufferDesc();
D3D11_BUFFER_DESC getSpritesVertexBufferDesc();
D3D11_BUFFER_DESC getSpritesIndexBufferDesc();
D3D11_BUFFER_DESC getSpritesAtlasBufferDesc();
D3D11_BUFFER_DESC getSpritesInfoBufferDesc();
D3D11_SUBRESOURCE_DATA getDataDesc(std::vector<unsigned char> &refData, int textureWidth = -1);
void getQuadsIndexData(std::vector<unsigned char> &buffer);
void getQuadsVertexData(std::vector<unsigned char> &buffer);
void getSpritesVertexData(std::vector<unsigned char> &buffer);
void getSpritesIndexData(std::vector<unsigned char> &buffer);