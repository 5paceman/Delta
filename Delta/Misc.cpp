#include "Misc.h"

uint64_t admintime = 0, instances = 0, itemList = 0, buffer = 0;
TOD_Sky tSky;


void MiscUtils::UpdateTOD()
{
	if (instances == 0)
	{
		instances = Memory::read(GameAssemblyBase + TOD_SKY, { 0xB8, 0x0 });
		itemList = Memory::read<uint64_t>(instances + 0x10);
		buffer = Memory::read<uint64_t>(itemList + 0x20);
		Memory::read(buffer, &tSky, sizeof(tSky));
	}

	while (true)
	{
		if (this->enabled)
		{
			Memory::write(tSky.PtrDayParameters + 0x50, 1.f);
			Sleep(5);
		}
	}
}

void MiscUtils::OnRender(const RenderEvent& evt) {}

void MiscUtils::Init(const InitEvent& evt)
{
	this->GameAssemblyBase = evt.GameAssemblyBase;
	this->FullbrightThread = std::thread(&MiscUtils::UpdateTOD, this);
}

void MiscUtils::ImguiSetup()
{
	ImGui::Begin("Misc", (bool*)0, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Bright Caves", &this->enabled);
	if(ImGui::Button("Admin Flag")) {
		uint64_t newFlags = ESP::localPlayer.PlayerFlags;
		if (this->admin)
			newFlags |= (int)PlayerFlags::IsAdmin;
		else
			newFlags ^= (int)PlayerFlags::IsAdmin;

		Memory::write<uint64_t>(ESP::localPlayer.ptr + 0x5B8, newFlags);
	}
	ImGui::End();
}