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

#include "..\IntSection.h"
#include <iostream>
#include <vector>
using namespace std;

#define SAFE_DELETE(p) { if(p != NULL){ delete [] p; p = NULL; } }	//��ȫɾ��ָ��p

/* CUDA��Radon�任.*/
__host__ const char* cudaPanRadon(float* h_pSrc, int src_width, int src_height, float* h_pDst, int pan_angles, int pan_rays, float scan_range, float R, float D);

/* ������ת֮�������.*/
float* cudaComputeRotatedValues(float* pSrc, int src_width, int src_height, float cos_theta, float sin_theta, int &Xmin, int &Ymin, int &Xmax, int &Ymax, int &NewWidth, int &NewHeight);

/* ���ֱ�ߵĺ���ֵ.*/
__device__ __host__ float cudaLineGetXValue(float &k, float &c, float y);

/* ���ֱ�ߵ��Ա���.*/
__device__ __host__ float cudaLineGetYValue(float &k, float &c, float x);

/* ͼ����ĳ�����߻���.*/
__device__ __host__ void cudaIntsections(float2* pDst, float2* temp, float* pPrj, int pan_rays, int pan_angles, int line_index, int angles_index, 
								float* pSrc, int Xmin, int Ymin, int Xmax, int Ymax, int Width, int Height, int intsection_length, float &k, float &c);
/* �����߻���ֵ*/
__device__ void cudaIntergrate(float2* Intsections, float *pDst, float *pSrc, int pan_angles, int intsection_length, int angles_index, int line_index, int Xmin, int Ymin, int Width, int Height);

/* ������ԭ����ת�任.*/
__host__ void PositionTransform(float &x, float &y, float cos_theta, float sin_theta);

/* �����ƹ̶�����ת�任.*/
__host__ void PositionTransform(float &x, float &y, float cos_theta, float sin_theta, float x0, float y0);

/* ������ԭ����ת�任.*/
__device__ void _cudaPositionTransform(float &x, float &y, float cos_theta, float sin_theta);

/* �����ƹ̶�����ת�任.*/
__device__ void _cudaPositionTransform(float &x, float &y, float cos_theta, float sin_theta, float x0, float y0);

/* ͼ��˫���Բ�ֵ.*/
__device__ float cudaBiLinearInterp(float *pSrc, int row, int col, float x, float y);

/* ��ȡͼ��ĳ��ĳ�е�ֵ.*/
__device__ float cudaGetPositionValue(float *pSrc, int row, int col, int x, int y);

/* ���ĸ����ֻ�ȡ���ֵ.*/
float FindMaxBetween4Numbers(float x, float y, float z, float w);

/* ���ĸ����ֻ�ȡ��Сֵ.*/
float FindMinBetween4Numbers(float x, float y, float z, float w);

/* float2 �������*/
__device__ __host__ float cudaDistance(const float2 &a, const float2 &b);