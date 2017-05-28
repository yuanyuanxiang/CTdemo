#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "IntSection.h"
#include "ImageTransform.h"
#include <algorithm>
#include <limits>
#include <vector>
using namespace std;

#include "templateFuncs.h"

// ������
#define CONVOLUTE_KERNEL_COSINE		0
#define CONVOLUTE_KERNEL_RL			1
#define CONVOLUTE_KERNEL_SL			2
#define CONVOLUTE_KERNEL_HAMMING	3

class CT : public ImageTransform
{
public:

	CT(const float* pSrc, int nWidth, int nHeight) : 
		ImageTransform(pSrc, nWidth, nHeight, nWidth, 1) { }

	// ����˺���
	static float CosineKernel(float x, float w0);

	// R_L������:n-̽������ţ�d-̽�������
	static float R_LKernel(int n, float d);

	// S_L������:n-̽������ţ�d-̽�������
	static float S_LKernel(int n, float d);

	// Hamming������:n-̽������ţ�d-̽�������
	static float HammingKernel(int n, float d, float a);

	// ϣ�����ؾ����
	static float HilbertKernel(float x);

	// ��ȡֱ�ߵ��Ա���
	static float LineGetXValue(float &k, float &c, float y);

	// ��ȡֱ�ߵĺ���ֵ
	static float LineGetYValue(float &k, float &c, float x);

	// ����ͼ��Ŀ����߶ȼ������ѵ����߲����������ο���MATLAB
	static int ComputeRaysNum(int nWidth, int nHeight);

	// ����һά��ֵ:��ֵ��x��ֵ.
	float LinearInterp(const float* pSrc, int nWidth, float x)
	{
		int x1 = floor(x), x2 = x1 + 1;
		if ( x1 < 0 || x2 >= nWidth)
			return 0;
		return pSrc[x1] * (1 - x + x1) + pSrc[x2] * (x - x1);
	}

	// ���Զ�ά��ֵ:��width*height�����в�ֵ��(x, y)��ֵ.
	float LinearInterp(const float* pSrc, int nWidth, int nHeight, int x, float y)
	{
		int h1 = floor(y), h2 = h1 + 1;
		if ( h1 < 0 || h2 >= nHeight)
			return 0;
		return pSrc[x + h1 * nWidth] * (1 - y + h1) + pSrc[x + h2 * nWidth] * (y - h1);
	}

	// ���
	void Convolute(float* pDst, float delta_r, float w0, int nConvKernel);

	// ƽ������ͶӰ
	void BackProject(float* pDst, int nRays, int nAngles, float delta_r, float delta_fai);

	void Convolute(float* pDst, float delta_r, float w0, float R, float D, int nConvKernel);

	void BackProject(float* pDst, int nRays, int nAngles, float delta_r, float delta_fai, float R, float D);

	// ����ͼ��
	float* DiffImage(const float* pPrj, int nRays, int nAngles, float delta_r);

	// ��������ϣ��������任
	void InverseHilbert(float* pDst, float delta_r);

	// �߻���
	float LineIntegrate(const CLogoRect &rect, float &k, float &c);

	// Radon�任
	void ImageRadon(float* pDst, float angles_separation, int nAnglesNum, float pixels_separation, int nRaysNum);

	// DBPͼ��
	void DBPImage(float* pDst, int nRays, int nAngles, float delta_r, float delta_fai, float theta);

	// �ص��������߻��֣����������radon�任
	void ImageIntegrate(float* pDst, float angle, int nLength);
};

#endif
