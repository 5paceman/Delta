#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <math.h>

class Draw {
public:
    static void DrawString(int x, int y, DWORD color, DWORD Format, LPD3DXFONT pFont, const wchar_t* string);
    static void DrawShadowedString(int x, int y, DWORD color, LPD3DXFONT pFont, const wchar_t* string);
    static void DrawCShadowedString(int x, int y, DWORD color, LPD3DXFONT pFont, const wchar_t* string);
    static int GetTextWidth(const wchar_t* szText, LPD3DXFONT pFont);
    static int GetTextHeight(const wchar_t* szText, LPD3DXFONT pFont);
    static void Line(LPDIRECT3DDEVICE9 d3Dev, float x1, float y1, float x2, float y2, float width, bool antialias, D3DCOLOR color);
    static void Box(LPDIRECT3DDEVICE9 d3Dev, float x, float y, float w, float h, float linewidth, D3DCOLOR color);
    static void BorderedBox(LPDIRECT3DDEVICE9 d3Dev, float x, float y, float w, float h, float width, D3DCOLOR boxColor, D3DCOLOR borderColor);
    static void BoxFilled(LPDIRECT3DDEVICE9 d3Dev, float x, float y, float w, float h, D3DCOLOR color);
    static void DrawTag(LPD3DXFONT pFont, LPDIRECT3DDEVICE9 d3Dev, std::wstring firstLine, std::wstring secondLine, const wchar_t* icon, int x, int y, D3DCOLOR firstColor, D3DCOLOR secondColor);

};
