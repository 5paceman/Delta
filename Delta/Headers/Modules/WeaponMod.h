#pragma once
#include "Module.h"
#include "ESP.h"
#include "imgui.h"

class WeaponMod : public Module {
private:
	uint64_t lastCurrentItem = 0;
	bool noSpread = false;
	bool fullAuto = false;
	bool instaEoka = false;
	bool instaReload = false;
	bool buildExtender = false;
public:
	void Init(const InitEvent& evt);
	void ImguiSetup();
	void OnRender(const RenderEvent& evt);
};