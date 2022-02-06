#include "MathHelper.h"

void NormalizeAngles(D3DXVECTOR3& vector)
{
    if (vector.x < -89) vector.x = -89;
    else if (vector.x > 89) vector.x = 89;
    if (vector.y < -360) vector.y += 360;
    else if (vector.y > 360) vector.y -= 360;
}

void SmoothAngle(D3DXVECTOR3& From, D3DXVECTOR3& To, float percent)
{
    D3DXVECTOR3 Delta = From - To;
    NormalizeAngles(Delta);
    Delta.x *= percent;
    Delta.y *= percent;
    if(From.y > 0)
        To = From - Delta;
    else
        To = Delta - From;
}

float rad2deg(float radians)
{
    return (float)(radians * (180.0 / 3.141592653589793238463));
}

double CalculateDistance(D3DXVECTOR3 vectorOne, D3DXVECTOR3 vectorTwo)
{
    return sqrt(pow(vectorTwo.x - vectorOne.x, 2.0) + pow(vectorTwo.y - vectorOne.y, 2.0) + pow(vectorTwo.z - vectorOne.z, 2.0));
}

D3DXVECTOR2 WorldToAngle(D3DXVECTOR3 origin, D3DXVECTOR3 destination)
{
    D3DXVECTOR3 direction = { origin.x - destination.x, origin.y - destination.y, origin.z - destination.z };
    double length = sqrt(pow(direction.x, 2.0) + pow(direction.y, 2.0) + pow(direction.z, 2.0));
    float y = rad2deg(-atan2(direction.x, -direction.z));
    float x = rad2deg((float)asin(direction.y / length));

    return { x, y };
}

bool WorldToScreen(D3DXVECTOR3 origin, D3DXVECTOR2* out, D3DXMATRIX viewMatrix, int sWidth, int sHeight)
{
    D3DXMATRIX temp;

    D3DXMatrixTranspose(&temp, &viewMatrix);

    D3DXVECTOR3 translationVector = D3DXVECTOR3(temp._41, temp._42, temp._43);
    D3DXVECTOR3 up = D3DXVECTOR3(temp._21, temp._22, temp._23);
    D3DXVECTOR3 right = D3DXVECTOR3(temp._11, temp._12, temp._13);

    float w = D3DXVec3Dot(&translationVector, &origin) + temp._44;

    if (w < 0.098f)
        return false;

    float y = D3DXVec3Dot(&up, &origin) + temp._24;
    float x = D3DXVec3Dot(&right, &origin) + temp._14;
    if (out != nullptr)
    {
        out->x = (sWidth / 2) * (1.f + x / w);
        out->y = (sHeight / 2) * (1.f - y / w);
    }

    return true;
}

double CalculateDistance2D(D3DXVECTOR2 vectorOne, D3DXVECTOR2 vectorTwo)
{
    return sqrt(pow(vectorTwo.x - vectorOne.x, 2) + pow(vectorTwo.y - vectorOne.y, 2));
}

bool GetPositionFromViewMatrix(D3DXVECTOR3* out, D3DXMATRIX* m)
{
    if (!m) return false;
    float	A = m->_22 * m->_33 - m->_32 * m->_23,
        B = m->_32 * m->_13 - m->_12 * m->_33,
        C = m->_12 * m->_23 - m->_22 * m->_13,
        Z = m->_11 * A + m->_21 * B + m->_31 * C;

    if (abs(Z) < 0.0001) return false;

    float	D = m->_31 * m->_23 - m->_21 * m->_33,
        E = m->_11 * m->_33 - m->_31 * m->_13,
        F = m->_21 * m->_13 - m->_11 * m->_23,
        G = m->_21 * m->_32 - m->_31 * m->_22,
        H = m->_31 * m->_12 - m->_11 * m->_32,
        K = m->_11 * m->_22 - m->_21 * m->_12;

    out->x = -(A * m->_41 + D * m->_42 + G * m->_43) / Z;
    out->y = -(B * m->_41 + E * m->_42 + H * m->_43) / Z;
    out->z = -(C * m->_41 + F * m->_42 + K * m->_43) / Z;
    return true;
}

D3DXVECTOR3 GetPosition(uint64_t transform)
{
    Matrix34* pMatriciesBuf;
    __m128 result;

    const __m128 mulVec0 = { -2.000, 2.000, -2.000, 0.000 };
    const __m128 mulVec1 = { 2.000, -2.000, -2.000, 0.000 };
    const __m128 mulVec2 = { -2.000, -2.000, 2.000, 0.000 };
    TransformAccessReadOnly pTransformAccessReadOnly;
    Memory::read(transform + 0x38, &pTransformAccessReadOnly, sizeof(pTransformAccessReadOnly));

    TransformData transformData;
    Memory::read(pTransformAccessReadOnly.pTransformData + 0x18, &transformData, sizeof(transformData));
    int32_t Index = 0;
    Memory::read(transform + 0x38 + sizeof(uint64_t), &Index, sizeof(int32_t));

    size_t sizeMatriciesBuf = sizeof(Matrix34) * Index + sizeof(Matrix34);
    size_t sizeIndicesBuf = sizeof(int) * Index + sizeof(int);

    // Allocate memory for storing large amounts of data (matricies and indicies)
    pMatriciesBuf = (Matrix34*)malloc(sizeMatriciesBuf);
    int* pIndicesBuf = (int*)malloc(sizeIndicesBuf);

    Memory::read(transformData.pTransformArray, pMatriciesBuf, sizeMatriciesBuf);
    Memory::read(transformData.pTransformIndices, pIndicesBuf, sizeIndicesBuf);
    result = *(__m128*)((ULONGLONG)pMatriciesBuf + 0x30 * Index);
    int transformIndex = *(int*)((ULONGLONG)pIndicesBuf + 0x4 * Index);

    while (transformIndex >= 0)
    {
        Matrix34 matrix34 = *(Matrix34*)((ULONGLONG)pMatriciesBuf + 0x30 * transformIndex);

        __m128 xxxx = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x00));	// xxxx
        __m128 yyyy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x55));	// yyyy
        __m128 zwxy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x8E));	// zwxy
        __m128 wzyw = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xDB));	// wzyw
        __m128 zzzz = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xAA));	// zzzz
        __m128 yxwy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x71));	// yxwy
        __m128 tmp7 = _mm_mul_ps(*(__m128*)(&matrix34.vec2), result);

        result = _mm_add_ps(
            _mm_add_ps(
                _mm_add_ps(
                    _mm_mul_ps(
                        _mm_sub_ps(
                            _mm_mul_ps(_mm_mul_ps(xxxx, mulVec1), zwxy),
                            _mm_mul_ps(_mm_mul_ps(yyyy, mulVec2), wzyw)),
                        _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0xAA))),
                    _mm_mul_ps(
                        _mm_sub_ps(
                            _mm_mul_ps(_mm_mul_ps(zzzz, mulVec2), wzyw),
                            _mm_mul_ps(_mm_mul_ps(xxxx, mulVec0), yxwy)),
                        _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x55)))),
                _mm_add_ps(
                    _mm_mul_ps(
                        _mm_sub_ps(
                            _mm_mul_ps(_mm_mul_ps(yyyy, mulVec0), yxwy),
                            _mm_mul_ps(_mm_mul_ps(zzzz, mulVec1), zwxy)),
                        _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x00))),
                    tmp7)), *(__m128*)(&matrix34.vec0));
        try {
            transformIndex = *(int*)((ULONGLONG)pIndicesBuf + 0x4 * transformIndex);
        }
        catch (...)
        {
            return D3DXVECTOR3{ 0, 0, 0 };
        }
    }
    free(pMatriciesBuf);
    free(pIndicesBuf);
    return D3DXVECTOR3(result.m128_f32[0], result.m128_f32[1], result.m128_f32[2]);
}
