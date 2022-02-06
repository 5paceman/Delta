#pragma once
#include <map>
#include <thread>
#include "Memory.h"
#include "Module.h"
#include "Helpers.h"
#include "Addresses.h"

enum ObjectType : int
{
    SupplyDrop,
    HackableCrate,
    ToolCupboard,
    SmallStash,
    LargeBox,
    Minicopter,
    PatrolHelicopter,
    Corpse
};

class InterestedObject {
public:
    D3DXVECTOR3 position;
    std::wstring name;
    DWORD lastRead = 0;
    ObjectType objType;
};

class ESP : public Module {
private:
    uint64_t GameAssemblyBase;
    uint64_t values;
    uint64_t Buffer;
    std::map<uint64_t, std::unique_ptr<InterestedObject>> interestedObjects;
    double closestDistance = 1000;
    std::thread NetworkablesThread;
    bool showSleeper = true;
    bool showSupply = true;
    bool showTool = false;
    bool showStash = true;
    bool showBox = false;
    bool showMini = false;
    bool showHackable = false;
    bool showPatrol = true;
    bool showCorpse = false;
    bool shouldEnd = false;
    int maxDistance = 900;

public:
    static LocalPlayer localPlayer;
    static std::map<uint64_t, std::unique_ptr<Player>> players;
    static uint64_t closestPlayer;
    void GetNetworkables();
    void DrawSkeleton(LPDIRECT3DDEVICE9 d3Dev, uint64_t entity, D3DXMATRIX viewMatrix, int screenWidth, int screenHeight);
    void AddSO(uint64_t entityPtr, std::wstring name, ObjectType object, int boneId);
    void AddPlayer(uint64_t playerPtr);
    void OnRender(const RenderEvent& evt);
    void Init(const InitEvent& evt);
    void ImguiSetup();
    LocalPlayer& GetLocalPlayer();
    uint64_t GetClosestPlayer();
};