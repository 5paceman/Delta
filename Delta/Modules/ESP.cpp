#include "ESP.h"
#include "imgui.h"

LocalPlayer ESP::localPlayer;
std::map<uint64_t, std::unique_ptr<Player>> ESP::players;
uint64_t ESP::closestPlayer;

void ESP::GetNetworkables()
{
    while (!this->shouldEnd)
    {
        if (values == 0)
            values = Memory::read(GameAssemblyBase + BASE_NETWORKABLE, { 0xB8, 0x0, 0x10,  0x28 });

        int size = Memory::read<int>(values + 0x10);

        if (Buffer == 0)
            Buffer = Memory::read<uint64_t>(values + 0x18);
        for (int i = 0; i < size; i++)
        {
            uint64_t entityPtr = Memory::read<uint64_t>(Buffer + 0x20 + (i * 0x8));
            uint64_t prefabId = Memory::read<uint64_t>(entityPtr + 0x48);
            switch (prefabId)
            {
            case 0xf4e1d514:
                AddPlayer(entityPtr);
                break;
            case 0x991d432c:
            {
                int state = Memory::read<int>(entityPtr + 0x120);
                if (state != 0) {
                    AddSO(entityPtr, L"Small Stash", ObjectType::SmallStash, 0);
                }
            }
            break;
            case 0x1d884996c:
                AddSO(entityPtr, L"Supply Drop", ObjectType::SupplyDrop, 0);
                break;
            case 0x93a391ec:
                AddSO(entityPtr, L"Tool Cupboard", ObjectType::ToolCupboard, 0);
                break;
            case 0x8386c121:
                AddSO(entityPtr, L"Large Box", ObjectType::LargeBox, 0);
                break;
            case 0x1b49137a5:
                AddSO(entityPtr, L"Patrol Helicopter", ObjectType::PatrolHelicopter, 22);
                break;
            case 0x9b3e0c8f:
                String playerName;
                Memory::read(entityPtr + 0x290, &playerName, sizeof(playerName));
                std::wstring label = playerName.string;
                label += L"'s Corpse";
                AddSO(entityPtr, label.c_str(), ObjectType::Corpse, 0);
                break;
            }
        }
        Sleep(16);
    }
}

void ESP::AddSO(uint64_t entityPtr, std::wstring name, ObjectType type, int boneId)
{
    if (interestedObjects.count(entityPtr))
    {
        uint64_t transform = GetBoneTransform(entityPtr, boneId);
        D3DXVECTOR3 position = GetPosition(transform);
        interestedObjects.find(entityPtr)->second->position = position;
        interestedObjects.find(entityPtr)->second->lastRead = GetTickCount();
    }
    else {
        std::unique_ptr<InterestedObject> object = std::make_unique<InterestedObject>();
        uint64_t transform = GetBoneTransform(entityPtr, boneId);
        D3DXVECTOR3 position = GetPosition(transform);
        object->position = position;
        object->name = name;
        object->lastRead = GetTickCount();
        object->objType = type;
        interestedObjects.insert(std::make_pair(entityPtr, std::move(object)));
    }
}

void ESP::AddPlayer(uint64_t playerPtr)
{
    BasePlayer basePlayer;
    Memory::read(playerPtr, &basePlayer, sizeof(basePlayer));


    PlayerModel pModel;
    Memory::read(basePlayer.PtrPlayerModel, &pModel, sizeof(pModel));


    if (pModel.isLocalPlayer)
    {
        localPlayer.pos = pModel.Position;
        localPlayer.clActiveItem = basePlayer.clActiveItem;
        localPlayer.health = basePlayer.health;
        localPlayer.maxHealth = basePlayer.maxHealth;
        localPlayer.startHealth = basePlayer.startHealth;
        localPlayer.PtrPlayerInventory = basePlayer.PtrPlayerInventory;
        localPlayer.ptr = playerPtr;
        localPlayer.PlayerFlags = basePlayer.PlayerFlags;

        Item item = localPlayer.GetActiveItem();
        ItemDefinition itemDef;
        Memory::read(item.PtrItemDefinition, &itemDef, sizeof(itemDef));
        localPlayer.activeItemCategory = itemDef.ItemCategory;

        uint64_t pMovement = Memory::read<uint64_t>(playerPtr + 0x5E8);
        Memory::writef(pMovement + 0xAC, 1.0f);
        Memory::writef(pMovement + 0xB0, 1.0f);
    }
    else {
        String displayName;
        Memory::read(basePlayer._displayName, &displayName, sizeof(displayName));
        if (players.count(playerPtr))
        {
            players.find(playerPtr)->second->pos.x = pModel.Position.x;
            players.find(playerPtr)->second->pos.y = pModel.Position.y;
            players.find(playerPtr)->second->pos.z = pModel.Position.z;
            players.find(playerPtr)->second->health = basePlayer.health;
            players.find(playerPtr)->second->maxHealth = basePlayer.maxHealth;
            players.find(playerPtr)->second->startHealth = basePlayer.startHealth;
            players.find(playerPtr)->second->PlayerFlags = basePlayer.PlayerFlags;
            players.find(playerPtr)->second->lastRead = GetTickCount();
        }
        else {
            std::unique_ptr<Player> player = std::make_unique<Player>();
            player->pos = pModel.Position;
            player->name = displayName.string;
            player->PlayerFlags = basePlayer.PlayerFlags;
            player->health = basePlayer.health;
            player->maxHealth = basePlayer.maxHealth;
            player->startHealth = basePlayer.startHealth;
            player->lastRead = GetTickCount();
            players.insert(std::make_pair(playerPtr, std::move(player)));
        }

    }
}

void ESP::Init(const InitEvent& evt)
{
    this->GameAssemblyBase = evt.GameAssemblyBase;
    NetworkablesThread = std::thread(&ESP::GetNetworkables, this);
    this->enabled = true;
}

void ESP::DrawSkeleton(LPDIRECT3DDEVICE9 d3Dev, uint64_t entity, D3DXMATRIX viewMatrix, int screenWidth, int screenHeight)
{
    BoneList bones[15] = {
        BoneList::l_foot, BoneList::l_knee, BoneList::l_hip,
        BoneList::r_foot, BoneList::r_knee, BoneList::r_hip,
        BoneList::spine1, BoneList::neck, BoneList::head,
        BoneList::l_upperarm, BoneList::l_forearm, BoneList::l_hand,
        BoneList::r_upperarm, BoneList::r_forearm, BoneList::r_hand
    };

    D3DXVECTOR2 BonePos[15] = {};

    for (int i = 0; i < 15; i++)
    {
        if (!WorldToScreen(GetPosition(GetBoneTransform(entity, (int)bones[i])), &BonePos[i], viewMatrix, screenWidth, screenHeight))
            return;
    }

    for (int j = 0; j < 15; j += 3)
    {
        Draw::Line(d3Dev, BonePos[j].x, BonePos[j].y, BonePos[j + 1].x, BonePos[j + 1].y, 3.f, true, D3DCOLOR_ARGB(255, 0, 0, 0));
        Draw::Line(d3Dev, BonePos[j].x, BonePos[j].y, BonePos[j + 1].x, BonePos[j + 1].y, 1.f, true, D3DCOLOR_ARGB(255, 255, 255, 255));
        Draw::Line(d3Dev, BonePos[j + 1].x, BonePos[j + 1].y, BonePos[j + 2].x, BonePos[j + 2].y, 3.f, true, D3DCOLOR_ARGB(255, 0, 0, 0));
        Draw::Line(d3Dev, BonePos[j + 1].x, BonePos[j + 1].y, BonePos[j + 2].x, BonePos[j + 2].y, 1.f, true, D3DCOLOR_ARGB(255, 255, 255, 255));
    }
}

void ESP::OnRender(const RenderEvent& evt)
{
    closestDistance = 1000;
    std::map<uint64_t, std::unique_ptr<Player>>::iterator it;
    for (it = players.begin(); it != players.end(); it++)
    {
        if (!showSleeper && it->second->hasFlags(PlayerFlags::Sleeping))
            continue;
        if (GetTickCount() - it->second->lastRead >= 3000)
        {
            it = players.erase(it);
            if (it != players.end())
                continue;
            else
                break;
        }

        D3DXVECTOR2 screenPos;
        ZeroMemory(&screenPos, sizeof(screenPos));
        if (WorldToScreen(it->second->pos, &screenPos, evt.viewMatrix, evt.screenWidth, evt.screenHeight))
        {
            int Distance = (int)CalculateDistance(localPlayer.pos, it->second->pos);
            int Distance2D = (int)CalculateDistance2D({ (FLOAT)evt.screenWidth / 2, (FLOAT)evt.screenHeight / 2 }, screenPos);
            if (Distance2D < closestDistance && !it->second->hasFlags(PlayerFlags::Sleeping) && !(GetKeyState(VK_RBUTTON) < 0) && (int)it->second->health > 0)
            {
                closestDistance = Distance2D;
                closestPlayer = it->first;
            }

            if (Distance >= this->maxDistance)
                continue;
            if (enabled)
            {
                std::wstring distanceString = L"[HP: " + std::to_wstring((int)it->second->health) + L"][" + std::to_wstring(Distance) + L"m]";
                DrawSkeleton(evt.d3Device, it->first, evt.viewMatrix, evt.screenWidth, evt.screenHeight);
                Draw::DrawTag(evt.d3Font, evt.d3Device, it->second->name, distanceString, L"!", (int)screenPos.x, (int)screenPos.y, D3DCOLOR_ARGB(255, 255, 255, 255), D3DCOLOR_ARGB(255, 185, 187, 190));
            }
        }
    }
    std::map<uint64_t, std::unique_ptr<InterestedObject>>::iterator objIt;
    for (objIt = interestedObjects.begin(); objIt != interestedObjects.end(); objIt++)
    {
        if (GetTickCount() - objIt->second->lastRead >= 3000)
        {
            objIt = interestedObjects.erase(objIt);
            if (objIt != interestedObjects.end())
                continue;
            else
                break;
        }
        switch (objIt->second->objType)
        {
        case SmallStash:
            if (!showStash)
                continue;
            break;
        case SupplyDrop:
            if (!showSupply)
                continue;
            break;
        case ToolCupboard:
            if (!showTool)
                continue;
            break;
        case LargeBox:
            if (!showBox)
                continue;
            break;
        case Minicopter:
            if (!showMini)
                continue;
            break;
        case HackableCrate:
            if (!showHackable)
                continue;
            break;
        case PatrolHelicopter:
            if (!showPatrol)
                continue;
            break;
        case Corpse:
            if (!showCorpse)
                continue;
            break;
        }
        D3DXVECTOR2 screenPos;
        ZeroMemory(&screenPos, sizeof(screenPos));
        if (WorldToScreen(objIt->second->position, &screenPos, evt.viewMatrix, evt.screenWidth, evt.screenHeight))
        {
            if (enabled)
            {
                int Distance = (int)CalculateDistance(localPlayer.pos, objIt->second->position);
                std::wstring distanceString = L"[" + std::to_wstring(Distance) + L"m]";
                Draw::DrawTag(evt.d3Font, evt.d3Device, objIt->second->name, distanceString, L"!", (int)screenPos.x, (int)screenPos.y, D3DCOLOR_ARGB(255, 255, 255, 255), D3DCOLOR_ARGB(255, 185, 187, 190));
            }
        }
    }
}

void ESP::ImguiSetup()
{
    ImGui::Begin("ESP", 0, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::GetStyle().Alpha = 1.0f;
    ImGui::Checkbox("Enabled", &this->enabled);
    ImGui::Checkbox("Sleepers", &this->showSleeper);
    ImGui::SliderInt("Max Distance", &this->maxDistance, 1, 2000);
    ImGui::Separator();
    ImGui::Text("Objects");
    if (ImGui::CollapsingHeader("Object Types"))
    {
        ImGui::Checkbox("Corpse", &this->showCorpse);
        ImGui::Checkbox("Supply Drop", &this->showSupply);
        ImGui::Checkbox("Tool Cupboard", &this->showTool);
        ImGui::Checkbox("Small Stash", &this->showStash);
        ImGui::Checkbox("Large Box", &this->showBox);
        ImGui::Checkbox("Hackable Crate", &this->showHackable);
        ImGui::Checkbox("Patrol Helicopter", &this->showPatrol);
        ImGui::TreePop();
    }
    ImGui::End();
}

LocalPlayer& ESP::GetLocalPlayer()
{
    return localPlayer;
}

uint64_t ESP::GetClosestPlayer()
{
    return closestPlayer;
}