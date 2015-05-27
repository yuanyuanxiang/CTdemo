#include "stdafx.h"
#include "Functions.h"
#include "IntSection.h"
#include <vector>
#include <algorithm>
#include <limits>
using namespace std;

// ����ʾ���ݽضϾ���
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


// ������ת���������Ϊ��ת�Ƕȵ����ҡ�����ֵ
void PositionTransform(float &x, float &y, float cos_sin[2], float x0, float y0)
{
	float delta_x = x - x0;
	float delta_y = y - y0;
	PositionTransform(delta_x, delta_y, cos_sin);
	x = x0 + delta_x;
	y = y0 + delta_y;
}


// ������ת���������Ϊ��ת�Ƕȵ����ҡ�����ֵ
void PositionTransform(float &x, float &y, float cos_sin[2])
{
	float x_temp = x * cos_sin[0] - y * cos_sin[1];
	y = x * cos_sin[1] + y * cos_sin[0];
	x = x_temp;
}


// ������ת���������Ϊ��ת�Ƕȵ����ҡ�����ֵ
void PositionTransform(float &x, float &y, float cos_theta, float sin_theta, float x0, float y0)
{
	float delta_x = x - x0;
	float delta_y = y - y0;
	PositionTransform(delta_x, delta_y, cos_theta, sin_theta);
	x = x0 + delta_x;
	y = y0 + delta_y;
}


// ������ת���������Ϊ��ת�Ƕȵ����ҡ�����ֵ
void PositionTransform(float &x, float &y, float cos_theta, float sin_theta)
{
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
float* pDst					Ŀ��
float* pSrc					ָ��ͶӰ���ݵ�ָ��
int nWidth					��ȣ��˴���ʾfai
int nHeight					�߶ȣ��˴���ʾr
float delta_r				��ɢ��֮���dr
float w0					��Ƶ��ֹ
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
			pDst[n + m * nWidth] = sum * delta_r;
		}
	}
}


// ����ˣ�HW(r). w0 - ��Ƶ��ֹ
float ConvKernel(float x, float w0)
{
	if(x == 0) return w0 * w0;
	float theta = 2 * PI * w0 * x;
	return (-1 + cos(theta) + theta * sin(theta)) / (2 * PI * PI * x * x);
}


/*
float* pDst			Ŀ��
float* pPrj			������ͶӰ
int nWidth			�ؽ�ͼ��Ŀ�
int nHeight			�ؽ�ͼ��ĸ�
int nRays			ͶӰ������(r)
int nAngles			ͶӰ������(fai)
float delta_r		�����ܶ�
float delta_fai		ɨ�����
*/
void BackProject(float* pDst, float* pPrj, int nWidth, int nHeight, int nRays, int nAngles, float delta_r, float delta_fai)
{
	int m, n, i;
	int cx = (nWidth + 1) / 2;
	int cy = (nHeight + 1) / 2;
	int med = (nRays + 1) / 2;
	float sum = 0.f;

	// 2015.5.19 �����Ǻ����ŵ�ѭ��������
	float *cos_fai = new float[nAngles];
	float *sin_fai = new float[nAngles];

#pragma omp parallel for
	for (i = 0; i < nAngles; ++i)
	{
		float fai = i * delta_fai;
		cos_fai[i] = cos(fai);
		sin_fai[i] = sin(fai);
	}

#pragma omp parallel for private(m, n, i) reduction(+ : sum)
	for (m = 0; m < nHeight; ++m)
	{
		for (n = 0; n < nWidth; ++n)
		{
			sum = 0.f;
			for (i = 0; i < nAngles; ++i)
			{
				float fai = i * delta_fai;
				float r = (n - cx) * cos_fai[i] + (m - cy) * sin_fai[i];
				sum += LinearInterp(pPrj, nAngles, nRays, i, med + r * delta_r);
			}
			pDst[n + m * nWidth] = sum * delta_fai;
		}
	}

	SAFE_DELETE(cos_fai);
	SAFE_DELETE(sin_fai);
}


// ����һά��ֵ:��ֵ��x��ֵ.
float LinearInterp(float* pPrj, int nWidth, float x)
{
	int x1 = floor(x), x2 = x1 + 1;
	if ( x1 < 0 || x2 >= nWidth)
		return 0;
	return pPrj[x1] * (1 - x + x1) + pPrj[x2] * (x - x1);
}


// ���Բ�ֵ:��width*height�����в�ֵ��(x, y)��ֵ.
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
	float u0 = D / tan(theta_0);
	float delta_u = (2.f * u0) / nHeight;

#pragma omp parallel for private(m, n, i) reduction(+ : sum)
	for (m = 0; m < nHeight; ++m)
	{
		for (n = 0; n < nWidth; ++n)
		{
			sum = 0.f;
			for (i = 0; i < nHeight; ++i)
			{
				// ��Ȩ�����
				float u = -u0 + i * delta_u;
				float alpha = D / sqrt(D * D + u * u);
				sum += alpha * pSrc[n + i * nWidth] * ConvKernel(m - i, w0);
			}
			pDst[n + m * nWidth] = sum * delta_r;
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

	// 2015.5.19 �����Ǻ����ŵ�ѭ��������
	float *cos_fai = new float[nAngles];
	float *sin_fai = new float[nAngles];

#pragma omp parallel for
	for (i = 0; i < nAngles; ++i)
	{
		float fai = i * delta_fai;
		cos_fai[i] = cos(fai);
		sin_fai[i] = sin(fai);
	}

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
				float r = x1 * cos_fai[i] + x2 * sin_fai[i];
				float alpha = R * D / ((R - x2 * cos_fai[i] + x1 * sin_fai[i]) * (R - x2 * cos_fai[i] + x1 * sin_fai[i]));
				sum += alpha * LinearInterp(pPrj, nAngles, nRays, i, med + r * delta_r);
			}
			pDst[n + m * nWidth] = sum * delta_fai;
		}
	}

	SAFE_DELETE(cos_fai);
	SAFE_DELETE(sin_fai);
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


// ��pSrcָ��ָ�������д���ļ�[path]��width��heightָ���˿����.
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
float &k		ֱ��б��
float &c		ֱ�߽ؾ�
float y			��֪yֵ
*/
float LineGetXValue(float &k, float &c, float y)
{
	ASSERT(k != 0);
	return (y - c) / k;
}


/*
float &k		ֱ��б��
float &c		ֱ�߽ؾ�
float x			��֪xֵ
*/
float LineGetYValue(float &k, float &c, float x)
{
	return (k * x + c);
}


/*
float* pSrc		ͼ������
int Xmin		���½�x����
int Ymin		���½�y����
int Xmax		���Ͻ�x����
int Ymax		���Ͻ�y����
int &Width		ͼ����
int &Height		ͼ��߶�
int &Rowlen		ÿ�и�����
int CurChannel	��ǰͨ��
int &Channel	ͨ������
float &k		ֱ��б��
float &c		ֱ����y��ؾ�
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

	// ����
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


/*
radon�����float* pDst
����ͼ����Ϣ��loat* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel
��ǰͨ����int nCurChannel
�Ƕȼ�༰������float angles_separation, int nAnglesNum
���߼�༰������float pixels_separation, int nRaysNum
*/
void ImageRadon(float* pDst, float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, int nCurChannel, 
				float angles_separation, int nAnglesNum, float pixels_separation, int nRaysNum)
{
	float zoom_rate = 1 / pixels_separation;
	int nNewWidth = zoom_rate * nWidth;
	int nNewHeight = zoom_rate * nHeight;
	int nNewRowlen = nNewWidth * nChannel;
	float *pSrc2 = ImageZoom(pSrc, nWidth, nHeight, nRowlen, nChannel, nNewWidth, nNewHeight);

	int nNewRaysNum = ComputeRaysNum(nNewWidth, nNewHeight);
	int nDetectorCenter = (nNewRaysNum + 1) / 2;
	float density = 1.f * nNewRaysNum / nRaysNum;

#pragma omp parallel for
	for (int i = 0; i < nAnglesNum; ++i)
	{
		float *temp = new float[nNewRaysNum];
		memset(temp, 0, nNewRaysNum * sizeof(float));
		float angle = i * angles_separation;
		ImageIntegrate(temp, pSrc2, nNewWidth, nNewHeight, nNewRowlen, nChannel, nCurChannel, angle, nNewRaysNum);
		
#pragma omp parallel for
		for (int j = 0; j < nRaysNum; ++j)
		{
			float x = j * density;
			pDst[i + j * nAnglesNum] = LinearInterp(temp, nNewRaysNum, x) * pixels_separation;
		}
		SAFE_DELETE(temp);
	}

	SAFE_DELETE(pSrc2);
}


// �ص��������߻��֣����������radon�任
/*
float* pDst		����ֵ
float* pSrc		ͼ������
int &nWidth		ͼ����
int &nHeight	ͼ��߶�
int &nRowlen	ÿ�и�����
int &nChannel	ͨ������
int nCurChannel	��ǰͨ��
float angle		����
int &nLength	���ָ���
*/
void ImageIntegrate(float* pDst, float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, int nCurChannel, float angle, int &nLength)
{
	// ͼ����ת֮�����Ϣ����������
	int NewWidth = 0, NewHeight = 0, NewRowlen = 0;
	int Xmin = 0, Ymin = 0, Xmax = 0, Ymax = 0;
	float *pBits = ImageRotate(pSrc, nWidth, nHeight, nRowlen, nChannel, angle, 0, 0, Xmin, Ymin, Xmax, Ymax, NewWidth, NewHeight, NewRowlen);
	// ͼ�����ۼ�
	float* add_width = new float[NewWidth];
	memset(add_width, 0, NewWidth * sizeof(float));
	// sumΪ��ʹOpenMP����������
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
		add_width[i] = sum;
	}

	// ̽�������ĺ�ͼ���ȵ�����
	int nDetectorCenter = (nLength + 1) / 2;
	int nHalfWidth = (NewWidth + 1) / 2;

#pragma omp parallel for
	for (int i = 0; i < nLength; ++i)
	{
		int s = (i - nDetectorCenter) + nHalfWidth;
		if (s >= 0 && s < NewWidth)
			pDst[i] = add_width[s];
		//TRACE("��� = %d, ֵ = %f\n", i, pDst[i]);
	}

	SAFE_DELETE(add_width);
	SAFE_DELETE(pBits);
}


/*
����ͼ����Ϣ��float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel
��ת�Ƕȣ�float angle
���ͼ����Ϣ��int &NewWidth, int &NewHeight, int &NewRowlen
*/
float* ImageRotate(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, float angle, int &NewWidth, int &NewHeight, int &NewRowlen)
{
	// ԭʼͼ���ĸ����������
	float x1, x2, x3, x4, y1, y2, y3, y4;
	x1 = 0;							y1 = 0;
	x2 = float(nWidth - 1);			y2 = 0;
	x3 = x2;						y3 = float(nHeight - 1);
	x4 = x1;						y4 = y3;
	// 2015.5.19 Ϊ�˼��ټ��㣬�����Ǻ����ŵ�ѭ������
	float cos_theta = cos(angle);
	float sin_theta = sin(angle);
	// �ĸ�����˳ʱ����ת,��ͼ�����½�
	PositionTransform(x1, y1, cos_theta, sin_theta);
	PositionTransform(x2, y2, cos_theta, sin_theta);
	PositionTransform(x3, y3, cos_theta, sin_theta);
	PositionTransform(x4, y4, cos_theta, sin_theta);
	int Xmin, Xmax, Ymin, Ymax;
	Xmax = int(FindMaxBetween4Numbers(x1, x2, x3, x4));
	Ymax = int(FindMaxBetween4Numbers(y1, y2, y3, y4));
	Xmin = int(FindMinBetween4Numbers(x1, x2, x3, x4));
	Ymin = int(FindMinBetween4Numbers(y1, y2, y3, y4));
	// ��ͼ���ȡ��߶ȡ�ÿ���ֽ����ı仯
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
			PositionTransform(x, y, cos_theta, -sin_theta);
			for (int nCurChannel = 0; nCurChannel < nChannel; ++nCurChannel)
				pDst[nCurChannel + i * nChannel + j * NewRowlen] = GetPositionValue(pSrc, nWidth, nHeight, nRowlen, nChannel, nCurChannel, x, y);
		}
	}

	return pDst;
}


float* ImageRotate(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, float angle, float x0, float y0, int &NewWidth, int &NewHeight, int &NewRowlen)
{
	// ԭʼͼ���ĸ����������
	float x1, x2, x3, x4, y1, y2, y3, y4;
	x1 = 0;							y1 = 0;
	x2 = float(nWidth - 1);			y2 = 0;
	x3 = x2;						y3 = float(nHeight - 1);
	x4 = x1;						y4 = y3;
	// 2015.5.19 Ϊ�˼��ټ��㣬�����Ǻ����ŵ�ѭ������
	float cos_theta = cos(angle);
	float sin_theta = sin(angle);
	// �ĸ�����˳ʱ����ת,��ͼ������
	PositionTransform(x1, y1, cos_theta, sin_theta, x0, y0);
	PositionTransform(x2, y2, cos_theta, sin_theta, x0, y0);
	PositionTransform(x3, y3, cos_theta, sin_theta, x0, y0);
	PositionTransform(x4, y4, cos_theta, sin_theta, x0, y0);
	int Xmin, Xmax, Ymin, Ymax;
	Xmax = int(FindMaxBetween4Numbers(x1, x2, x3, x4));
	Ymax = int(FindMaxBetween4Numbers(y1, y2, y3, y4));
	Xmin = int(FindMinBetween4Numbers(x1, x2, x3, x4));
	Ymin = int(FindMinBetween4Numbers(y1, y2, y3, y4));
	// ��ͼ���ȡ��߶ȡ�ÿ���ֽ����ı仯
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
			PositionTransform(x, y, cos_theta, -sin_theta, x0, y0);
			for (int nCurChannel = 0; nCurChannel < nChannel; ++nCurChannel)
				pDst[nCurChannel + i * nChannel + j * NewRowlen] = GetPositionValue(pSrc, nWidth, nHeight, nRowlen, nChannel, nCurChannel, x, y);
		}
	}

	return pDst;
}


float* ImageRotate(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, float angle, float x0, float y0, int &Xmin, int &Ymin, int &Xmax, int &Ymax, int &NewWidth, int &NewHeight, int &NewRowlen)
{
	// ԭʼͼ���ĸ����������
	float x1, x2, x3, x4, y1, y2, y3, y4;
	x1 = 0;							y1 = 0;
	x2 = float(nWidth - 1);			y2 = 0;
	x3 = x2;						y3 = float(nHeight - 1);
	x4 = x1;						y4 = y3;
	// 2015.5.19 Ϊ�˼��ټ��㣬�����Ǻ����ŵ�ѭ������
	float cos_theta = cos(angle);
	float sin_theta = sin(angle);
	// �ĸ�����˳ʱ����ת,��ͼ������
	PositionTransform(x1, y1, cos_theta, sin_theta, x0, y0);
	PositionTransform(x2, y2, cos_theta, sin_theta, x0, y0);
	PositionTransform(x3, y3, cos_theta, sin_theta, x0, y0);
	PositionTransform(x4, y4, cos_theta, sin_theta, x0, y0);
	Xmax = int(FindMaxBetween4Numbers(x1, x2, x3, x4));
	Ymax = int(FindMaxBetween4Numbers(y1, y2, y3, y4));
	Xmin = int(FindMinBetween4Numbers(x1, x2, x3, x4));
	Ymin = int(FindMinBetween4Numbers(y1, y2, y3, y4));
	// ��ͼ���ȡ��߶ȡ�ÿ���ֽ����ı仯
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
			PositionTransform(x, y, cos_theta, -sin_theta, x0, y0);
			for (int nCurChannel = 0; nCurChannel < nChannel; ++nCurChannel)
			{
				pDst[nCurChannel + i * nChannel + j * NewRowlen] = GetPositionValue(pSrc, nWidth, nHeight, nRowlen, nChannel, nCurChannel, x, y);
				//TRACE("�� = %d, �� = %d, ֵ = %f\n", i, j, pDst[nCurChannel + i * nChannel + j * NewRowlen]);
			}
		}
	}

	return pDst;
}


float* ImageZoom(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, int NewWidth, int NewHeight)
{
	int NewRowlen = nChannel * NewWidth;
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


int ComputeRaysNum(int nWidth, int nHeight)
{
	// �ο�MATLAB
	int temp1 = nWidth - (int)floor((nWidth - 1) / 2.f) - 1;
	int temp2 = nHeight - (int)floor((nHeight - 1) / 2.f) - 1;
	return 2 * (int)ceil(sqrt(1.f * temp1 * temp1 + temp2 * temp2)) + 3;
}


/*
��������			����ͼ��
float* pPrj			ͶӰͼ��
int nRays			ͼ��߶�
int nAngles			ͼ����
float delta_r		���
*/
float* DiffImage(float* pPrj, int nRays, int nAngles, float delta_r)
{
	float* pDst = new float[nRays * nAngles * sizeof(float)];
	for (int n = 0; n < nAngles; ++n)
	{
		pDst[n] =  pPrj[n] / delta_r;
	}

#pragma omp parallel for
	for (int m = 1; m < nRays; ++m)
	{
		for (int n = 0; n < nAngles; ++n)
		{
			pDst[n + m * nAngles] =  (pPrj[n + m * nAngles] - pPrj[n + (m - 1) * nAngles]) / delta_r;
		}
	}
	return pDst;
}


/*
float* pDst			Ŀ��
float* pPrj			ͶӰͼ��
int nWidth			ԭʼͼ��Ŀ�
int nHeight			ԭʼͼ��ĸ�
int nRays			ͶӰ������(r)
int nAngles			ͶӰ������(fai)
float delta_r		ɨ�����
float delta_fai		ɨ�����
float theta			Hilbert�Ƕ�
*/
void DBPImage(float* pDst, float* pPrj, int nWidth, int nHeight, int nRays, int nAngles, float delta_r, float delta_fai, float theta)
{
	int m, n, i;
	int cx = (nWidth + 1) / 2;
	int cy = (nHeight + 1) / 2;
	int med = (nRays + 1) / 2;
	float sum = 0.f;

	// 2015.5.19 �����Ǻ����ŵ�ѭ��������
	float *cos_fai = new float[nAngles];
	float *sin_fai = new float[nAngles];
	float *sgn = new float[nAngles];

	// sin(fai - theta) = sin*cos - cos*sin
	float sin_theta = sin(theta);
	float cos_theta = cos(theta);

#pragma omp parallel for
	for (i = 0; i < nAngles; ++i)
	{
		float fai = i * delta_fai;
		cos_fai[i] = cos(fai);
		sin_fai[i] = sin(fai);
		float sin_fai_theta = sin_fai[i] * cos_theta - cos_fai[i] * sin_theta;
		if (sin_fai_theta > 0)
			sgn[i] = 1.f;
		else if (sin_fai_theta < 0)
			sgn[i] = -1.f;
		else
			sgn[i] = 0.f;
	}

	float* pDiff = DiffImage(pPrj, nRays, nAngles, delta_r);

#pragma omp parallel for private(m, n, i) reduction(+ : sum)
	for (m = 0; m < nHeight; ++m)
	{
		for (n = 0; n < nWidth; ++n)
		{
			sum = 0.f;
			for (i = 0; i < nAngles; ++i)
			{
				float fai = i * delta_fai;
				float r = (n - cx) * cos_fai[i] + (m - cy) * sin_fai[i];
				sum += sgn[i] * LinearInterp(pDiff, nAngles, nRays, i, med + r / delta_r);
			}
			pDst[n + m * nWidth] = sum * delta_fai;
		}
	}

	SAFE_DELETE(pDiff);
	SAFE_DELETE(cos_fai);
	SAFE_DELETE(sin_fai);
	SAFE_DELETE(sgn);
}


// ϣ�����ؾ���˺���
float HilbertKernel(float x)
{
	if (x == 0)
	//return 0;
	return (numeric_limits<float>::max)();
	return 1 / (PI * x);
}


/*
float* pDst					Ŀ��
float* pSrc					ָ��DBP���ݵ�ָ��
int nWidth					��ȣ��˴���ʾfai
int nHeight					�߶ȣ��˴���ʾr
float delta_r				��ɢ��֮���dr
*/
void InverseHilbert(float* pDst, float* pSrc, int nWidth, int nHeight, float delta_r)
{
	int s, t, s1;
	float sum = 0.f;

	float Lt = -1.f;
	float Ut = nHeight;
	float s0 = 0.f, Ct;

//#pragma omp parallel for private(m, n, i) reduction(+ : sum)
	for (s = 0; s < nHeight; ++s)
	{
		for (t = 0; t < nWidth; ++t)
		{
			sum = 0.f;
			Ct = 0.f;
			for (s1 = 0; s1 < nHeight; ++s1)
			{
				sum += sqrt((s1 - Lt) * (Ut - s1)) * pSrc[t + s1 * nWidth] * HilbertKernel(s - s1);
				Ct += sqrt((s1 - Lt) * (Ut - s1)) * pSrc[t + s1 * nWidth] * HilbertKernel(s0 - s1);
			}
			pDst[t + s * nWidth] = -(sum - Ct) / sqrt((s - Lt) * (Ut - s)) * delta_r;
		}
	}
}