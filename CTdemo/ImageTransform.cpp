#include "stdafx.h"
#include "ImageTransform.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/** 
* @brief ��תͼ��
* @param[in] &pt ��ת�任
* @param[out] &NewWidth �¿��
* @param[out] &NewHeight �¸߶�
* @param[out] &dstArea ͼ������
*/
float* ImageTransform::ImageRotate(const PositionTransform &pt, int &NewWidth, int &NewHeight, CLogoRect &dstArea) const 
{
	// ԭʼͼ���ĸ����������
	float x1, x2, x3, x4, y1, y2, y3, y4;
	x1 = 0;							y1 = 0;
	x2 = float(m_nWidth - 1);		y2 = 0;
	x3 = x2;						y3 = float(m_nHeight - 1);
	x4 = x1;						y4 = y3;
	// �ĸ�������ʱ����ת
	pt.Transform(x1, y1);
	pt.Transform(x2, y2);
	pt.Transform(x3, y3);
	pt.Transform(x4, y4);
	int Xmax = int(FindMaxBetween4Numbers(x1, x2, x3, x4));
	int Ymax = int(FindMaxBetween4Numbers(y1, y2, y3, y4));
	int Xmin = int(FindMinBetween4Numbers(x1, x2, x3, x4));
	int Ymin = int(FindMinBetween4Numbers(y1, y2, y3, y4));
	dstArea = CLogoRect(Xmin-1, Ymin-1, Xmax, Ymax);
	// ��ͼ���ȡ��߶ȡ�ÿ���ֽ����ı仯
	NewWidth = Xmax - Xmin + 1;
	NewHeight = Ymax - Ymin + 1;
	int NewRowlen = m_nChannel * NewWidth;
	float *pDst = new float[NewRowlen * NewHeight];
	float x0 = pt.x0, y0 = pt.y0, cos_theta = pt.cos_theta, sin_theta = -pt.sin_theta;

	for (int nCurChannel = 0; nCurChannel < m_nChannel; ++nCurChannel)
	{
		float *p0 = pDst + nCurChannel;
		for (int i = 0; i < NewWidth; ++i, p0 += m_nChannel)
		{
			float x = float(i + Xmin), dx = x - x0, t1 = dx * cos_theta, t2 = dx * sin_theta;
			float *p = p0;
			for (int j = 0; j < NewHeight; ++j, p += NewRowlen)
			{
				float y = float(j + Ymin), dy = y - y0, s1 = dy * sin_theta, s2 = dy * cos_theta;
				x = x0 + t1 - s1; y = y0 + t2 + s2;
				// �ο�biLinearInterp
				int x1 = int(x), y1 = int(y), x3 = x1 + 1, y3 = y1 + 1;
				// ���½ǵĵ�
				const float* pLB = m_pSrc + nCurChannel + x1 * m_nChannel + y1 * m_nRowlen;
				// ��Խ��Ĵ���
				*p = (x1 < 0 || x3 >= m_nWidth || y1 < 0 || y3 >= m_nHeight) ? 0 :
					(*pLB * (x3 - x) + *(pLB + m_nChannel) * (x - x1)) * (y3 - y)
					+ (*(pLB + m_nRowlen) * (x3 - x) + *(pLB + m_nChannel + m_nRowlen) * (x - x1)) * (y - y1);
			}
		}
	}

	return pDst;
}


/** 
* @brief ��ȡ����Ȥ����
* @param[in] &roi ����Ȥ����
*/
float* ImageTransform::ImageRoi(const CLogoRect &roi) const 
{
	ASSERT (0 <= roi.left && roi.left < m_nWidth && 0 <= roi.top && roi.top < m_nHeight);

	int nNewWidth = roi.Width();
	int nNewHeight = roi.Height();
	if (nNewWidth <= 0 || nNewHeight <= 0)
		return NULL;
	int nNewRowlen = nNewWidth * m_nChannel;
	float* pDst = new float[nNewHeight * nNewRowlen];

	const float *pSrcLine = m_pSrc + roi.left * m_nChannel + (m_nHeight - roi.bottom) * m_nRowlen;
	float *pDstLine = pDst;
	for (int i = 0; i < nNewHeight; ++i)
	{
		memcpy(pDstLine, pSrcLine, nNewRowlen * sizeof(float));
		pSrcLine += m_nRowlen;
		pDstLine += nNewRowlen;
	}

	return pDst;
}


/** 
* @brief �Ŵ�ͼ��
* @param[in] NewWidth �µĿ��
* @param[in] NewHeight �µĸ߶�
*/
float* ImageTransform::ImageZoom(int NewWidth, int NewHeight) const 
{
	int NewRowlen = m_nChannel * NewWidth;
	float* pDst = new float[NewRowlen * NewHeight];
	float wRatio = (float)m_nWidth / NewWidth;
	float hRatio = (float)m_nHeight / NewHeight;

	for (int nCurChannel = 0; nCurChannel < m_nChannel; ++nCurChannel)
	{
		float *p0 = pDst + nCurChannel;
		for (int i = 0; i < NewWidth; ++i, p0 += m_nChannel)
		{
			float *p = p0;
			for (int j = 0; j < NewHeight; ++j, p += NewRowlen)
			{
				*p = biLinearInterp(nCurChannel, i * wRatio, j * hRatio);
			}
		}
	}

	return pDst;
}
