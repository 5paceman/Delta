#pragma once
#include <string>
#include <d3dx9.h>

struct RenderEvent {
public:
	D3DXMATRIX viewMatrix;
	int screenWidth = 0, screenHeight =0;
	LPDIRECT3DDEVICE9 d3Device;
	LPD3DXFONT d3Font;
};

struct InitEvent {
public:
	uint64_t UnityPlayerBase, GameAssemblyBase;
};

class Module {
public:
	virtual void OnRender(const RenderEvent& evt) = 0;
	virtual void Init(const InitEvent& evt) = 0;
	virtual void ImguiSetup() = 0;
	bool IsEnabled()
	{
		return this->enabled;
	}

	std::wstring GetName()
	{
		return this->name;
	}
protected:
	bool enabled = false;
	std::wstring name;
};