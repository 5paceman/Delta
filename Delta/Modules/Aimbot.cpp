#include "Aimbot.h"

void Aimbot::Aim()
{
    while (true)
    {
        if (this->enabled)
        {
            if (GetKeyState(VK_RBUTTON) < 0 && ESP::closestPlayer != 0 && ESP::players.count(ESP::closestPlayer) && ESP::players.find(ESP::closestPlayer)->second->health > 0)
            {
                if (ESP::closestPlayer != this->previousPlayer)
                    this->curSmoothTime = 0;

                this->curSmoothTime += 0.033f;
                if (this->curSmoothTime > this->smoothTime)
                    this->curSmoothTime = this->smoothTime;

                uint64_t eHeadTransform;
                if (boneAim == BoneList::Random)
                    eHeadTransform = GetBoneTransform(ESP::closestPlayer, this->RandomBone());
                else
                    eHeadTransform = GetBoneTransform(ESP::closestPlayer, (int)boneAim);

                D3DXVECTOR3 eHeadPos = GetPosition(eHeadTransform);
               // if (!WorldToScreen(eHeadPos, nullptr, this->viewMatrix, this->sWidth, this->sHeight))
               //     continue;
                D3DXVECTOR3 cameraPos;
                if (GetPositionFromViewMatrix(&cameraPos, &viewMatrix))
                {
                    float bulletGravity = 0.75f;
                    double Distance = CalculateDistance(cameraPos, eHeadPos);
                    float BulletTime = Distance / 375.f;
                    eHeadPos.y += (bulletGravity * BulletTime * BulletTime);
                    D3DXVECTOR2 angles = WorldToAngle(cameraPos, eHeadPos);
                    D3DXVECTOR3 aimAngles;
                    aimAngles.x = angles.x;
                    aimAngles.y = angles.y;
                    uint64_t Input = Memory::read<uint64_t>(ESP::localPlayer.ptr + 0x5E0);
                    D3DXVECTOR3 bodyAngles = Memory::read<D3DXVECTOR3>(Input + 0x3C);
                    if (this->smooth)
                        SmoothAngle(bodyAngles, aimAngles, this->curSmoothTime / this->smoothTime);

                    bodyAngles.x = aimAngles.x;
                    bodyAngles.y = aimAngles.y;
                    Memory::write<D3DXVECTOR3>(Input + 0x3C, bodyAngles);
                }
                this->previousPlayer = ESP::closestPlayer;
                Sleep(16);
            }
            else {
                this->curSmoothTime = 0;
            }
        }
    }
}

int Aimbot::RandomBone()
{
    std::array<int, 5> list{ (int)BoneList::head, (int)BoneList::pelvis, (int)BoneList::neck, (int)BoneList::l_knee, (int)BoneList::l_breast };
    int index = rand() % list.size(); // pick a random index
    return list[index]; // a random value taken from that list
}

void Aimbot::Init(const InitEvent& evt)
{
    this->AimThread = std::thread(&Aimbot::Aim, this);
    this->GameAssemblyBase = evt.GameAssemblyBase;
}

void Aimbot::ImguiSetup()
{
    ImGui::Begin("Aimbot", (bool*)1, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Checkbox("Enabled", &this->enabled);
    if (ImGui::CollapsingHeader("Aim At"))
    {
        if (ImGui::RadioButton("Head", this->boneAim == BoneList::head))
        {
            if (this->boneAim != BoneList::head)
                this->boneAim = BoneList::head;
        }
        if (ImGui::RadioButton("Pelvis", this->boneAim == BoneList::pelvis))
        {
            if (this->boneAim != BoneList::pelvis)
                this->boneAim = BoneList::pelvis;
        }
        if (ImGui::RadioButton("Neck", this->boneAim == BoneList::neck))
        {
            if (this->boneAim != BoneList::neck)
                this->boneAim = BoneList::neck;
        }
        if (ImGui::RadioButton("Knee", this->boneAim == BoneList::l_knee))
        {
            if (this->boneAim != BoneList::l_knee)
                this->boneAim = BoneList::l_knee;
        }
        if (ImGui::RadioButton("Chest", this->boneAim == BoneList::l_breast))
        {
            if (this->boneAim != BoneList::l_breast)
                this->boneAim = BoneList::l_breast;
        }
        if (ImGui::RadioButton("Randomize", this->boneAim == BoneList::Random))
        {
            if (this->boneAim != BoneList::Random)
                this->boneAim = BoneList::Random;
        }
    }
    ImGui::Separator();
    ImGui::Checkbox("Smoothing", &this->smooth);
    ImGui::SliderFloat("Smooth Time", &this->smoothTime, 0.1f, 25.f, "%.2f");
	ImGui::End();
}

void Aimbot::OnRender(const RenderEvent& evt)
{
    this->viewMatrix = evt.viewMatrix;
    this->sHeight = evt.screenHeight;
    this->sWidth = evt.screenWidth;
}