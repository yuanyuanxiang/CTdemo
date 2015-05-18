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

__device__ float cudaLinearInterp(float r_id, int fai_id, float* prj, int angles, int rays);

__host__ const char* cudaBackProject(float* pDst, float* prj, int width, int height, int rays, int angles,  float delta_r, float delta_fai);

__global__ void cudaBackProjectKernel(float* d_pDst, float* d_prj, int width, int height, int rays, int angles, float delta_r, float delta_fai, int cx, int cy, int med);