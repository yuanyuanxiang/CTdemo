
#include "PanRadon.h"
#include <algorithm>

/*
float* pSrc				//ͼ������
int src_width			//ͼ����
int src_height			//ͼ��߶�
float* pDst				//ͶӰ����
int pan_angles			//ͶӰ���
int pan_rays			//ͶӰ�߶�
float scan_range		//������Χ
float R					//SO
float D					//SO'
*/
void PanRadon(float* pSrc, int src_width, int src_height, float* pDst, int pan_angles, int pan_rays, float scan_range, float R, float D)
{
	float diagonal = sqrt(1.f * src_width * src_width + src_height * src_height);
	float theta_0 = acos(diagonal / 2.f / R);
	float pan_u0 = D / tan(theta_0);
	float pan_delta_u = (2.f * pan_u0) / pan_rays;
	float angles_separation = scan_range / pan_angles;
	int center_y = (src_height + 1) / 2;
	for (int i = 0; i < pan_angles; ++i)
	{
		int Xmin, Ymin, Xmax, Ymax, NewWidth, NewHeight;
		float rad = i * angles_separation;
		float *pCur = ComputeRotatedValues(pSrc, src_width, src_height, rad, Xmin, Ymin, Xmax, Ymax, NewWidth, NewHeight);
		for (int j = 0; j < pan_rays; ++j)
		{
			float u = -pan_u0 + j * pan_delta_u;
			float k = -u / D;
			float c = -R * k + center_y;
			pDst[i + j * pan_angles] = LineIntegrate(pCur, Xmin, Ymin, Xmax, Ymax, NewWidth, NewHeight, k, c);
		}
		SAFE_DELETE(pCur);
	}
}


float* ComputeRotatedValues(float *_ptrT, int Width, int Height, float angle, int &Xmin, int &Ymin, int &Xmax, int &Ymax, int &NewWidth, int &NewHeight) 
{
	// ԭʼͼ���ĸ����������
	float x1, x2, x3, x4, y1, y2, y3, y4;
	x1 = 0;							y1 = 0;
	x2 = float(Width - 1);			y2 = 0;
	x3 = x2;						y3 = float(Height - 1);
	x4 = x1;						y4 = y3;
	// �ĸ�����˳ʱ����ת,������ԭ��
	float m_Ox = Width / 2.f;
	float m_Oy = Height / 2.f;
	PositionTransform(x1, y1, angle, m_Ox, m_Oy);
	PositionTransform(x2, y2, angle, m_Ox, m_Oy);
	PositionTransform(x3, y3, angle, m_Ox, m_Oy);
	PositionTransform(x4, y4, angle, m_Ox, m_Oy);
	Xmax = int(FindMaxBetween4Numbers(x1, x2, x3, x4));
	Ymax = int(FindMaxBetween4Numbers(y1, y2, y3, y4));
	Xmin = int(FindMinBetween4Numbers(y1, y2, y3, y4));
	Ymin = int(FindMinBetween4Numbers(y1, y2, y3, y4));
	// ��ͼ���ȡ��߶ȡ�ÿ���ֽ����ı仯
	NewWidth = Xmax - Xmin + 1;
	NewHeight = Ymax - Ymin + 1;
	float *pDst = new float[NewWidth * NewHeight];
	memset(pDst, 0, NewWidth * NewHeight * sizeof(float));
	
	for (int i = 0; i < NewWidth; ++i)
	{
		for (int j = 0; j < NewHeight; ++j)
		{
			float x = float(i + Xmin);
			float y = float(j + Ymin);
			PositionTransform(x, y, -angle, m_Ox, m_Oy);
			*(pDst + i + j * NewWidth) = biLinearInterp(_ptrT, Height, Width, x, y);
		}
	}
	
	return pDst;
}


float GetPositionValue(float *_ptrT, int row, int col, int x, int y)
{
	if (x < 0 || x >= col || y < 0 || y >= row)
		return 0;
	return (float)*(_ptrT + x + y * col);
}


float biLinearInterp(float *_ptrT, int row, int col, float x, float y)
{
	int x1, x2, x3, x4, y1, y2, y3, y4;
	float Ans1, Ans2;
	x1 = floor(x);	y1 = floor(y);
	x2 = x1 + 1;	y2 = y1;
	x3 = x2;		y3 = y1 + 1;
	x4 = x1;		y4 = y3;
	Ans1 = GetPositionValue(_ptrT, row, col, x1, y1) * (1 - x + x1) + GetPositionValue(_ptrT, row, col, x2, y2) * (x - x1);
	Ans2 = GetPositionValue(_ptrT, row, col, x4, y4) * (1 - x + x4) + GetPositionValue(_ptrT, row, col, x3, y3) * (x - x4);
	return (Ans1 * (1 - y + y1) + Ans2 * (y - y1));
}


float cudaLineGetXValue(float &k, float &c, float y)
{
	return (y - c) / k;
}


float cudaLineGetYValue(float &k, float &c, float x)
{
	return (k * x + c);
}


/*
float* pSrc		ͼ������
int Xmin		���½�x����
int Ymin		���½�y����
int Xmax		���Ͻ�x����
int Ymax		���Ͻ�y����
int Width		ͼ����
int Height		ͼ��߶�
float &k		ֱ��б��
float &c		ֱ����y��ؾ�
*/
float LineIntegrate(float* pSrc, int Xmin, int Ymin, int Xmax, int Ymax, int Width, int Height, float &k, float &c) 
{
	vector <CIntSection> Sections;

	for (int i = Xmin; i <= Xmax; ++i)
	{
		float x = i;
		float y = cudaLineGetYValue(k, c, x);
		// ��Ϊֱ���ǵ������������Ե�y����ͼ���˾�break
		if (y < Ymin || y > Ymax)
			break;
		Sections.push_back(CIntSection(x, y));
	}
	// �������ۣ����ټ�����
	if (k >= 0)
	{
		int Y_start = int(c);
		int Y_end = Ymax;

		for (int j = Y_start; j <= Y_end; ++j)
		{
			float y = j;
			float x = cudaLineGetXValue(k, c, y);
			// ���ﲻ��ֱ��break��������ͼ���Կ���
			if (x < Xmin || x > Xmax)
				continue;
			Sections.push_back(CIntSection(x, y));
		}
	}
	else
	{
		int Y_start = int(c);
		int Y_end = Ymin;

		for (int j = Y_start; j >= Y_end; --j)
		{
			float y = j;
			float x = cudaLineGetXValue(k, c, y);
			// ���ﲻ��ֱ��break��������ͼ���Կ���
			if (x < Xmin || x > Xmax)
				continue;
			Sections.push_back(CIntSection(x, y));
		}
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
			s += Distance(Sections[i], Sections[i + 1]) * pSrc[x + y * Width];
	}
	return s;
}