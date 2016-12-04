// System includes

#include <stdio.h>
#include <assert.h>

#ifdef CUDA

// CUDA runtime
#include <cuda_runtime.h>

// CUDA helper functions
#include <helper_cuda.h>         // helper functions for CUDA error check

// Helper functions and utilities to work with CUDA
#include <helper_functions.h>

#include "cuda_runtime_api.h"

#else 

struct float3 
{
	float x, y, z;
	float3(): x(0), y(0), z(0){ }
	float3(float x0, float y0, float z0): x(x0), y(y0), z(z0) { }
};

float3 make_float3(float x0, float y0, float z0);

#endif // CUDA

#include <iostream>
#include <vector>
using namespace std;

#define SAFE_DELETE(p) { if(p != NULL){ delete [] p; p = NULL; } }	//安全删除指针p

float LineGetXValue(float &k, float &c, float y);

float LineGetYValue(float &k, float &c, float x);

float Distance(const float3 &a, const float3 &b);

// 计算交线长
void ComputeIntsections(float3* pDst, float3* temp, float* pPrj, int nRaysIndex, int nAnglesIndex, int nAngles, float* pSrc, int Xmin, int Ymin, int Xmax, int Ymax, float &k, float &c);

int ComputeIntsections(float3* pDst, float3* temp, int Xmin, int Ymin, int Xmax, int Ymax, float &k, float &c);

const char* Art(float* pDst, int nWidth, int nHeight, float* pPrj, int nRays, int nAngles, float rays_separation, float angles_separation, int nItNum = 10);

const char* ArtRadon(float* pPrj, int nRays, int nAngles, float* pSrc, int nWidth, int nHeight, float rays_separation, float angles_separation);