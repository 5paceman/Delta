#include "GameHelper.h"

uint64_t GetBoneTransform(uint64_t entity, int BoneIndex)
{
	uint64_t model = Memory::read<uint64_t>(entity + 0x118);
	uint64_t boneTransforms = Memory::read<uint64_t>(model + 0x48);
	uint64_t boneTransform = Memory::read<uint64_t>(boneTransforms + (0x20 + BoneIndex * 0x8));
	uint64_t returnVal = Memory::read<uint64_t>(boneTransform + 0x10);
	return returnVal;
}

