#pragma once
#include "Module.h"
#include "Memory.h"
#include "imgui.h"
#include "Addresses.h"
#include <thread>
#include "Structs.h"
#include "ESP.h"

class MiscUtils : public Module{
private:
	std::thread FullbrightThread;
	uint64_t GameAssemblyBase = 0;
	bool admin = false;
public:
	void UpdateTOD();
	void Init(const InitEvent& evt);
	void ImguiSetup();
	void OnRender(const RenderEvent& evt);
};