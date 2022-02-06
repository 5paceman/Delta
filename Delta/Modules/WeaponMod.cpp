#include "WeaponMod.h"

void WeaponMod::Init(const InitEvent& evt) {}

void WeaponMod::ImguiSetup()
{
	ImGui::Begin("Weapon Mods");
	ImGui::Checkbox("No Recoil", &this->enabled);
	ImGui::Checkbox("No Spread", &this->noSpread);
    ImGui::Checkbox("Full Automatic", &this->fullAuto);
    ImGui::Checkbox("Instant Reload", &this->instaReload);
    ImGui::Checkbox("Instant Eoka", &this->instaEoka);
    ImGui::Checkbox("Build Distance", &this->buildExtender);
	ImGui::End();
}

void WeaponMod::OnRender(const RenderEvent& evt)
{
	if (this->lastCurrentItem != ESP::localPlayer.clActiveItem)
	{
        Item item = ESP::localPlayer.GetActiveItem();
        int itemid = Memory::read<int>(item.PtrItemDefinition + 0x40);
        if (item.GetItemCategory() == ItemCategory::Weapon)
        {
            BaseProjectile bProj;
            Memory::read(item.PtrHeldEntity, &bProj, sizeof(bProj));
            if (this->enabled)
            {
                Memory::writef(bProj.PtrRecoilProperties + 0x18, 0.0f);
                Memory::writef(bProj.PtrRecoilProperties + 0x1C, 0.0f);
                Memory::writef(bProj.PtrRecoilProperties + 0x20, 0.0f);
                Memory::writef(bProj.PtrRecoilProperties + 0x24, 0.0f); 
                Memory::write<bool>(item.PtrHeldEntity + 0x330, true);
            }
            if (this->noSpread)
            {
                Memory::writef(item.PtrHeldEntity + 0x2D0, -0.0f);
                Memory::writef(item.PtrHeldEntity + 0x2D4, -3.0f);
                Memory::writef(item.PtrHeldEntity + 0x2D8, -0.0f);
            }
            if (this->instaEoka && itemid == -75944661 /* Eoka Pistol  */)
            {
                Memory::writef(item.PtrHeldEntity + 0x340, 1.0f);
            }
            if (this->instaReload)
            {
                Memory::write<bool>(item.PtrHeldEntity + 0x2A8, true);
            }
            if (this->fullAuto)
            {
                Memory::write<bool>(item.PtrHeldEntity + 0x270, true);
            }
            if (this->buildExtender && itemid == 1525520776)
            {
                uint64_t Construction = Memory::read<uint64_t>(item.PtrHeldEntity + 0x1E8);
                Memory::write<float>(Construction + 0xD4, 180.f);
            }
        }
        this->lastCurrentItem = ESP::localPlayer.clActiveItem;
	}
}