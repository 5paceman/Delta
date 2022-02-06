#pragma once
#include <d3dx9.h>

class PlayerInventory
{
public:
    char pad_0000[32]; //0x0000
    uint64_t PtrContainerMain; //0x0020
    uint64_t PtrBeltContainer; //0x0028
    uint64_t PtrContainerWear; //0x0030
};

enum PlayerFlags : int
{
	Aiming = 16384,
	Connected = 256,
	IsAdmin = 4,
	Relaxed = 65536,
	SafeZone = 131072,
	ServerFall = 262144,
	Sleeping = 16,
	Workbench1 = 1048576,
	Workbench2 = 2097152,
	Workbench3 = 4194304,
	Wounded = 64
};

enum class BoneList : int
{
    pelvis = 0,
    l_hip = 1,
    l_knee = 2,
    l_foot = 3,
    l_toe = 4,
    l_ankle_scale = 5,
    penis = 6,
    GenitalCensor = 7,
    GenitalCensor_LOD0 = 8,
    Inner_LOD0 = 9,
    Inner_LOD1 = 10,
    GenitalCensor_LOD1 = 11,
    GenitalCensor_LOD2 = 12,
    r_hip = 13,
    r_knee = 14,
    r_foot = 15,
    r_toe = 16,
    r_ankle_scale = 17,
    spine1 = 18,
    spine1_scale = 19,
    spine2 = 20,
    spine3 = 21,
    spine4 = 22,
    l_clavicle = 23,
    l_upperarm = 24,
    l_forearm = 25,
    l_hand = 26,
    l_index1 = 27,
    l_index2 = 28,
    l_index3 = 29,
    l_little1 = 30,
    l_little2 = 31,
    l_little3 = 32,
    l_middle1 = 33,
    l_middle2 = 34,
    l_middle3 = 35,
    l_prop = 36,
    l_ring1 = 37,
    l_ring2 = 38,
    l_ring3 = 39,
    l_thumb1 = 40,
    l_thumb2 = 41,
    l_thumb3 = 42,
    IKtarget_righthand_min = 43,
    IKtarget_righthand_max = 44,
    l_ulna = 45,
    neck = 46,
    head = 47,
    jaw = 48,
    eyeTranform = 49,
    l_eye = 50,
    l_Eyelid = 51,
    r_eye = 52,
    r_Eyelid = 53,
    r_clavicle = 54,
    r_upperarm = 55,
    r_forearm = 56,
    r_hand = 57,
    r_index1 = 58,
    r_index2 = 59,
    r_index3 = 60,
    r_little1 = 61,
    r_little2 = 62,
    r_little3 = 63,
    r_middle1 = 64,
    r_middle2 = 65,
    r_middle3 = 66,
    r_prop = 67,
    r_ring1 = 68,
    r_ring2 = 69,
    r_ring3 = 70,
    r_thumb1 = 71,
    r_thumb2 = 72,
    r_thumb3 = 73,
    IKtarget_lefthand_min = 74,
    IKtarget_lefthand_max = 75,
    r_ulna = 76,
    l_breast = 77,
    r_breast = 78,
    BoobCensor = 79,
    BreastCensor_LOD0 = 80,
    Inner_LOD2 = 81,
    Inner_LOD3 = 82,
    BreastCensor_LOD1 = 83,
    BreastCensor_LOD2 = 84,
    Random = 200
};

class Player {
public:
    uint64_t PlayerFlags = 0;
    D3DXVECTOR3 pos = {};
    D3DXVECTOR3 velocity = {};
    D3DXVECTOR3 newVelocity = {};
    std::wstring name;
    uint64_t ptr = 0;
    uint64_t PtrPlayerInventory = 0;
    float startHealth = 0;
    float maxHealth = 0;
    float health = 0;
    uint64_t clActiveItem = 0;
    DWORD lastRead = 0;

    bool hasFlags(int flags)
    {
        return (this->PlayerFlags & flags);
    }

    Item GetItemInfo(int id)
    {
        PlayerInventory inventory;
        Memory::read(this->ptr, &inventory, sizeof(inventory));
        ItemContainer beltContainer;
        Memory::read(inventory.PtrBeltContainer, &beltContainer, sizeof(beltContainer));
        uint64_t Items = Memory::read<uint64_t>(beltContainer.PtrItemList + 0x10);
        Item item;
        uint64_t weaponPtr = Memory::read<uint64_t>(Items + 0x20 + (id * 0x8));
        Memory::read(weaponPtr, &item, sizeof(item));
        return item;
    }

    Item GetActiveItem()
    {
        Item item;
        for (int i = 0; i < 6; i++)
        {
            item = GetItemInfo(i);
            if (this->clActiveItem == item.uid)
            {
                return item;
            }
        }
        return item;
    }


};

class LocalPlayer {
public:
    uint64_t PlayerFlags = 0;
    D3DXVECTOR3  pos = {};
    D3DXVECTOR3  headPos = {};
    D3DXVECTOR3 newVelocity = {};
    std::wstring activeItemName;
    uint64_t activeItemCategory = 0;
    uint64_t activeItemId = 0;
    uint64_t activeItemAmmoId = 0;
    float activeItemBulletSpeed = 0;
    int clActiveItem = 0;
    uint64_t ptr = 0;
    uint64_t PtrPlayerInventory;
    float startHealth = 0;
    float maxHealth = 0;
    float health = 0;

    bool hasFlags(int flags)
    {
        return (this->PlayerFlags & flags);
    }

    Item GetItemInfo(int id)
    {
        PlayerInventory inventory;
        Memory::read(this->PtrPlayerInventory, &inventory, sizeof(inventory));
        ItemContainer beltContainer;
        Memory::read(inventory.PtrBeltContainer, &beltContainer, sizeof(beltContainer));
        uint64_t Items = Memory::read<uint64_t>(beltContainer.PtrItemList + 0x10);
        Item item;
        uint64_t weaponPtr = Memory::read<uint64_t>(Items + 0x20 + (id * 0x8));
        Memory::read(weaponPtr, &item, sizeof(item));
        return item;
    }

    Item GetActiveItem()
    {
        Item item;
        for (int i = 0; i < 6; i++)
        {
            item = GetItemInfo(i);
            if (this->clActiveItem == item.uid)
            {
                return item;
            }
        }
        return item;
    }
};

class BasePlayer
{
public:
    char pad_0000[432]; //0x0000
    float startHealth; //0x01B0
    char pad_01B4[64]; //0x01B4
    float health; //0x01F4
    float maxHealth; //0x01F8
    char pad_01FC[660]; //0x01FC
    uint64_t PtrPlayerModel; //0x0490
    char pad_0498[152]; //0x0498
    uint64_t clActiveItem; //0x0530
    char pad_0538[128]; //0x0538
    uint64_t PlayerFlags; //0x05B8
    char pad_05C0[8]; //0x05C0
    uint64_t PtrPlayerInventory; //0x05C8
    char pad_05D0[80]; //0x05D0
    uint64_t _displayName; //0x0620
}; //Size: 0x05E9

struct PlayerModel
{
public:
    char pad_0000[456]; //0x0000
    D3DXVECTOR3 Position; //0x01C8
    D3DXVECTOR3 Velocity; //0x01D4
    D3DXVECTOR3 speedOverride; //0x01E0
    D3DXVECTOR3 newVelocity; //0x01EC
    char pad_01F8[64]; //0x01F8
    bool isVisible; //0x0238
    char pad_0239[16]; //0x0239
    bool isLocalPlayer; //0x0249
}; //Size: 0x0811