#pragma once
#include "Module.h"
#include <thread>
#include "imgui.h"
#include "ESP.h"
#include "Helpers.h"
#include <array>
#include "DrawHelper.h"

class Aimbot : public Module {
private:
	std::thread AimThread;
	uint64_t GameAssemblyBase = 0;
	D3DXMATRIX viewMatrix;
	BoneList boneAim = BoneList::head;
	uint64_t previousPlayer = 0;
	float smoothTime = 1.f;
	float curSmoothTime = 0;
	bool smooth = false;
	int sHeight, sWidth;
public:
	void Aim();
	void Init(const InitEvent& evt);
	void ImguiSetup();
	void OnRender(const RenderEvent& evt);
	int RandomBone();
};