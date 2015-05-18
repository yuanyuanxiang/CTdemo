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

__host__ const char* cudaConvolute(float* h_pDst, float* h_prj, int row, int col, float delta_r, float w0);

__device__ float cudaConvKernel(float x, float w0);

__device__ float cudaRLFilter(int n, float d);

__global__ void cudaInitConvolute(float* d_pConv, int row, int col, float w0);

__global__ void cudaRowMajor2ColMajor(float *d_pSrc, float *d_temp, int row, int col);

__global__ void cudaColMajor2RowMajor(float *d_pSrc, float *d_temp, int row, int col);