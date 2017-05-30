// System includes

#include <stdio.h>
#include <assert.h>

#include "DataTypes.h"

float3 make_float3(float x0, float y0, float z0);

#include <iostream>
#include <vector>
using namespace std;

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) { if(p != NULL){ delete [] p; p = NULL; } }	//安全删除指针p
#endif

float Distance(const float3 &a, const float3 &b);

// 计算交线长
void ComputeIntsections(float3* pDst, float3* temp, float* pPrj, int nRaysIndex, int nAnglesIndex, int nAngles, float* pSrc, int Xmin, int Ymin, int Xmax, int Ymax, float &k, float &c);

int ComputeIntsections(float3* pDst, float3* temp, int Xmin, int Ymin, int Xmax, int Ymax, float &k, float &c);

const char* Art(float* pDst, int nWidth, int nHeight, float* pPrj, int nRays, int nAngles, float rays_separation, float angles_separation, int nItNum = 10);

const char* ArtRadon(float* pPrj, int nRays, int nAngles, float* pSrc, int nWidth, int nHeight, float rays_separation, float angles_separation);
