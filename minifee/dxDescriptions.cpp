#include "dxDescriptions.h"

WNDCLASSEX getWindowClassEx(WNDPROC WndProc, HINSTANCE hInstance)
{
	WNDCLASSEX wc = {
	sizeof(WNDCLASSEX), CS_VREDRAW | CS_HREDRAW | CS_OWNDC,
		WndProc, 0, 0, hInstance, NULL, NULL, (HBRUSH)(COLOR_WINDOW + 1),
		NULL, "DHCLASS", NULL };
	return wc;
}

RECT getRect(int x, int y, int width, int height)
{
	RECT rect;
	rect.top = y;
	rect.bottom = height;
	rect.left = x;
	rect.right = width;
	return rect;
}

DXGI_RATIONAL getSwapChainRefreshRate()
{
	DXGI_RATIONAL refreshRate;
	refreshRate.Numerator = 60;
	refreshRate.Denominator = 1;
	return refreshRate;
}

DXGI_MODE_DESC getSwapChainBufferDesc(int width, int height, DXGI_RATIONAL refreshRate)
{
	DXGI_MODE_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.Width = width;
	bufferDesc.Height = height;
	bufferDesc.RefreshRate = refreshRate;
	bufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
	return bufferDesc;
}

DXGI_SAMPLE_DESC getSwapChainSampleDesc()
{
	DXGI_SAMPLE_DESC sampleDesc;
	sampleDesc.Count = 1;
	sampleDesc.Quality = 0;
	return sampleDesc;
}

DXGI_SWAP_CHAIN_DESC getSwapChainDesc(DXGI_MODE_DESC bufferDesc, HWND mainWindow, DXGI_SAMPLE_DESC sampleDesc, bool windowed)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.Flags = 0;
	swapChainDesc.OutputWindow = mainWindow;
	swapChainDesc.SampleDesc = sampleDesc;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Windowed = windowed;
	return swapChainDesc;
}

D3D11_RENDER_TARGET_VIEW_DESC getBackbufferRTDesc()
{
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	return renderTargetViewDesc;
}

D3D11_DEPTH_STENCIL_DESC getDepthStencilDesc()
{
	D3D11_DEPTH_STENCIL_DESC ds_desc;
	ZeroMemory(&ds_desc, sizeof(ds_desc));
	ds_desc.DepthEnable = false;
	ds_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	ds_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	ds_desc.StencilEnable = false;
	return ds_desc;
}

D3D11_TEXTURE2D_DESC getIndexTexDesc(int width, int height)
{
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8_UINT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	return texDesc;
}

D3D11_RENDER_TARGET_VIEW_DESC getIndexRTVDesc()
{
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = DXGI_FORMAT_R8_UINT;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	return renderTargetViewDesc;
}

D3D11_TEXTURE2D_DESC getTilesTexDesc()
{
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = 256;
	texDesc.Height = 256;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8_UINT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	return texDesc;
}

D3D11_TEXTURE2D_DESC getTilemapTexDesc()
{
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = 256;
	texDesc.Height = 256;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R16_UINT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	return texDesc;
}

D3D11_TEXTURE1D_DESC getPaletteTexDesc()
{
	D3D11_TEXTURE1D_DESC paletteTexDesc;
	ZeroMemory(&paletteTexDesc, sizeof(paletteTexDesc));
	paletteTexDesc.Width = 256;
	paletteTexDesc.MipLevels = 1;
	paletteTexDesc.ArraySize = 1;
	paletteTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	paletteTexDesc.Usage = D3D11_USAGE_DYNAMIC;
	paletteTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	paletteTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	paletteTexDesc.MiscFlags = 0;
	return paletteTexDesc;
}

D3D11_TEXTURE2D_DESC getSpritesTexDesc(int height)
{
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = 1024;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8_UINT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_IMMUTABLE;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	return texDesc;
}

D3D11_TEXTURE2D_DESC getSpriteLookupTexDesc()
{
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = 256;
	texDesc.Height = 256;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8_UINT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DYNAMIC;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texDesc.MiscFlags = 0;
	return texDesc;
}

D3D11_SHADER_RESOURCE_VIEW_DESC getIndexRVDesc()
{
	D3D11_SHADER_RESOURCE_VIEW_DESC rvDesc;
	rvDesc.Format = DXGI_FORMAT_R8_UINT;
	rvDesc.Texture2D.MipLevels = 1;
	rvDesc.Texture2D.MostDetailedMip = 0;
	rvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	return rvDesc;
}

D3D11_SHADER_RESOURCE_VIEW_DESC getPaletteRVDesc()
{
	D3D11_SHADER_RESOURCE_VIEW_DESC rvDesc;
	rvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rvDesc.Texture1D.MipLevels = 1;
	rvDesc.Texture1D.MostDetailedMip = 0;
	rvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
	return rvDesc;
}

D3D11_SHADER_RESOURCE_VIEW_DESC getTilesRVDesc()
{
	D3D11_SHADER_RESOURCE_VIEW_DESC rvDesc;
	rvDesc.Format = DXGI_FORMAT_R8_UINT;
	rvDesc.Texture2D.MipLevels = 1;
	rvDesc.Texture2D.MostDetailedMip = 0;
	rvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	return rvDesc;
}

D3D11_SHADER_RESOURCE_VIEW_DESC getTilemapRVDesc()
{
	D3D11_SHADER_RESOURCE_VIEW_DESC rvDesc;
	rvDesc.Format = DXGI_FORMAT_R16_UINT;
	rvDesc.Texture2D.MipLevels = 1;
	rvDesc.Texture2D.MostDetailedMip = 0;
	rvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	return rvDesc;
}

D3D11_SHADER_RESOURCE_VIEW_DESC getSpritesRVDesc()
{
	D3D11_SHADER_RESOURCE_VIEW_DESC rvDesc;
	rvDesc.Format = DXGI_FORMAT_R8_UINT;
	rvDesc.Texture2D.MipLevels = 1;
	rvDesc.Texture2D.MostDetailedMip = 0;
	rvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	return rvDesc;
}

D3D11_SHADER_RESOURCE_VIEW_DESC getSpriteLookupRVDesc()
{
	D3D11_SHADER_RESOURCE_VIEW_DESC rvDesc;
	rvDesc.Format = DXGI_FORMAT_R8_UINT;
	rvDesc.Texture2D.MipLevels = 1;
	rvDesc.Texture2D.MostDetailedMip = 0;
	rvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	return rvDesc;
}

D3D11_INPUT_ELEMENT_DESC spriteLookupVertexDesc[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3D11_INPUT_ELEMENT_DESC indexedVertexDesc[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3D11_INPUT_ELEMENT_DESC tilemapsVertexDesc[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R16_UINT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 1, DXGI_FORMAT_R8_UINT, 0, 10, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 2, DXGI_FORMAT_R8_UINT, 0, 11, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3D11_BUFFER_DESC getQuadsIndexBufferDesc()
{
	D3D11_BUFFER_DESC bufferDescription;
	ZeroMemory(&bufferDescription, sizeof(bufferDescription));
	bufferDescription.ByteWidth = 12;
	bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDescription.CPUAccessFlags = 0;
	bufferDescription.MiscFlags = 0;
	return bufferDescription;
}

D3D11_BUFFER_DESC getQuadsVertexBufferDesc()
{
	D3D11_BUFFER_DESC bufferDescription;
	ZeroMemory(&bufferDescription, sizeof(bufferDescription));
	bufferDescription.ByteWidth = sizeof(DirectX::XMFLOAT2) * 4;
	bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDescription.CPUAccessFlags = 0;
	bufferDescription.MiscFlags = 0;
	return bufferDescription;
}

D3D11_BUFFER_DESC getSpritesVertexBufferDesc()
{
	D3D11_BUFFER_DESC bufferDescription;
	ZeroMemory(&bufferDescription, sizeof(bufferDescription));
	bufferDescription.ByteWidth = MAX_SPRITES * sizeof(SpriteVertex);
	bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDescription.CPUAccessFlags = 0;
	bufferDescription.MiscFlags = 0;
	return bufferDescription;
}

D3D11_BUFFER_DESC getSpritesIndexBufferDesc()
{
	D3D11_BUFFER_DESC bufferDescription;
	ZeroMemory(&bufferDescription, sizeof(bufferDescription));
	bufferDescription.ByteWidth = MAX_SPRITES * 6 * 2;
	bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDescription.CPUAccessFlags = 0;
	bufferDescription.MiscFlags = 0;
	return bufferDescription;
}

D3D11_BUFFER_DESC getSpritesAtlasBufferDesc()
{
	D3D11_BUFFER_DESC bufferDescription;
	ZeroMemory(&bufferDescription, sizeof(bufferDescription));
	bufferDescription.ByteWidth = 1024 * sizeof(SpriteConstant);//spriteIndexTable.size() * sizeof(SpriteConstant);
	bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = 0;
	bufferDescription.MiscFlags = 0;
	return bufferDescription;
}

D3D11_BUFFER_DESC getSpritesInfoBufferDesc()
{
	D3D11_BUFFER_DESC bufferDescription;
	ZeroMemory(&bufferDescription, sizeof(bufferDescription));
	bufferDescription.ByteWidth = MAX_SPRITES * sizeof(SpriteVertex);
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDescription.MiscFlags = 0;
	return bufferDescription;
}

D3D11_SUBRESOURCE_DATA getDataDesc(std::vector<unsigned char> &refData, int textureWidth)
{
	D3D11_SUBRESOURCE_DATA bufferDataDesc;
	bufferDataDesc.pSysMem = (void*)&refData[0];
	bufferDataDesc.SysMemPitch = textureWidth == -1 ? refData.size() : textureWidth;
	bufferDataDesc.SysMemSlicePitch = 0;
	return bufferDataDesc;
}

void getQuadsIndexData(std::vector<unsigned char> &buffer)
{
	buffer.resize(sizeof(uint16_t) * 6);

	void* bufferDataPtr = (void*)&buffer[0];
	((uint16_t*)bufferDataPtr)[0] = 0;
	((uint16_t*)bufferDataPtr)[1] = 1;
	((uint16_t*)bufferDataPtr)[2] = 2;
	((uint16_t*)bufferDataPtr)[3] = 1;
	((uint16_t*)bufferDataPtr)[4] = 3;
	((uint16_t*)bufferDataPtr)[5] = 2;
}

void getQuadsVertexData(std::vector<unsigned char> &buffer)
{
	buffer.resize(sizeof(DirectX::XMFLOAT2) * 4);

	void* bufferDataPtr = (void*)&buffer[0];
	((DirectX::XMFLOAT2*)bufferDataPtr)[0] = DirectX::XMFLOAT2(-1.0f, 1.0f);
	((DirectX::XMFLOAT2*)bufferDataPtr)[1] = DirectX::XMFLOAT2(1.0f, 1.0f);
	((DirectX::XMFLOAT2*)bufferDataPtr)[2] = DirectX::XMFLOAT2(-1.0f, -1.0f);
	((DirectX::XMFLOAT2*)bufferDataPtr)[3] = DirectX::XMFLOAT2(1.0f, -1.0f);
}

void getSpritesVertexData(std::vector<unsigned char> &buffer)
{
	buffer.resize((size_t)MAX_SPRITES * sizeof(DirectX::XMFLOAT2) * 4);
	void* bufferDataPtr = (void*)&buffer[0];
	for (int i = 0; i < MAX_SPRITES; i++) {
		((DirectX::XMFLOAT2*)bufferDataPtr)[i * 4 + 0] = DirectX::XMFLOAT2(0.0f, 0.0f);
		((DirectX::XMFLOAT2*)bufferDataPtr)[i * 4 + 1] = DirectX::XMFLOAT2(1.0f, 0.0f);
		((DirectX::XMFLOAT2*)bufferDataPtr)[i * 4 + 2] = DirectX::XMFLOAT2(0.0f, 1.0f);
		((DirectX::XMFLOAT2*)bufferDataPtr)[i * 4 + 3] = DirectX::XMFLOAT2(1.0f, 1.0f);
	}
}

void getSpritesIndexData(std::vector<unsigned char> &buffer)
{
	buffer.resize((size_t)MAX_SPRITES * sizeof(uint16_t) * 6);
	void* bufferDataPtr = (void*)&buffer[0];
	for (int i = 0; i < MAX_SPRITES; i++) {
		((uint16_t*)bufferDataPtr)[i * 6 + 0] = i * 4 + 0;
		((uint16_t*)bufferDataPtr)[i * 6 + 1] = i * 4 + 1;
		((uint16_t*)bufferDataPtr)[i * 6 + 2] = i * 4 + 2;
		((uint16_t*)bufferDataPtr)[i * 6 + 3] = i * 4 + 1;
		((uint16_t*)bufferDataPtr)[i * 6 + 4] = i * 4 + 3;
		((uint16_t*)bufferDataPtr)[i * 6 + 5] = i * 4 + 2;
	}
}