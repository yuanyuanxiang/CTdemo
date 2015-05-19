// System includes
#include <stdio.h>
#include <assert.h>

// CUDA runtime
#include <cuda_runtime.h>

// CUDA helper functions
#include <helper_cuda.h>         // helper functions for CUDA error check

// Helper functions and utilities to work with CUDA
#include <helper_functions.h>

#include "cuda_runtime_api.h"

#define PI 3.141592653f

#define RAD(i) (PI * (i) / 180.0f)	//角度转弧度

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)	//计算每行字节数

__global__ void cudaVectorAssigned(float* vec, int len, float val = 0);

__global__ void cudaCopyColumnData(float *d_pDst, int RaysNum, int AnglesNum, float *d_pVec, int ImageDiag, int column, int HalfRays, int HalfDiag, float Separation);

__global__ void cudaRotateKernel(float* d_pDst, BYTE* d_pSrc, int Width, int Height, int Rowlen, int NewWidth, int NewHeight, int Xmin, int Ymin, float cos_theta, float sin_theta);

__global__ void cudaCopyTempData(float *d_pDst, float *d_pSrc, int Width, int Height, int nHalfDiag, int nHalfWidth, float fPixelDistance);

__host__ const char* cudaRadon(float* h_pDst, int RaysNum, int AnglesNum, float rays_separation, float angle_separation, BYTE* h_pSrc, int Width, int Height, int Rowlen, float fSubPixel);

__device__ float cudaGetPositionPixel(float x, float y, BYTE* h_pSrc, int Width, int Height, int Rowlen);

__device__ float cudaGetImagePixel(int x, int y, BYTE* h_pSrc, int Width, int Height, int Rowlen);

__device__ float cudaGetIndexValue(int x, int y, float* pSrc, int Width, int Height);

__device__ float cudaGetPositionValue(float x, float y, float* pSrc, int Width, int Height);

__device__ void cudaPositonTransform(float &x, float &y, float theta);

__device__ void cudaPositonTransform(float &x, float &y, float cos_theta, float sin_theta);

__host__ float* cudaImageZoom(float* d_pDst, BYTE* d_pSrc, int NewWidth, int NewHeight, int Width, int Height, int Rowlen);

__global__ void cudaZoomKernel(float* d_pDst, BYTE* d_pSrc, float wRatio, float hRatio, int NewWidth, int NewHeight, int Width, int Height, int Rowlen);

__host__ void PositionTransform(float &x, float &y, float theta);

__host__ void PositionTransform(float &x, float &y, float cos_theta, float sin_theta);

__host__ float FindMaxBetween4Numbers(float x, float y, float z, float w);

__host__ float FindMinBetween4Numbers(float x, float y, float z, float w);
