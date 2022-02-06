#pragma once
#include "Memory.h"

enum ItemCategory : int// TypeDefIndex: 9814
{
    Weapon = 0,
    Construction = 1,
    Items = 2,
    Resources = 3,
    Attire = 4,
    Tool = 5,
    Medical = 6,
    Food = 7,
    Ammunition = 8,
    Traps = 9,
    Misc = 10,
    All = 11,
    Common = 12,
    Component = 13,
    Search = 14,
    Favourite = 15,
    Electrical = 16,
    Fun = 17
};

class ItemDefinition
{
public:
    char pad_0000[24]; //0x0000
    short ItemId = 0; //0x0018
    uint64_t PtrShortName = 0; //0x0020
    char pad_0030[0x18]; //0x0030
    int ItemCategory = 0; //0x0040
}; //Size: 0x0041

class ItemContainer
{
public:
    char pad_0000[16]; //0x0000
    uint64_t Flags; //0x0010
    char pad_0018[16]; //0x0018
    uint8_t capacity; //0x0028
    char pad_0029[15]; //0x0029
    uint64_t PtrItemList; //0x0038
}; //Size: 0x0058

class Item
{
public:
    char pad_0000[32]; //0x0000
    uint64_t PtrItemDefinition; //0x0020
    uint64_t uid; //0x0028
    uint32_t amount; //0x0030
    char pad_0034[100]; //0x0034
    uint64_t PtrHeldEntity; //0x0098

    ItemCategory GetItemCategory()
    {
        ItemDefinition itemDef;
        Memory::read(PtrItemDefinition, &itemDef, sizeof(itemDef));
        return (ItemCategory)itemDef.ItemCategory;
    }
}; //Size: 0x0098

class TOD_Sky
{
public:
    char pad_0000[56]; //0x0000
    uint64_t PtrCycleParameters; //0x0038
    uint64_t PtrWorldParameters; //0x0040
    uint64_t PtrAtmosphereParameters; //0x0048
    uint64_t PtrDayParameters; //0x0050
    uint64_t PtrNightParameters; //0x0058
    uint64_t PtrSunParameters; //0x0060
    uint64_t PtrMoonParameters; //0x0068
    char pad_0070[16]; //0x0070
    uint64_t PtrLightParameters; //0x0080
}; //Size: 0x0088

class RecoilProperties
{
public:
    char pad_0000[24]; //0x0000
    float recoilYawMin; //0x0018
    float recoilYawMax; //0x001C
    float recoilPitchMin; //0x0020
    float recoilPitchMax; //0x0024
}; //Size: 0x0028

class BaseProjectile
{
public:
    char pad_0000[620]; //0x0000
    float projectileVelocityScale; //0x026C
    bool automatic; //0x0270
    char pad_0271[67]; //0x0271
    float reloadFractionDuration; //0x02B4
    float aimSway; //0x02B8
    char pad_02BC[4]; //0x02BC
    uint64_t PtrRecoilProperties; //0x02C0
    char pad_02C8[8]; //0x02C8
    float aimCone; //0x02D0
    float hipAimCone; //0x02D4
    float aimconePenaltyPerShot; //0x02D8
    char pad_02DC[20]; //0x02DC
    bool isCycling; //0x02F0
    bool aiming; //0x02F1
}; //Size: 0x041D

struct TransformAccessReadOnly
{
    ULONGLONG	pTransformData;
    int			index;
};

struct TransformData
{
    ULONGLONG pTransformArray;
    ULONGLONG pTransformIndices;
};

struct Matrix34
{
    D3DXVECTOR4 vec0;
    D3DXVECTOR4 vec1;
    D3DXVECTOR4 vec2;
};

struct String
{
    char pad[0x10];
    int length = 0;
    wchar_t string[256] = { 0 };
};