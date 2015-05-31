#include "stdafx.h"
#include "Art.h"


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
	int n = 0, n1, n2, s = 0;
	for (int i = Xmin; i <= Xmax; ++i)
	{
		float x = i;
		float y = LineGetYValue(k, c, x);
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
			float y = j;
			float x = LineGetXValue(k, c, y);
			if (x < Xmin || x > Xmax)
				continue;
			temp[n++] = make_float3(x, y, 0);
		}
	}
	else
	{
		for (int j = Ymax; j >= Ymin; --j)
		{
			float y = j;
			float x = LineGetXValue(k, c, y);
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

	float sum = 0.f;
	int Width = Xmax - Xmin;
	int Height = Ymax - Ymin;
	for (int i = 0; i < s - 1; ++i)
	{
		int x = pDst[i].x - Xmin;
		int y = pDst[i].y - Ymin;
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
*/
void ComputeIntsections(float3* pDst, float3* temp, int Xmin, int Ymin, int Xmax, int Ymax, float &k, float &c)
{
	int n = 0, n1, n2, s = 0;
	for (int i = Xmin; i <= Xmax; ++i)
	{
		float x = i;
		float y = LineGetYValue(k, c, x);
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
			float y = j;
			float x = LineGetXValue(k, c, y);
			if (x < Xmin || x > Xmax)
				continue;
			temp[n++] = make_float3(x, y, 0);
		}
	}
	else
	{
		for (int j = Ymax; j >= Ymin; --j)
		{
			float y = j;
			float x = LineGetXValue(k, c, y);
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

	float sum = 0.f;
	int Width = Xmax - Xmin;
	int Height = Ymax - Ymin;
	for (int i = 0; i < s - 1; ++i)
	{
		int x = pDst[i].x - Xmin;
		int y = pDst[i].y - Ymin;
		pDst[i].z = Distance(pDst[i], pDst[i + 1]);
	}
}


float Distance(const float3 &a, const float3 &b)
{
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}


const char* Art(float* pDst, int nWidth, int nHeight, float* pPrj, int nRays, int nAngles, float rays_separation, float angles_separation)
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
	catch (const std::bad_alloc &e)
	{
		return "分配内存失败！";
	}
	temp = new float3[Length];
	float med = nRays / 2.f;
	for (int j = 0; j < nRays; ++j)
	{
		float distance = -med + rays_separation * j;
		for (int i = 0; i < nAngles; ++i)
		{
			float angle, k, c;
			angle = angles_separation * i;
			k = tan(angle + PI / 2.f);
			c = distance / sin(angle);
			curIntSecs = IntSecs + (i + j * nAngles) * Length;
			memset(temp, 0, Length * sizeof(float3));
			if (k == 0)
			{
				int n = 0;
				if (Xmin <= distance && distance <= Xmax)
				{
					for (int x = Xmin; x <= Xmax; ++x)
					{
						curIntSecs[n++] = make_float3(x, distance, 1.f);
					}
				}
			}
			else ComputeIntsections(curIntSecs, temp, Xmin, Ymin, Xmax, Ymax, k, c);
		}
	}
	int it_K = 0;
	do 
	{
		for (int j = 0; j < nRays; ++j)
		{
			for (int i = 0; i < nAngles; ++i)
			{
				curIntSecs = IntSecs + (i + j * nAngles) * Length;
				float R_Square = 0.f;
				for (int k = 0; k < Length; ++k)
				{
					R_Square += curIntSecs[k].z * curIntSecs[k].z;
				}
				if (R_Square == 0)
					continue;
				float RX = 0.f;
				for (int k = 0; k < Length; ++k)
				{
					int x = curIntSecs[k].x - Xmin;
					int y = curIntSecs[k].y - Ymin;
					float R = curIntSecs[k].z;
					if (0 <= x && x < nWidth && 0 <= y && y < nHeight)
						RX += R * pDst[x + y * nWidth];
				}
				float Delta = pPrj[i + j * nAngles] - RX;
				for (int k = 0; k < Length; ++k)
				{
					int x = curIntSecs[k].x - Xmin;
					int y = curIntSecs[k].y - Ymin;
					float R = curIntSecs[k].z;
					if (0 <= x && x < nWidth && 0 <= y && y < nHeight)
						pDst[x + y * nWidth] += Delta * R / R_Square;
				}
			}
		}
	} while (it_K++ < 8);
	SAFE_DELETE(temp);
	SAFE_DELETE(IntSecs);
	return str;
}


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
	for (int j = 0; j < nRays; ++j)
	{
		float distance = -med + rays_separation * j;
		for (int i = 0; i < nAngles; ++i)
		{
			float angle, k, c;
			angle = angles_separation * i;
			k = tan(angle + PI / 2.f);
			c = distance / sin(angle);
			memset(temp, 0, Length * sizeof(float3));
			if (k == 0)
			{
				int n = 0;
				if (Xmin <= distance && distance <= Xmax)
				{
					for (int x = Xmin; x < Xmax; ++x)
					{
						int s = x - Xmin;
						int t = distance - Ymin;
						if (0 <= s && s < nWidth && 0 <= t && t < nHeight)
							pPrj[i + j * nAngles] += pSrc[s + t * nWidth];
					}
				}
			}
			else ComputeIntsections(IntSecs, temp, pPrj, j, i, nAngles, pSrc, Xmin, Ymin, Xmax, Ymax, k, c);
		}
	}
	SAFE_DELETE(temp);
	SAFE_DELETE(IntSecs);
	return str;
}