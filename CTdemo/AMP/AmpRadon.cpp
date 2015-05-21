#include "stdafx.h"
#include "AmpRadon.h"

// ����ʾ���ݽضϾ���
#pragma warning(disable: 4244)

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
void AmpRadon(float* pSrc, int src_width, int src_height, float* pDst, int pan_angles, int pan_rays, float scan_range, float R, float D)
{
	float diagonal = sqrt(1.f * src_width * src_width + src_height * src_height);
	float theta_0 = acos(diagonal / 2.f / R);
	float pan_u0 = D / tan(theta_0);
	float pan_delta_u = (2.f * pan_u0) / pan_rays;
	float angles_separation = scan_range / pan_angles;
	int center_y = (src_height + 1) / 2;
	vector<float> vec_src(src_width * src_height);
	vec_src.reserve(src_width * src_height);
	vec_src.assign(pSrc, pSrc + src_width * src_height);
	array<float, 2> src_array(src_height, src_width, vec_src.begin(), vec_src.end());
	for (int i = 0; i < pan_angles; ++i)
	{
		int Xmin, Ymin, Xmax, Ymax, NewWidth, NewHeight;
		float rad = i * angles_separation;
		vector<float> pCur = _AMP_ComputeRotatedValues(src_array, src_width, src_height, rad, Xmin, Ymin, Xmax, Ymax, NewWidth, NewHeight);
		for (int j = 0; j < pan_rays; ++j)
		{
			float u = -pan_u0 + j * pan_delta_u;
			float k = -u / D;
			float c = -R * k + center_y;
			pDst[i + j * pan_angles] = _AMP_LineIntegrate(pCur, Xmin, Ymin, Xmax, Ymax, NewWidth, NewHeight, k, c);
		}
	}
	return;
}


vector<float> _AMP_ComputeRotatedValues(array<float, 2> &src_array, int src_width, int src_height, float angle, int &Xmin, int &Ymin, int &Xmax, int &Ymax, int &NewWidth, int &NewHeight)
{
	// ԭʼͼ���ĸ����������
	float x1, x2, x3, x4, y1, y2, y3, y4;
	x1 = 0;							y1 = 0;
	x2 = float(src_width - 1);		y2 = 0;
	x3 = x2;						y3 = float(src_height - 1);
	x4 = x1;						y4 = y3;
	// �ĸ�����˳ʱ����ת,������ԭ��
	float m_Ox = (src_width + 1) / 2.f;
	float m_Oy = (src_height + 1) / 2.f;
	float cos_theta = cos(angle);
	float sin_theta = sin(angle);
	PositionTransform(x1, y1, cos_theta, sin_theta, m_Ox, m_Oy);
	PositionTransform(x2, y2, cos_theta, sin_theta, m_Ox, m_Oy);
	PositionTransform(x3, y3, cos_theta, sin_theta, m_Ox, m_Oy);
	PositionTransform(x4, y4, cos_theta, sin_theta, m_Ox, m_Oy);
	Xmax = int(FindMaxBetween4Numbers(x1, x2, x3, x4));
	Ymax = int(FindMaxBetween4Numbers(y1, y2, y3, y4));
	Xmin = int(FindMinBetween4Numbers(y1, y2, y3, y4));
	Ymin = int(FindMinBetween4Numbers(y1, y2, y3, y4));
	// ��ͼ���ȡ��߶ȡ�ÿ���ֽ����ı仯
	NewWidth = Xmax - Xmin + 1;
	NewHeight = Ymax - Ymin + 1;

	array<float, 2> rotated_array(NewHeight, NewWidth);
	parallel_for_each(rotated_array.extent, [=, &src_array, &rotated_array](index<2> t_idx) restrict(amp)
	{
		int row = t_idx[0];
		int col = t_idx[1];
		float x = float(col + Xmin);
		float y = float(row + Ymin);
		_AMP_PositionTransform(x, y, cos_theta, -sin_theta, m_Ox, m_Oy);
		rotated_array[t_idx] = _AMP_Interpolate(src_array, src_height, src_width, x, y);
	});

	return rotated_array;
}


float _AMP_GetPositionValue(array<float, 2> &_ptrT, int row, int col, int x, int y) restrict(amp)
{
	if (x < 0 || x >= col || y < 0 || y >= row)
		return 0;
	return _ptrT(y, x);
}


float _AMP_Interpolate(array<float, 2> &_ptrT, int row, int col, float x, float y) restrict(amp)
{
	int x1, x2, x3, x4, y1, y2, y3, y4;
	float Ans1, Ans2;
	x1 = floor(x);	y1 = floor(y);
	x2 = x1 + 1;	y2 = y1;
	x3 = x2;		y3 = y1 + 1;
	x4 = x1;		y4 = y3;
	Ans1 = _AMP_GetPositionValue(_ptrT, row, col, x1, y1) * (1 - x + x1) + _AMP_GetPositionValue(_ptrT, row, col, x2, y2) * (x - x1);
	Ans2 = _AMP_GetPositionValue(_ptrT, row, col, x4, y4) * (1 - x + x4) + _AMP_GetPositionValue(_ptrT, row, col, x3, y3) * (x - x4);
	return (Ans1 * (1 - y + y1) + Ans2 * (y - y1));
}


float _AMP_LineGetXValue(float &k, float &c, float y)
{
	return (y - c) / k;
}


float _AMP_LineGetYValue(float &k, float &c, float x)
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
float _AMP_LineIntegrate(vector<float> &pSrc, int &Xmin, int &Ymin, int &Xmax, int &Ymax, int &Width, int &Height, float &k, float &c) 
{
	vector <CIntSection> Sections;

	for (int i = Xmin; i <= Xmax; ++i)
	{
		float x = i;
		float y = _AMP_LineGetYValue(k, c, x);
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
			float x = _AMP_LineGetXValue(k, c, y);
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
			float x = _AMP_LineGetXValue(k, c, y);
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


void _AMP_PositionTransform(float &x, float &y, float cos_theta, float sin_theta) restrict(amp)
{
	float x_temp = x * cos_theta - y * sin_theta;
	y = x * sin_theta + y * cos_theta;
	x = x_temp;
}


void _AMP_PositionTransform(float &x, float &y, float cos_theta, float sin_theta, float x0, float y0) restrict(amp)
{
	float delta_x = x - x0;
	float delta_y = y - y0;
	_AMP_PositionTransform(delta_x, delta_y, cos_theta, sin_theta);
	x = x0 + delta_x;
	y = y0 + delta_y;
}