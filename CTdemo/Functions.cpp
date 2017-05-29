#include "stdafx.h"
#include "Functions.h"

// pi*pi
const float PI_2 = PI * PI;

// 卷积核：HW(r). w0 - 高频截止
inline float CT::CosineKernel(float x, float w0)
{
	if(x == 0)
		return w0 * w0;
	float theta = 2 * PI * w0 * x;
	return (-1 + cos(theta) + theta * sin(theta)) / (2 * PI_2 * x * x);
}


// R_L窗函数:n-探测器编号；d-探测器间距
inline float CT::R_LKernel(int n, float d)
{
	if (n == 0)
		return 1 / (4 * d * d);
	return (n % 2 == 0) ? 0 : -1 / (PI_2 * n * n * d * d);
}


// S_L窗函数:n-探测器编号；d-探测器间距
inline float CT::S_LKernel(int n, float d)
{
	return 2 / ((PI_2 * d * d) * (1 - 4 * n * n));
}


// Hamming窗函数:n-探测器编号；d-探测器间距
inline float CT::HammingKernel(int n, float d, float a)
{
	int n_2 = n * n;//n*n
	float d_2 = d * d;//d*d
	if (n == 0)
		return -(4 + (-4 + PI_2) * a) / (4 * d_2 * PI_2);
	if (n == 1 || n == -1)
		return (PI_2 * (-1 + a) - 8 * a) / (8 * d_2 * PI_2);
	return n % 2 == 0 ? 
		(-a + n_2 * (-(-3 * a + n_2 + 1)) + (a + (2 * a - 1) * n_2 * n_2 - (a + 1) * n_2)) / (2 * PI_2 * d_2 * n_2 * (n_2 - 1) * (n_2 - 1)) : 
		(-a + n_2 * (-(-3 * a + n_2 + 1)) - (a + (2 * a - 1) * n_2 * n_2 - (a + 1) * n_2)) / (2 * PI_2 * d_2 * n_2 * (n_2 - 1) * (n_2 - 1));
}


// 希尔伯特卷积核函数
inline float CT::HilbertKernel(float x)
{
	return 0 == x ? 0 : 1 / (PI * x);
}


/** 
* @brief 获得直线的x值
* @param[in] k		直线斜率
* @param[in] c		直线截距
* @param[in] y		已知y值
*/
inline float CT::LineGetXValue(float k, float c, float y)
{
	return 0 == k ? 0 : (y - c) / k;
}


/** 
* @brief 获得直线的y值
* @param[in] &k		直线斜率
* @param[in] &c		直线截距
* @param[in] x		已知x值
*/
inline float CT::LineGetYValue(float k, float c, float x)
{
	return (k * x + c);
}


/**
* @brief 计算最佳的射线采样条数
* @param[in] nWidth 宽度
* @param[in] nHeight 高度
*/
int CT::ComputeRaysNum(int nWidth, int nHeight)
{
	// 参考MATLAB
	int temp1 = nWidth - (int)floor((nWidth - 1) / 2.f) - 1;
	int temp2 = nHeight - (int)floor((nHeight - 1) / 2.f) - 1;
	return 2 * (int)ceil(sqrt(1.f * temp1 * temp1 + temp2 * temp2)) + 3;
}

/** 
* @brief 卷积
* @param[in] *pDst				目标
* @param[in] delta_r			离散化之后的dr
* @param[in] w0					高频截止
*/
void CT::Convolute(float* pDst, float delta_r, float w0, int nConvKernel)
{
	int m, n, i;
	float sum;

	switch (nConvKernel)
	{
	case CONVOLUTE_KERNEL_COSINE:
		for (m = 0; m < m_nHeight; ++m)
		{
			for (n = 0; n < m_nWidth; ++n)
			{
				sum = 0;
				for (i = 0; i < m_nHeight; ++i)
					sum += m_pSrc[n + i * m_nWidth] * CosineKernel(m - i, w0);
				pDst[n + m * m_nWidth] = sum * delta_r;
			}
		}
		break;
	case CONVOLUTE_KERNEL_RL:
		for (m = 0; m < m_nHeight; ++m)
		{
			for (n = 0; n < m_nWidth; ++n)
			{
				sum = 0;
				for (i = 0; i < m_nHeight; ++i)
					sum += m_pSrc[n + i * m_nWidth] * R_LKernel(m - i, delta_r);
				pDst[n + m * m_nWidth] = sum * delta_r;
			}
		}
		break;
	case CONVOLUTE_KERNEL_SL:
		for (m = 0; m < m_nHeight; ++m)
		{
			for (n = 0; n < m_nWidth; ++n)
			{
				sum = 0;
				for (i = 0; i < m_nHeight; ++i)
					sum += m_pSrc[n + i * m_nWidth] * S_LKernel(m - i, delta_r);
				pDst[n + m * m_nWidth] = sum * delta_r;
			}
		}
		break;
	case CONVOLUTE_KERNEL_HAMMING:
		for (m = 0; m < m_nHeight; ++m)
		{
			for (n = 0; n < m_nWidth; ++n)
			{
				sum = 0;
				for (i = 0; i < m_nHeight; ++i)
					sum += m_pSrc[n + i * m_nWidth] * HammingKernel(m - i, delta_r, w0);
				pDst[n + m * m_nWidth] = sum * delta_r;
			}
		}
		break;
	default:
		break;
	}
}

/** 
* @brief 平行束反投影
* @param[in] *pDst			目标
* @param[in] nRays			投影的行数(r)
* @param[in] nAngles		投影的列数(fai)
* @param[in] delta_r		射线密度
* @param[in] delta_fai		扫描参数
*/
void CT::BackProject(float* pDst, int nRays, int nAngles, float delta_r, float delta_fai)
{
	int m, n, i;
	int cx = (m_nWidth + 1) / 2;
	int cy = (m_nHeight + 1) / 2;
	int med = (nRays + 1) / 2;
	float sum = 0;

	// 2015.5.19 将三角函数放到循环体外面
	float *cos_fai = new float[nAngles];
	float *sin_fai = new float[nAngles];

	for (i = 0; i < nAngles; ++i)
	{
		float fai = i * delta_fai;
		cos_fai[i] = cos(fai);
		sin_fai[i] = sin(fai);
	}

	for (m = 0; m < m_nHeight; ++m)
	{
		for (n = 0; n < m_nWidth; ++n)
		{
			sum = 0;
			for (i = 0; i < nAngles; ++i)
			{
				float fai = i * delta_fai;
				float r = (n - cx) * cos_fai[i] + (m - cy) * sin_fai[i];
				sum += LinearInterp(m_pSrc, nAngles, nRays, i, med + r * delta_r);
			}
			pDst[n + m * m_nWidth] = sum * delta_fai;
		}
	}

	SAFE_DELETE(cos_fai);
	SAFE_DELETE(sin_fai);
}

void CT::Convolute(float* pDst, float delta_r, float w0, float R, float D, int nConvKernel)
{
	int m, n, i;
	float sum = 0;
	float theta_0 = acos(m_nHeight * delta_r / 2.f / R);
	float u0 = D / tan(theta_0);
	float delta_u = (2.f * u0) / m_nHeight;

	switch (nConvKernel)
	{
	case CONVOLUTE_KERNEL_COSINE:
		for (m = 0; m < m_nHeight; ++m)
		{
			for (n = 0; n < m_nWidth; ++n)
			{
				sum = 0;
				for (i = 0; i < m_nHeight; ++i)
				{
					// 加权做卷积
					float u = -u0 + i * delta_u;
					float alpha = D / sqrt(D * D + u * u);
					sum += alpha * m_pSrc[n + i * m_nWidth] * CosineKernel(m - i, w0);
				}
				pDst[n + m * m_nWidth] = sum * delta_r;
			}
		}
		break;
	case CONVOLUTE_KERNEL_RL:
		for (m = 0; m < m_nHeight; ++m)
		{
			for (n = 0; n < m_nWidth; ++n)
			{
				sum = 0;
				for (i = 0; i < m_nHeight; ++i)
				{
					// 加权做卷积
					float u = -u0 + i * delta_u;
					float alpha = D / sqrt(D * D + u * u);
					sum += alpha * m_pSrc[n + i * m_nWidth] * R_LKernel(m - i, delta_r);
				}
				pDst[n + m * m_nWidth] = sum * delta_r;
			}
		}
		break;
	case CONVOLUTE_KERNEL_SL:
		for (m = 0; m < m_nHeight; ++m)
		{
			for (n = 0; n < m_nWidth; ++n)
			{
				sum = 0;
				for (i = 0; i < m_nHeight; ++i)
				{
					// 加权做卷积
					float u = -u0 + i * delta_u;
					float alpha = D / sqrt(D * D + u * u);
					sum += alpha * m_pSrc[n + i * m_nWidth] * S_LKernel(m - i, delta_r);
				}
				pDst[n + m * m_nWidth] = sum * delta_r;
			}
		}
		break;
	case CONVOLUTE_KERNEL_HAMMING:
		for (m = 0; m < m_nHeight; ++m)
		{
			for (n = 0; n < m_nWidth; ++n)
			{
				sum = 0;
				for (i = 0; i < m_nHeight; ++i)
				{
					// 加权做卷积
					float u = -u0 + i * delta_u;
					float alpha = D / sqrt(D * D + u * u);
					sum += alpha * m_pSrc[n + i * m_nWidth] * HammingKernel(m - i, delta_r, w0);
				}
				pDst[n + m * m_nWidth] = sum * delta_r;
			}
		}
		break;
	default:
		break;
	}
}

void CT::BackProject(float* pDst, int nRays, int nAngles, float delta_r, float delta_fai, float R, float D)
{
	int m, n, i;
	int cx = (m_nWidth + 1) / 2;
	int cy = (m_nHeight + 1) / 2;
	int med = (nRays + 1) / 2;
	float sum = 0;

	// 2015.5.19 将三角函数放到循环体外面
	float *cos_fai = new float[nAngles];
	float *sin_fai = new float[nAngles];

	for (i = 0; i < nAngles; ++i)
	{
		float fai = i * delta_fai;
		cos_fai[i] = cos(fai);
		sin_fai[i] = sin(fai);
	}

	for (m = 0; m < m_nHeight; ++m)
	{
		for (n = 0; n < m_nWidth; ++n)
		{
			sum = 0;
			for (i = 0; i < nAngles; ++i)
			{
				float x1 = (float)n - cx;
				float x2 = (float)m - cy;
				float fai = i * delta_fai;
				float r = x1 * cos_fai[i] + x2 * sin_fai[i];
				float alpha = R * D / ((R - x2 * cos_fai[i] + x1 * sin_fai[i]) * (R - x2 * cos_fai[i] + x1 * sin_fai[i]));
				sum += alpha * LinearInterp(m_pSrc, nAngles, nRays, i, med + r * delta_r);
			}
			pDst[n + m * m_nWidth] = sum * delta_fai;
		}
	}

	SAFE_DELETE(cos_fai);
	SAFE_DELETE(sin_fai);
}


/**
* @brief 导数图像
* @param[in] *pSrc			投影图像
* @param[in] nRays			图像高度
* @param[in] nAngles		图像宽度
* @param[in] delta_r		间距
*/
float* CT::DiffImage(const float* pSrc, int nRays, int nAngles, float delta_r)
{
	float* pDst = new float[nRays * nAngles];
	delta_r = 1.f / delta_r;
	for (int n = 0; n < nAngles; ++n)// 第1行
	{
		pDst[n] =  pSrc[n] * delta_r;
	}
	float *p0 = pDst + nAngles;
	const float *q0 = pSrc + nAngles;
	for (int m = 1; m < nRays; ++m, p0 += nAngles, q0 += nAngles)
	{
		float *p = p0;
		const float *q1 = q0, *q2 = q1 - nAngles;
		for (int n = 0; n < nAngles; ++n, ++p, ++q1)
		{
			*p = (*q1 - *q2) * delta_r;
		}
	}
	return pDst;
}

/** 
* @brief 有限区域希尔伯特逆变换
* @param[in] *pDst					目标
* @param[in] *pSrc					指向DBP数据的指针
* @param[in] nWidth					宽度，此处表示fai
* @param[in] nHeight				高度，此处表示r
* @param[in] delta_r				离散化之后的dr
*/
void CT::InverseHilbert(float* pDst, float delta_r)
{
	int s, t, s1;
	float sum = 0;

	float Lt = 0;
	float Ut = (float)m_nHeight;
	float s0 = 0, Ct = 0;

	for (t = 0; t < m_nWidth; ++t)
	{
		for (s = 0; s < m_nHeight; ++s)
		{
			sum = 0;
			Ct = 0;
			for (s1 = Lt; s1 < Ut; ++s1)
			{
				sum += sqrt((s1 - Lt) * (Ut - s1)) * m_pSrc[t + s1 * m_nWidth] * HilbertKernel(s - s1);
				Ct += sqrt((s1 - Lt) * (Ut - s1)) * m_pSrc[t + s1 * m_nWidth] * HilbertKernel(s0 - s1);
			}
			float sqrt_result = sqrt((s - Lt) * (Ut - s));
			if (sqrt_result != 0)
				pDst[t + s * m_nWidth] = -1.f / sqrt_result * (sum - Ct) * delta_r;
		}
	}
}

/**
* @brief 线积分
* @param[in] &k		直线斜率
* @param[in] &c		直接与y轴截距
*/
float CT::LineIntegrate(const CLogoRect &rect, float &k, float &c)
{
	vector <CIntSection> Sections;
	int Xmin = rect.left;
	int Ymin = rect.right;
	int Xmax = rect.top;
	int Ymax = rect.bottom;
	for (int i = Xmin; i <= Xmax; ++i)
	{
		float x = (float)i;
		float y = LineGetYValue(k, c, x);
		if (y < Ymin || y > Ymax)
			continue;
		Sections.push_back(CIntSection(x, y));
	}

	for (int j = Ymin; j <= Ymax; ++j)
	{
		float y = (float)j;
		float x = LineGetXValue(k, c, y);
		if (x < Xmin || x > Xmax)
			continue;
		Sections.push_back(CIntSection(x, y));
	}

	// 排序
	sort(Sections.begin(), Sections.end());

	float s = 0;
	int Num = Sections.size() - 1;
	for (int i = 0; i < Num; ++i)
	{
		int x = (int)Sections[i].m_xPos - Xmin;
		int y = (int)Sections[i].m_yPos - Ymin;
		if (0 <= x && x < m_nWidth && 0 <= y && y < m_nHeight)
			s += Distance(Sections[i], Sections[i + 1]) * m_pSrc[x * m_nChannel + y * m_nRowlen];
	}
	return s;
}

/** 
* @brief radon变换
* @param[in] *pDst
* @param[in] angles_separation 角度间距 
* @param[in] nAnglesNum 角度个数 
* @param[in] pixels_separation 射线间距 
* @param[in] nRaysNum 射线个数 
*/
void CT::ImageRadon(float* pDst, float angles_separation, int nAnglesNum, float pixels_separation, int nRaysNum)
{
	float zoom_rate = 1 / pixels_separation;
	int nNewWidth = zoom_rate * m_nWidth;
	int nNewHeight = zoom_rate * m_nHeight;

	ImageTransform it(m_pSrc, m_nWidth, m_nHeight, m_nRowlen, m_nChannel);
	float *pZoomSrc = it.ImageZoom(nNewWidth, nNewHeight);
	CT ct(pZoomSrc, nNewWidth, nNewHeight);
	int nNewRaysNum = ComputeRaysNum(nNewWidth, nNewHeight);
	int nDetectorCenter = (nNewRaysNum + 1) / 2;
	float density = float(nNewRaysNum) / nRaysNum;
	float angle = 0;
	for (int i = 0; i < nAnglesNum; ++i, angle += angles_separation)
	{
		float *temp = new float[nNewRaysNum];
		memset(temp, 0, nNewRaysNum * sizeof(float));
		ct.ImageIntegrate(temp, angle, nNewRaysNum);
		float *p = pDst + i, x = 0;
		for (int j = 0; j < nRaysNum; ++j, p += nAnglesNum, x += density)
		{
			*p = LinearInterp(temp, nNewRaysNum, x) * pixels_separation;
		}
		SAFE_DELETE(temp);
	}

	SAFE_DELETE(pZoomSrc);
}

/** 
* @brief DBP 图像
* @param[in] *pDst			目标
* @param[in] nRays			投影的行数(r)
* @param[in] nAngles		投影的列数(fai)
* @param[in] delta_r		扫描参数
* @param[in] delta_fai		扫描参数
* @param[in] theta			Hilbert角度
*/
void CT::DBPImage(float* pDst, int nRays, int nAngles, float delta_r, float delta_fai, float theta)
{
	int m, n, i;
	int cx = (m_nWidth + 1) / 2;
	int cy = (m_nHeight + 1) / 2;
	int med = (nRays + 1) / 2;
	float sum = 0;

	// 2015.5.19 将三角函数放到循环体外面
	float *cos_fai = new float[nAngles];
	float *sin_fai = new float[nAngles];
	float *sgn = new float[nAngles];

	// sin(fai - theta) = sin*cos - cos*sin
	float sin_theta = sin(theta);
	float cos_theta = cos(theta);

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
			sgn[i] = 0;
	}

	float* pDiff = DiffImage(m_pSrc, nRays, nAngles, delta_r);

	for (m = 0; m < m_nHeight; ++m)
	{
		for (n = 0; n < m_nWidth; ++n)
		{
			sum = 0;
			for (i = 0; i < nAngles; ++i)
			{
				float fai = i * delta_fai;
				float r = (n - cx) * cos_fai[i] + (m - cy) * sin_fai[i];
				sum += sgn[i] * LinearInterp(pDiff, nAngles, nRays, i, med + r * delta_r);
			}
			pDst[n + m * m_nWidth] = sum * delta_fai;
		}
	}

	SAFE_DELETE(pDiff);
	SAFE_DELETE(cos_fai);
	SAFE_DELETE(sin_fai);
	SAFE_DELETE(sgn);
}

/*
* @brief 沿单个方向线积分，单个方向的radon变换
* @param[in] *pDst			积分值
* @param[in] angle			方向
* @param[in] nLength		积分个数
*/
void CT::ImageIntegrate(float* pDst, float angle, int nLength)
{
	// 图像旋转之后的信息及顶点坐标
	int NewWidth = 0, NewHeight = 0;
	float *pBits = ImageRotate(PositionTransform(angle, 0, 0), NewWidth, NewHeight, CLogoRect());
	// 图像按行累加
	float* add_width = new float[NewWidth];
	int nNewRowlen = NewWidth * m_nChannel;
	const float *p = pBits;
	for (int i = 0; i < NewWidth; ++i, p += m_nChannel)
	{
		float sum = 0;
		const float *t = p;
		for (int j = 0; j < NewHeight; ++j, t += nNewRowlen)
		{
			sum += *t;
		}
		add_width[i] = sum;
	}

	// 探测器中心和图像宽度的中心
	int nDetectorCenter = (nLength + 1) / 2;
	int nHalfWidth = (NewWidth + 1) / 2;

	for (int i = 0; i < nLength; ++i)
	{
		int s = (i - nDetectorCenter) + nHalfWidth;
		if (s >= 0 && s < NewWidth)
			pDst[i] = add_width[s];
	}

	SAFE_DELETE(add_width);
	SAFE_DELETE(pBits);
}
