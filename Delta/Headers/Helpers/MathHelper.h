#pragma once
#include <math.h>
#include <d3dx9.h>
#include "Memory.h"
#include "Structs.h"
#include <immintrin.h>

double CalculateDistance(D3DXVECTOR3 vectorOne, D3DXVECTOR3 vectorTwo);
double CalculateDistance2D(D3DXVECTOR2 vectorOne, D3DXVECTOR2 vectorTwo);
bool GetPositionFromViewMatrix(D3DXVECTOR3* out, D3DXMATRIX* m);
bool WorldToScreen(D3DXVECTOR3 origin, D3DXVECTOR2* out, D3DXMATRIX viewMatrix, int sWidth, int sHeight);
D3DXVECTOR2 WorldToAngle(D3DXVECTOR3 origin, D3DXVECTOR3 destination);
float rad2deg(float radians);
D3DXVECTOR3 GetPosition(uint64_t transform);
void NormalizeAngles(D3DXVECTOR3& vector);
void SmoothAngle(D3DXVECTOR3& From, D3DXVECTOR3& To, float percent);