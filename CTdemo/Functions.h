#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "IntSection.h"
#include "ImageTransform.h"
#include <algorithm>
#include <limits>
#include <vector>
using namespace std;

#include "templateFuncs.h"

// 窗函数
#define CONVOLUTE_KERNEL_COSINE		0
#define CONVOLUTE_KERNEL_RL			1
#define CONVOLUTE_KERNEL_SL			2
#define CONVOLUTE_KERNEL_HAMMING	3

class CT : public ImageTransform
{
public:

	CT(const float* pSrc, int nWidth, int nHeight) : 
		ImageTransform(pSrc, nWidth, nHeight, nWidth, 1) { }

	// 卷积核函数
	static float CosineKernel(float x, float w0);

	// R_L窗函数:n-探测器编号；d-探测器间距
	static float R_LKernel(int n, float d);

	// S_L窗函数:n-探测器编号；d-探测器间距
	static float S_LKernel(int n, float d);

	// Hamming窗函数:n-探测器编号；d-探测器间距
	static float HammingKernel(int n, float d, float a);

	// 希尔伯特卷积核
	static float HilbertKernel(float x);

	// 获取直线的自变量
	static float LineGetXValue(float &k, float &c, float y);

	// 获取直线的函数值
	static float LineGetYValue(float &k, float &c, float x);

	// 根据图像的宽度与高度计算出最佳的射线采样条数，参考自MATLAB
	static int ComputeRaysNum(int nWidth, int nHeight);

	// 线性一维插值:插值出x的值.
	float LinearInterp(const float* pSrc, int nWidth, float x)
	{
		int x1 = floor(x), x2 = x1 + 1;
		if ( x1 < 0 || x2 >= nWidth)
			return 0;
		return pSrc[x1] * (1 - x + x1) + pSrc[x2] * (x - x1);
	}

	// 线性二维插值:在width*height网格中插值出(x, y)的值.
	float LinearInterp(const float* pSrc, int nWidth, int nHeight, int x, float y)
	{
		int h1 = floor(y), h2 = h1 + 1;
		if ( h1 < 0 || h2 >= nHeight)
			return 0;
		return pSrc[x + h1 * nWidth] * (1 - y + h1) + pSrc[x + h2 * nWidth] * (y - h1);
	}

	// 卷积
	void Convolute(float* pDst, float delta_r, float w0, int nConvKernel);

	// 平行束反投影
	void BackProject(float* pDst, int nRays, int nAngles, float delta_r, float delta_fai);

	void Convolute(float* pDst, float delta_r, float w0, float R, float D, int nConvKernel);

	void BackProject(float* pDst, int nRays, int nAngles, float delta_r, float delta_fai, float R, float D);

	// 导数图像
	float* DiffImage(const float* pPrj, int nRays, int nAngles, float delta_r);

	// 有限区域希尔伯特逆变换
	void InverseHilbert(float* pDst, float delta_r);

	// 线积分
	float LineIntegrate(const CLogoRect &rect, float &k, float &c);

	// Radon变换
	void ImageRadon(float* pDst, float angles_separation, int nAnglesNum, float pixels_separation, int nRaysNum);

	// DBP图像
	void DBPImage(float* pDst, int nRays, int nAngles, float delta_r, float delta_fai, float theta);

	// 沿单个方向线积分，单个方向的radon变换
	void ImageIntegrate(float* pDst, float angle, int nLength);
};

#endif
