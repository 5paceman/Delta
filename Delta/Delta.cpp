#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dwmapi.h>
#include "Memory.h"
#include "Modules.h"
#include "Addresses.h"
#include "DrawHelper.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib, "dwmapi.lib")

int screenWidth = 800;
int screenHeight = 800;
#define CENTERX (GetSystemMetrics(SM_CXSCREEN) /2)-(sWidth / 2)
#define CENTERY (GetSystemMetrics(SM_CYSCREEN) /2)-(sHeight / 2)
LPDIRECT3D9 d3Direct;
LPDIRECT3DDEVICE9 d3Dev;
HWND hWnd;
LPD3DXFONT d3Font;
const MARGINS margin = { -1 };
DWORD RustPID = NULL;
HWND parentWindow;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void ChangeClickability(bool canclick, HWND ownd);

bool ShowConfig = false;

int LastFrameRate, FrameRate, LastTickCount;

RenderEvent renderEvent;

std::vector<Module*> modules;

uint64_t MainCamera = 0;

void render()
{
    bool test = false;
    ImGui_ImplDX9_NewFrame();
    for (auto mod : modules)
    {
        mod->ImguiSetup();
    }
    d3Dev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
    d3Dev->BeginScene();
    d3Dev->SetRenderState(D3DRS_ALPHAFUNC, FALSE);


    int tickCount = GetTickCount64();
    if (tickCount - LastTickCount >= 1000)
    {
        LastFrameRate = FrameRate;
        FrameRate = 0;
        LastTickCount = tickCount;
    }
    FrameRate++;
    
    renderEvent.viewMatrix = Memory::read<D3DXMATRIX>(MainCamera + 0x2E4);
    std::wstring info = L"Delta v1.0\n";
    info += L"FPS: " + std::to_wstring(LastFrameRate);
    info += L"\nActive Item: " + std::to_wstring(ESP::localPlayer.activeItemCategory);
    if (ESP::players.count(ESP::closestPlayer))
    {
        info += L"\nTarget: " + ESP::players.find(ESP::closestPlayer)->second->name;
    }
    Draw::DrawShadowedString(0, 0, D3DCOLOR_ARGB(255, 255, 255, 255), d3Font, info.c_str());

    for (auto mod : modules)
    {
        mod->OnRender(renderEvent);
    }

    if (ShowConfig)
    {
        ChangeClickability(true, hWnd);
        ImGui::Render();
    }
    else {
        ChangeClickability(false, hWnd);
    }

    d3Dev->EndScene();
    d3Dev->Present(NULL, NULL, NULL, NULL);
    Sleep(16);
}

void InitModules()
{
    renderEvent.screenWidth = screenWidth;
    renderEvent.screenHeight = screenHeight;
    renderEvent.d3Device = d3Dev;
    renderEvent.d3Font = d3Font;

    InitEvent initEvent;
    initEvent.GameAssemblyBase = Memory::get_module_base(RustPID, L"GameAssembly.dll");
    initEvent.UnityPlayerBase = Memory::get_module_base(RustPID, L"UnityPlayer.dll");

    MainCamera = Memory::read(initEvent.UnityPlayerBase + GOM, { 0x8, 0x10, 0x30, 0x18 });
    modules.push_back(new ESP());
    modules.push_back(new Aimbot());
    modules.push_back(new MiscUtils());
    modules.push_back(new WeaponMod());
    for (auto mod : modules)
    {
        mod->Init(initEvent);
    }
}

void InitImgui()
{
    ImGui_ImplDX9_Init(hWnd, d3Dev);
    ImGuiStyle* style = &ImGui::GetStyle();

    style->WindowPadding = ImVec2(15, 15);
    style->WindowRounding = 5.0f;
    style->FramePadding = ImVec2(5, 5);
    style->FrameRounding = 4.0f;
    style->ItemSpacing = ImVec2(12, 8);
    style->ItemInnerSpacing = ImVec2(8, 6);
    style->IndentSpacing = 25.0f;
    style->ScrollbarSize = 15.0f;
    style->ScrollbarRounding = 9.0f;
    style->GrabMinSize = 5.0f;
    style->GrabRounding = 3.0f;

    style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
    style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
    style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
    style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
    style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
    style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
    style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
    style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
    style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
}

void initializeD3D(HWND hWnd)
{
    d3Direct = Direct3DCreate9(D3D_SDK_VERSION);
    D3DPRESENT_PARAMETERS d3Params;

    ZeroMemory(&d3Params, sizeof(d3Params));
    d3Params.Windowed = TRUE;
    d3Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3Params.hDeviceWindow = hWnd;
    d3Params.BackBufferFormat = D3DFMT_A8R8G8B8;
    d3Params.BackBufferWidth = screenWidth;
    d3Params.BackBufferHeight = screenHeight;
    d3Params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    d3Params.EnableAutoDepthStencil = TRUE;
    d3Params.AutoDepthStencilFormat = D3DFMT_D16;

    d3Direct->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3Params, &d3Dev);

    D3DXCreateFont(d3Dev, 12, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Lucida Console", &d3Font);
}

int windowstate = 1;

void ChangeClickability(bool canclick, HWND ownd)
{
    long style = GetWindowLong(
        ownd, GWL_EXSTYLE);
    if (canclick) {
        style &= ~WS_EX_LAYERED;
        SetWindowLong(ownd,
            GWL_EXSTYLE, style);
        SetForegroundWindow(ownd);
        windowstate = 1;
    }
    else {
        style |= WS_EX_LAYERED;
        SetWindowLong(ownd,
            GWL_EXSTYLE, style);
        windowstate = 0;
    }
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    DWORD lpdwProcessId;
    GetWindowThreadProcessId(hWnd, &lpdwProcessId);
    if (lpdwProcessId == lParam)
    {
        parentWindow = hWnd;
        return FALSE;
    }
    return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    while ((RustPID = Memory::get_proc_id(L"RustClient.exe")) == NULL) {
        Sleep(30000);
    }


    EnumWindows(EnumWindowsProc, RustPID);
    RECT parentRect;
    if (parentWindow != NULL)
    {

        GetWindowRect(parentWindow, &parentRect);
        screenWidth = parentRect.right - parentRect.left;
        screenHeight = parentRect.bottom - parentRect.top;
    }

    WNDCLASSEX wClassEx;

    ZeroMemory(&wClassEx, sizeof(wClassEx));
    wClassEx.cbSize = sizeof(wClassEx);
    wClassEx.style = ACS_TRANSPARENT;
    wClassEx.lpfnWndProc = WindowProc;
    wClassEx.hInstance = hInstance;
    wClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
    wClassEx.hbrBackground = (HBRUSH)RGB(0, 0, 0);
    wClassEx.lpszClassName = L"Delta";

    RegisterClassEx(&wClassEx);

    hWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE, L"Delta", L"",  WS_POPUP, parentRect.left, parentRect.top, screenWidth, screenHeight, NULL, NULL, hInstance, NULL);

    SetWindowLong(hWnd, GWL_EXSTYLE, (int)GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);
    SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);

    ShowWindow(hWnd, nCmdShow);

    initializeD3D(hWnd);

    InitImgui();
    InitModules();

    MSG msg;
    while (TRUE)
    {
        ::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        render();
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == WM_QUIT)
            exit(0);
    }

    ImGui_ImplDX9_Shutdown();
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (GetAsyncKeyState(VK_END) & 1)
    {
        ShowConfig ^= 1;
    }
    if (ImGui_ImplDX9_WndProcHandler(hWnd, message, wParam, lParam))
    {
        return true;
    }

    switch (message)
    {
    case WM_PAINT:
    {
        DwmExtendFrameIntoClientArea(hWnd, &margin);
    }break;

    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    } break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}