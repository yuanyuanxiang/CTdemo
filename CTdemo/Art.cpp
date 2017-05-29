#include "stdafx.h"
#include "Art.h"
#include "Functions.h"

// 2016-12-4增加，保证在不安装CUDA的条件下仍可编译成功
#ifndef CUDA

float3 make_float3(float x0, float y0, float z0)
{
	return float3(x0, y0, z0);
}

#endif
/*
float3* pDst							存放交线长度的数组，x、y存放像素坐标，z存放交线长度
float3* temp							大小和pDst一样，是中间变量
float* pPrj								图像投影
int nRaysIndex							射线编号
int nAnglesIndex						角度编号
int nAngles								角度数目
float* pSrc								图像数据
int Xmin, int Ymin, int Xmax, int Ymax	图像的左下角和右上角坐标
float &k, float &c						直线斜率与截距
*/
void ComputeIntsections(float3* pDst, float3* temp, float* pPrj, int nRaysIndex, int nAnglesIndex, int nAngles, float* pSrc, int Xmin, int Ymin, int Xmax, int Ymax, float &k, float &c)
{
	int s = ComputeIntsections(pDst, temp, Xmin, Ymin, Xmax, Ymax, k, c);
	float sum = 0;
	int Width = Xmax - Xmin;
	int Height = Ymax - Ymin;
	for (int i = 0; i < s - 1; ++i)
	{
		int x = (int)pDst[i].x - Xmin;
		int y = (int)pDst[i].y - Ymin;
		pDst[i].z = Distance(pDst[i], pDst[i + 1]);
		if (0 <= x && x < Width && 0 <= y && y < Height)
			sum += pDst[i].z * pSrc[x + y * Width];
	}
	pPrj[nAnglesIndex + nRaysIndex * nAngles] = sum;
}


/*
float3* pDst							存放交线长度的数组，x、y存放像素坐标，z存放交线长度
float3* temp							大小和pDst一样，是中间变量
int Xmin, int Ymin, int Xmax, int Ymax	图像的左下角和右上角坐标
float &k, float &c						直线斜率与截距
返回交点个数。
*/
int ComputeIntsections(float3* pDst, float3* temp, int Xmin, int Ymin, int Xmax, int Ymax, float &k, float &c)
{
	int n = 0, n1, n2, s = 0;
	for (int i = Xmin; i <= Xmax; ++i)
	{
		float x = (float)i;
		float y = CT::LineGetYValue(k, c, x);
		if (y < Ymin || y > Ymax)
			continue;
		temp[n++] = make_float3(x, y, 0);
	}
	n1 = n;
	// 为了让数组按x升序排列，必须分类讨论
	if (k >= 0)
	{
		for (int j = Ymin; j <= Ymax; ++j)
		{
			float y = (float)j;
			float x = CT::LineGetXValue(k, c, y);
			if (x < Xmin || x > Xmax)
				continue;
			temp[n++] = make_float3(x, y, 0);
		}
	}
	else
	{
		for (int j = Ymax; j >= Ymin; --j)
		{
			float y = (float)j;
			float x = CT::LineGetXValue(k, c, y);
			if (x < Xmin || x > Xmax)
				continue;
			temp[n++] = make_float3(x, y, 0);
		}
	}
	n2 = n - n1;
	// 对两个有序数组进行排序
	if (n1 >= n2)
	{
		for (int i = 0; i < n2; ++i)
		{
			if (temp[i].x < temp[n1 + i].x)
			{
				pDst[s++] = temp[i];
			}
			else
			{
				pDst[s++] = temp[n1 + i];
			}
		}
		for (int i = n2; i < n1; ++i)
		{
			pDst[s++] = temp[i];
		}
	}
	else
	{
		for (int i = 0; i < n1; ++i)
		{
			if (temp[i].x < temp[n1 + i].x)
			{
				pDst[s++] = temp[i];
			}
			else
			{
				pDst[s++] = temp[n1 + i];
			}
		}
		for (int i = n1; i < n2; ++i)
		{
			pDst[s++] = temp[n1 + i];
		}
	}

	float sum = 0;
	int Width = Xmax - Xmin;
	int Height = Ymax - Ymin;
	for (int i = 0; i < s - 1; ++i)
	{
		pDst[i].z = Distance(pDst[i], pDst[i + 1]);
	}
	return s;
}


/*
float* pDst, int nWidth, int nHeight			重建图像及宽高
float* pPrj, int nRays, int nAngles				投影数据
float rays_separation, float angles_separation	采样间距
int nItNum										迭代次数
*/
const char* Art(float* pDst, int nWidth, int nHeight, float* pPrj, int nRays, int nAngles, float rays_separation, float angles_separation, int nItNum)
{
	const char* str = NULL;
	float Xmin, Ymin, Xmax, Ymax;
	Xmin = -nWidth / 2.f;
	Ymin = -nHeight / 2.f;
	Xmax = -Xmin;
	Ymax = -Ymin;
	int Length = nWidth + nHeight;
	float3 *IntSecs = NULL, *curIntSecs = NULL, *temp = NULL;
	try
	{
		IntSecs = new float3[nRays * nAngles * Length];
		memset(IntSecs, 0, nRays * nAngles * Length * sizeof(float3));
	}
	catch (const std::bad_alloc )
	{
		return "分配内存失败！";
	}
	temp = new float3[Length];
	float med = nRays / 2.f;
	for (int j = 0; j < nRays; ++j)
	{
		float distance = -med + rays_separation * j;
		int n = 0;
		curIntSecs = IntSecs + j * nAngles * Length;
		if (Xmin <= distance && distance <= Xmax)
		{
			for (float y = Ymin; y <= Ymax; ++y)
			{
				curIntSecs[n++] = make_float3(distance, y, 1.f);
			}
		}
	}
	for (int j = 0; j < nRays; ++j)
	{
		float distance = -med + rays_separation * j;
		for (int i = 1; i < nAngles; ++i)
		{
			float angle, k, c;
			angle = angles_separation * i;
			k = tan(angle + PI / 2.f);
			c = distance / sin(angle);
			curIntSecs = IntSecs + (i + j * nAngles) * Length;
			memset(temp, 0, Length * sizeof(float3));
			ComputeIntsections(curIntSecs, temp, Xmin, Ymin, Xmax, Ymax, k, c);
		}
	}
	// 迭代
	int it_K = 0;
	float* R_Square = NULL;
	try
	{
		R_Square = new float[nRays * nAngles];
		memset(R_Square, 0, nRays * nAngles * sizeof(float));
	}
	catch (const std::bad_alloc )
	{
		SAFE_DELETE(temp);
		SAFE_DELETE(IntSecs);
		return "分配内存失败！";
	}
	// 计算|R|*|R|
	for (int j = 0; j < nRays; ++j)
	{
		for (int i = 0; i < nAngles; ++i)
		{
			curIntSecs = IntSecs + (i + j * nAngles) * Length;
			for (int k = 0; k < Length; ++k)
			{
				R_Square[i + j * nAngles] += curIntSecs[k].z * curIntSecs[k].z;
			}
		}
	}
	// 当前像素和交线长
	int* x = new int[Length];
	int* y = new int[Length];
	float* R = new float[Length];
	bool* judge = new bool[Length];
	do 
	{
		for (int j = 0; j < nRays; ++j)
		{
			for (int i = 0; i < nAngles; ++i)
			{
				curIntSecs = IntSecs + (i + j * nAngles) * Length;
				if (R_Square[i + j * nAngles] == 0)
					continue;

				for (int k = 0; k < Length; ++k)
				{
					x[k] = int(curIntSecs[k].x - Xmin);
					y[k] = int(curIntSecs[k].y - Ymin);
					R[k] = curIntSecs[k].z;
					judge[k] = (0 <= x[k] && x[k] < nWidth && 0 <= y[k] && y[k] < nHeight);
				}

				float RX = 0;
				for (int k = 0; k < Length; ++k)
				{
					if (judge[k])
						RX += R[k] * pDst[x[k] + y[k] * nWidth];
				}

				float Delta = pPrj[i + j * nAngles] - RX;
				float rate = Delta / R_Square[i + j * nAngles];
				for (int k = 0; k < Length; ++k)
				{
					if (judge[k])
						pDst[x[k] + y[k] * nWidth] += rate * R[k];
				}
			}
		}
	} while (it_K++ < nItNum);
	SAFE_DELETE(x);
	SAFE_DELETE(y);
	SAFE_DELETE(R);
	SAFE_DELETE(judge);
	SAFE_DELETE(R_Square);
	SAFE_DELETE(temp);
	SAFE_DELETE(IntSecs);
	return str;
}


/*
float* pPrj, int nRays, int nAngles				投影数据
float* pSrc, int nWidth, int nHeight			原始图像
float rays_separation, float angles_separation	采样间距
*/
const char* ArtRadon(float* pPrj, int nRays, int nAngles, float* pSrc, int nWidth, int nHeight, float rays_separation, float angles_separation)
{
	const char* str = NULL;
	float Xmin, Ymin, Xmax, Ymax;
	Xmin = -nWidth / 2.f;
	Ymin = -nHeight / 2.f;
	Xmax = -Xmin;
	Ymax = -Ymin;
	int Length = nWidth + nHeight;
	float3 *IntSecs = NULL, *temp = NULL;
	IntSecs = new float3[Length];
	temp = new float3[Length];
	float med = nRays / 2.f;
	ZeroMemory(pPrj, nRays * nAngles * sizeof(float));
	// 当法向角度是0时
	for (int j = 0; j < nRays; ++j)
	{
		float distance = -med + rays_separation * j;
		for (float i = Ymin; i < Ymax; ++i)
		{
			int x = int(nWidth / 2.f + distance);
			int y = int(i - Ymin);
			if (0 <= x && x < nWidth && 0 <= y && y < nHeight)
				pPrj[j * nAngles] += pSrc[x + y * nWidth];
		}
	}
	for (int j = 0; j < nRays; ++j)
	{
		float distance = -med + rays_separation * j;
		for (int i = 1; i < nAngles; ++i)
		{
			float angle, k, c;
			angle = angles_separation * i;
			k = tan(angle + PI / 2.f);
			c = distance / sin(angle);
			memset(IntSecs, 0, Length * sizeof(float3));
			memset(temp, 0, Length * sizeof(float3));
			ComputeIntsections(IntSecs, temp, pPrj, j, i, nAngles, pSrc, Xmin, Ymin, Xmax, Ymax, k, c);
		}
	}
	SAFE_DELETE(temp);
	SAFE_DELETE(IntSecs);
	return str;
}