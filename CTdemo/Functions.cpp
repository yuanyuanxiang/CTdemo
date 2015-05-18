#include "stdafx.h"
#include "Functions.h"
#include "IntSection.h"
#include <vector>
#include <algorithm>
using namespace std;

// 不显示数据截断警告
#pragma warning(disable: 4244)

float GetPositionValue(float *pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, int nCurChannel, int x, int y)
{
	if (x < 0 || x >= nWidth || y < 0 || y >= nHeight)
		return 0;
	return (float)*(pSrc + nCurChannel + x * nChannel + y * nRowlen);
}


float biLinearInterp(float *pSrc, int &nWidth, int &nHegiht, int &nRowlen, int &nChannel, int nCurChannel, float x, float y)
{
	int x1, x2, x3, x4, y1, y2, y3, y4;
	float Ans1, Ans2;
	x1 = floor(x);	y1 = floor(y);
	x2 = x1 + 1;	y2 = y1;
	x3 = x2;		y3 = y1 + 1;
	x4 = x1;		y4 = y3;
	Ans1 = GetPositionValue(pSrc, nWidth, nHegiht, nRowlen, nChannel, nCurChannel, x1, y1) * (1 - x + x1) 
		+ GetPositionValue(pSrc, nWidth, nHegiht, nRowlen, nChannel, nCurChannel, x2, y2) * (x - x1);
	Ans2 = GetPositionValue(pSrc, nWidth, nHegiht, nRowlen, nChannel, nCurChannel, x4, y4) * (1 - x + x4) 
		+ GetPositionValue(pSrc, nWidth, nHegiht, nRowlen, nChannel, nCurChannel, x3, y3) * (x - x4);
	return (Ans1 * (1 - y + y1) + Ans2 * (y - y1));
}


void PositionTransform(float &x, float &y, float theta, float x0, float y0)
{
	float delta_x = x - x0;
	float delta_y = y - y0;
	PositionTransform(delta_x, delta_y, theta);
	x = x0 + delta_x;
	y = y0 + delta_y;
}


void PositionTransform(float &x, float &y, float theta)
{
	float cos_theta = cos(theta);
	float sin_theta = sin(theta);
	float x_temp = x * cos_theta - y * sin_theta;
	y = x * sin_theta + y * cos_theta;
	x = x_temp;
}


float FindMaxBetween4Numbers(float x, float y, float z, float w)
{
	float Max1, Max2;
	Max1 = x > y ? x : y;
	Max2 = z > w ? z : w;
	return (Max1 > Max2 ? Max1 : Max2);
}


float FindMinBetween4Numbers(float x, float y, float z, float w)
{
	float Min1, Min2;
	Min1 = x < y ? x : y;
	Min2 = z < w ? z : w;
	return (Min1 < Min2 ? Min1 : Min2);
}


/*
float* pDst					目标
float* pSrc					指向投影数据的指针
int nWidth					宽度，此处表示fai
int nHeight					高度，此处表示r
float delta_r				离散化之后的dr
float w0					高频截止
*/
void Convolute(float* pDst, float* pSrc, int nWidth, int nHeight, float delta_r, float w0)
{
	int m, n, i;
	float sum = 0.f;

#pragma omp parallel for private(m, n, i) reduction(+ : sum)
	for (m = 0; m < nHeight; ++m)
	{
		for (n = 0; n < nWidth; ++n)
		{
			sum = 0.f;
			for (i = 0; i < nHeight; ++i)
				sum += pSrc[n + i * nWidth] * ConvKernel(m - i, w0);
			pDst[n + m * nWidth] = sum;
		}
	}
}


// 卷积核：HW(r). w0 - 高频截止
float ConvKernel(float x, float w0)
{
	if(x == 0) return w0 * w0;
	float theta = 2 * PI * w0 * x;
	return (-1 + cos(theta) + theta * sin(theta)) / (2 * PI * PI * x * x);
}


/*
float* pDst			目标
float* pPrj			卷积后的投影
int nWidth			重建图像的宽
int nHeight			重建图像的高
int nRays			投影的行数(r)
int nAngles			投影的列数(fai)
float delta_r		扫描参数
float delta_fai		扫描参数
*/
void BackProject(float* pDst, float* pPrj, int nWidth, int nHeight, int nRays, int nAngles, float delta_r, float delta_fai)
{
	int m, n, i;
	int cx = (nWidth + 1) / 2;
	int cy = (nHeight + 1) / 2;
	int med = (nRays + 1) / 2;
	float sum = 0.f;

#pragma omp parallel for private(m, n, i) reduction(+ : sum)
	for (m = 0; m < nHeight; ++m)
	{
		for (n = 0; n < nWidth; ++n)
		{
			sum = 0.f;
			for (i = 0; i < nAngles; ++i)
			{
				float fai = i * delta_fai;
				float r = (n - cx) * cos(fai) + (m - cy) * sin(fai);
				sum += LinearInterp(pPrj, nAngles, nRays, i, med + r);
			}
			pDst[n + m * nWidth] = sum * delta_fai;
		}
	}
}


// 线性插值:在width*height网格中插值出(x, y)的值.
float LinearInterp(float* pPrj, int nWidth, int nHeight, int x, float y)
{
	int h1 = floor(y), h2 = h1 + 1;
	if ( h1 < 0 || h2 >= nHeight)
		return 0.f;
	return pPrj[x + h1 * nWidth] * (1 - y + h1) + pPrj[x + h2 * nWidth] * (y - h1);
}


void Convolute(float* pDst, float* pSrc, int nWidth, int nHeight, float delta_r, float w0, float R, float D)
{
	int m, n, i;
	float sum = 0.f;
	float theta_0 = acos(nHeight * delta_r / 2.f / R);
	float m_fPan_u0 = D / tan(theta_0);
	float m_fPan_delta_u = (2.f * m_fPan_u0) / nHeight;

#pragma omp parallel for private(m, n, i) reduction(+ : sum)
	for (m = 0; m < nHeight; ++m)
	{
		for (n = 0; n < nWidth; ++n)
		{
			sum = 0.f;
			for (i = 0; i < nHeight; ++i)
			{
				// 加权做卷积
				float u = -m_fPan_u0 + i * m_fPan_delta_u;
				float alpha = D / sqrt(D * D + u * u);
				sum += alpha * pSrc[n + i * nWidth] * ConvKernel(m - i, w0);
			}
			pDst[n + m * nWidth] = sum;
		}
	}
}


void BackProject(float* pDst, float* pPrj, int nWidth, int nHeight, int nRays, int nAngles, float delta_r, float delta_fai, float R, float D)
{
	int m, n, i;
	int cx = (nWidth + 1) / 2;
	int cy = (nHeight + 1) / 2;
	int med = (nRays + 1) / 2;
	float sum = 0.f;

	float theta_0 = acos(nRays * delta_r / 2.f / R);
	float m_fPan_u0 = D / tan(theta_0);
	float m_fPan_delta_u = (2.f * m_fPan_u0) / nRays;
	float delta_u = m_fPan_delta_u;
	delta_r = R / D * delta_u;

#pragma omp parallel for private(m, n, i) reduction(+ : sum)
	for (m = 0; m < nHeight; ++m)
	{
		for (n = 0; n < nWidth; ++n)
		{
			sum = 0.f;
			for (i = 0; i < nAngles; ++i)
			{
				float x1 = n - cx;
				float x2 = m - cy;
				float fai = i * delta_fai;
				float cos_fai = cos(fai);
				float sin_fai = sin(fai);
				float r = x1 * cos_fai + x2 * sin_fai;
				float alpha = R * D / ((R - x2 * cos_fai + x1 * sin_fai) * (R - x2 * cos_fai + x1 * sin_fai));
				sum += alpha * LinearInterp(pPrj, nAngles, nRays, i, med + r / delta_r);
			}
			pDst[n + m * nWidth] = sum * delta_fai;
		}
	}
}


void SetColorTabFor8BitImage(CImage *pImage)
{
	RGBQUAD ColorTab[256];
	for(int i = 0; i < 256; i++)
	{
		ColorTab[i].rgbBlue = ColorTab[i].rgbGreen = ColorTab[i].rgbRed = i;
	}
	pImage->SetColorTable(0, 256, ColorTab);
}


// 将pSrc指针指向的数据写入文件[path]，width和height指定了宽与高.
bool Write2File(float* pSrc, int nWidth, int nHeight, CString path)
{
	ofstream InputFile(path);
	if(InputFile)
	{
		for (int i = 0; i < nHeight; i++)
		{
			for (int j = 0; j < nWidth; j++)
			{
				InputFile << pSrc[j + i * nWidth] << ",";
			}
			InputFile << endl;
		}
		InputFile.close();
		return true;
	}
	return false;
}


/*
float &k		直线斜率
float &c		直线截距
float y			已知y值
*/
float LineGetXValue(float &k, float &c, float y)
{
	ASSERT(k != 0);
	return (y - c) / k;
}


/*
float &k		直线斜率
float &c		直线截距
float x			已知x值
*/
float LineGetYValue(float &k, float &c, float x)
{
	return (k * x + c);
}


/*
float* pSrc		图像数据
int Xmin		左下角x坐标
int Ymin		左下角y坐标
int Xmax		右上角x坐标
int Ymax		右上角y坐标
int &Width		图像宽度
int &Height		图像高度
int &Rowlen		每行浮点数
int CurChannel	当前通道
int &Channel	通道总数
float &k		直线斜率
float &c		直接与y轴截距
*/
float LineIntegrate(float* pSrc, int &Width, int &Height, int &Rowlen, int &Channel, int CurChannel, int Xmin, int Ymin, int Xmax, int Ymax, float &k, float &c)
{
	vector <CIntSection> Sections;

	for (int i = Xmin; i <= Xmax; ++i)
	{
		float x = i;
		float y = LineGetYValue(k, c, x);
		if (y < Ymin || y > Ymax)
			continue;
		Sections.push_back(CIntSection(x, y));
	}

	for (int j = Ymin; j <= Ymax; ++j)
	{
		float y = j;
		float x = LineGetXValue(k, c, y);
		if (x < Xmin || x > Xmax)
			continue;
		Sections.push_back(CIntSection(x, y));
	}

	// 排序
	sort(Sections.begin(), Sections.end());

	float s = 0.f;
	int Num = Sections.size() - 1;
	for (int i = 0; i < Num; ++i)
	{
		int x = Sections[i].m_xPos - Xmin;
		int y = Sections[i].m_yPos - Ymin;
		if (0 <= x && x < Width && 0 <= y && y < Height)
			s += Distance(Sections[i], Sections[i + 1]) * pSrc[CurChannel + x * Channel + y * Rowlen];
	}
	return s;
}


void ImageRadon(float* pDst, float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, int nCurChannel, float angles_separation, int nAnglesNum, float rays_separation, int nRaysNum)
{
	float zoom_rate = 1 / rays_separation;
	int nNewWidth = zoom_rate * nWidth;
	int nNewHeight = zoom_rate * nHeight;
	int nNewRowlen = nNewWidth * nChannel;
	float *pSrc2 = ImageZoom(pSrc, nWidth, nHeight, nRowlen, nChannel, nNewWidth, nNewHeight);

	int m_nImageDiag = (int)ceil(sqrt(1.0f * nWidth * nWidth + nHeight * nHeight));
	int nDetectorCenter = (nRaysNum + 1) / 2;
	int nHalfRays = (nRaysNum + 1) / 2;

#pragma omp parallel for
	for (int i = 0; i < nAnglesNum; ++i)
	{
		float *temp = new float[nRaysNum];
		memset(temp, 0, nRaysNum * sizeof(float));
		float angle = i * angles_separation;
		ImageIntegrate(temp, nRaysNum, pSrc2, nNewWidth, nNewHeight, nNewRowlen, nChannel, nCurChannel, angle, rays_separation);
		
#pragma omp parallel for
		for (int j = 0; j < nRaysNum; ++j)
		{
			int num = (j - nHalfRays) + nDetectorCenter;
			if(0 <= num && num < nRaysNum)
			{
				pDst[i + j * nAnglesNum] = temp[num];
			}
		}
		SAFE_DELETE(temp);
	}

	SAFE_DELETE(pSrc2);
}


// 沿单个方向线积分，单个方向的radon变换
void ImageIntegrate(float* pDst, int nLength, float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, int nCurChannel, float angle, float sub_pixel)
{
	int NewWidth = 0, NewHeight = 0, NewRowlen;
	int Xmin, Ymin, Xmax, Ymax;
	float *pBits = ImageRotate(pSrc, nWidth, nHeight, nRowlen, nChannel, angle, 0, 0, Xmin, Ymin, Xmax, Ymax, NewWidth, NewHeight, NewRowlen);
	float* add_width = new float[NewWidth];
	memset(add_width, 0, NewWidth * sizeof(float));

	float sum = 0.f;
	int nNewRowlen = NewWidth * nChannel;

#pragma omp parallel for reduction(+ : sum)
	for (int i = 0; i < NewWidth; ++i)
	{
		sum = 0.f;
		for (int j = 0; j < NewHeight; ++j)
		{
			sum += pBits[nCurChannel + i * nChannel + j * nNewRowlen];
		}
		add_width[i] = sum * sub_pixel;
	}

	int nDetectorCenter = (nLength + 1) / 2;
	int nHalfWidth = (NewWidth + 1) / 2;

#pragma omp parallel for
	for (int i = 0; i < nLength; ++i)
	{
		int s = (i - nDetectorCenter) / sub_pixel + nHalfWidth;
		if (s >= 0 && s < NewWidth)
			pDst[i] = add_width[s];
	}
	SAFE_DELETE(add_width);
	SAFE_DELETE(pBits);
}


float* ImageRotate(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, float angle, int &NewWidth, int &NewHeight, int &NewRowlen)
{
	// 原始图像四个顶点的坐标
	float x1, x2, x3, x4, y1, y2, y3, y4;
	x1 = 0;							y1 = 0;
	x2 = float(nWidth - 1);			y2 = 0;
	x3 = x2;						y3 = float(nHeight - 1);
	x4 = x1;						y4 = y3;
	// 四个顶点顺时针旋转,绕图像左下角
	PositionTransform(x1, y1, angle);
	PositionTransform(x2, y2, angle);
	PositionTransform(x3, y3, angle);
	PositionTransform(x4, y4, angle);
	int Xmin, Xmax, Ymin, Ymax;
	Xmax = int(FindMaxBetween4Numbers(x1, x2, x3, x4));
	Ymax = int(FindMaxBetween4Numbers(y1, y2, y3, y4));
	Xmin = int(FindMinBetween4Numbers(x1, x2, x3, x4));
	Ymin = int(FindMinBetween4Numbers(y1, y2, y3, y4));
	// 新图像宽度、高度、每行字节数的变化
	NewWidth = Xmax - Xmin + 1;
	NewHeight = Ymax - Ymin + 1;
	NewRowlen = nChannel * NewWidth;
	float *pDst = new float[NewRowlen * NewHeight];
	memset(pDst, 0, NewRowlen * NewHeight * sizeof(float));

#pragma omp parallel for
	for (int i = 0; i < NewWidth; ++i)
	{
		for (int j = 0; j < NewHeight; ++j)
		{
			float x = float(i + Xmin);
			float y = float(j + Ymin);
			PositionTransform(x, y, -angle);
			for (int nCurChannel = 0; nCurChannel < nChannel; ++nCurChannel)
				pDst[nCurChannel + i * nChannel + j * NewRowlen] = GetPositionValue(pSrc, nWidth, nHeight, nRowlen, nChannel, nCurChannel, x, y);
		}
	}

	return pDst;
}


float* ImageRotate(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, float angle, float x0, float y0, int &NewWidth, int &NewHeight, int &NewRowlen)
{
	// 原始图像四个顶点的坐标
	float x1, x2, x3, x4, y1, y2, y3, y4;
	x1 = 0;							y1 = 0;
	x2 = float(nWidth - 1);			y2 = 0;
	x3 = x2;						y3 = float(nHeight - 1);
	x4 = x1;						y4 = y3;
	// 四个顶点顺时针旋转,绕图像中心
	PositionTransform(x1, y1, angle, x0, y0);
	PositionTransform(x2, y2, angle, x0, y0);
	PositionTransform(x3, y3, angle, x0, y0);
	PositionTransform(x4, y4, angle, x0, y0);
	int Xmin, Xmax, Ymin, Ymax;
	Xmax = int(FindMaxBetween4Numbers(x1, x2, x3, x4));
	Ymax = int(FindMaxBetween4Numbers(y1, y2, y3, y4));
	Xmin = int(FindMinBetween4Numbers(x1, x2, x3, x4));
	Ymin = int(FindMinBetween4Numbers(y1, y2, y3, y4));
	// 新图像宽度、高度、每行字节数的变化
	NewWidth = Xmax - Xmin + 1;
	NewHeight = Ymax - Ymin + 1;
	NewRowlen = nChannel * NewWidth;
	float *pDst = new float[NewRowlen * NewHeight];
	memset(pDst, 0, NewRowlen * NewHeight * sizeof(float));

#pragma omp parallel for
	for (int i = 0; i < NewWidth; ++i)
	{
		for (int j = 0; j < NewHeight; ++j)
		{
			float x = float(i + Xmin);
			float y = float(j + Ymin);
			PositionTransform(x, y, -angle, x0, y0);
			for (int nCurChannel = 0; nCurChannel < nChannel; ++nCurChannel)
				pDst[nCurChannel + i * nChannel + j * NewRowlen] = GetPositionValue(pSrc, nWidth, nHeight, nRowlen, nChannel, nCurChannel, x, y);
		}
	}

	return pDst;
}


float* ImageRotate(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, float angle, float x0, float y0, int &Xmin, int &Ymin, int &Xmax, int &Ymax, int &NewWidth, int &NewHeight, int &NewRowlen)
{
	// 原始图像四个顶点的坐标
	float x1, x2, x3, x4, y1, y2, y3, y4;
	x1 = 0;							y1 = 0;
	x2 = float(nWidth - 1);			y2 = 0;
	x3 = x2;						y3 = float(nHeight - 1);
	x4 = x1;						y4 = y3;
	// 四个顶点顺时针旋转,绕图像中心
	PositionTransform(x1, y1, angle, x0, y0);
	PositionTransform(x2, y2, angle, x0, y0);
	PositionTransform(x3, y3, angle, x0, y0);
	PositionTransform(x4, y4, angle, x0, y0);
	Xmax = int(FindMaxBetween4Numbers(x1, x2, x3, x4));
	Ymax = int(FindMaxBetween4Numbers(y1, y2, y3, y4));
	Xmin = int(FindMinBetween4Numbers(x1, x2, x3, x4));
	Ymin = int(FindMinBetween4Numbers(y1, y2, y3, y4));
	// 新图像宽度、高度、每行字节数的变化
	NewWidth = Xmax - Xmin + 1;
	NewHeight = Ymax - Ymin + 1;
	NewRowlen = nChannel * NewWidth;
	float *pDst = new float[NewRowlen * NewHeight];
	memset(pDst, 0, NewRowlen * NewHeight * sizeof(float));

#pragma omp parallel for
	for (int i = 0; i < NewWidth; ++i)
	{
		for (int j = 0; j < NewHeight; ++j)
		{
			float x = float(i + Xmin);
			float y = float(j + Ymin);
			PositionTransform(x, y, -angle, x0, y0);
			for (int nCurChannel = 0; nCurChannel < nChannel; ++nCurChannel)
				pDst[nCurChannel + i * nChannel + j * NewRowlen] = GetPositionValue(pSrc, nWidth, nHeight, nRowlen, nChannel, nCurChannel, x, y);
		}
	}

	return pDst;
}


float* ImageZoom(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, int NewWidth, int NewHeight)
{
	int NewRowlen = nChannel * NewHeight;
	float* pDst = new float[NewRowlen * NewHeight];
	float wRatio = 1.f * nWidth / NewWidth;
	float hRatio = 1.f * nHeight / NewHeight;

#pragma omp parallel for
	for (int i = 0; i < NewWidth; ++i)
	{
		for (int j = 0; j < NewHeight; ++j)
		{
			for (int nCurChannel = 0; nCurChannel < nChannel; ++nCurChannel)
				pDst[nCurChannel + i * nChannel + j * NewRowlen] = GetPositionValue(pSrc, nWidth, nHeight, nRowlen, nChannel, nCurChannel, i * wRatio, j * hRatio);
		}
	}

	return pDst;
}