#pragma once

/** 
* @file ImageTransform.h
* @brief ͼ�񼸺α任���������Կ�ƽ̨
* @author Ԭ����
*/

#include "DataTypes.h"
#include <cmath>

/**
* @class PositionTransform
* @brief ��任��
*/
class PositionTransform
{
public:
	float theta;		/**< ��ת�Ƕ� */
	float sin_theta;	/**< �Ƕ����� */
	float cos_theta;	/**< �Ƕ����� */
	float x0, y0;		/**< ��ת���� */

public:
	// ��ת�Ƕ�angle����ת����Ϊԭ��
	PositionTransform(float angle) : 
		theta(angle), sin_theta(sin(angle)), cos_theta(cos(angle)), x0(0), y0(0) { }

	// ��ת�Ƕ�angle����ת����Ϊx, y
	PositionTransform(float angle, float x, float y) : 
		theta(angle), sin_theta(sin(angle)), cos_theta(cos(angle)), x0(x), y0(y) { }

	/// ����������ʱ�룩��ת
	inline void Transform(float &x, float &y) const 
	{
		float dx = x - x0, dy = y - y0;
		float x_temp = dx * cos_theta - dy * sin_theta;
		dy = dx * sin_theta + dy * cos_theta;
		dx = x_temp;
		x = x0 + dx;
		y = y0 + dy;
	}


	/// ���귴��˳ʱ�룩��ת
	inline void InvTransform(float &x, float &y) const 
	{
		float dx = x - x0, dy = y - y0;
		float x_temp = dx * cos_theta - dy * -sin_theta;
		dy = dx * -sin_theta + dy * cos_theta;
		dx = x_temp;
		x = x0 + dx;
		y = y0 + dy;
	}
};

/**
* @class ImageTransform
* @brief ͼ��任��
*/
class ImageTransform
{
protected:
	const float *m_pSrc;/**< ͼ������ */
	int m_nWidth;		/**< ͼ���� */
	int m_nHeight;		/**< ͼ��߶� */
	int m_nRowlen;		/**< ͼ��ÿ���ֽ��� */
	int m_nChannel;		/**< ÿ����Ԫ���� */
	float *data;

	/** 
	* @brief ˫���Բ�ֵ��ȡ���� (x, y) ���ĺ���ֵ
	* @param[in] nCurChannel	��ǰͨ��
	* @param[in] x				��ֵ����
	* @param[in] y				��ֵ����
	*/
	inline float biLinearInterp(int nCurChannel, float x, float y) const 
	{
		/** 
		// ˫���Բ�ֵ��ʾ��ͼ:
		// Ans2��		(x4, y4)				(x3, y3)
		//							(x, y)
		// Ans1��		(x1, y1)				(x2, y2)
		*/
		int x1 = int(x), y1 = int(y), x3 = x1 + 1, y3 = y1 + 1;
		// ���½ǵĵ�
		const float* pLB = m_pSrc + nCurChannel + x1 * m_nChannel + y1 * m_nRowlen;
		// ��Խ��Ĵ���
		return (x1 < 0 || x3 >= m_nWidth || y1 < 0 || y3 >= m_nHeight) ? 0 : 
			(*pLB * (x3 - x) + *(pLB + m_nChannel) * (x - x1)) * (y3 - y) 
			+ (*(pLB + m_nRowlen) * (x3 - x) + *(pLB + m_nChannel + m_nRowlen) * (x - x1)) * (y - y1);
	}

public:
	/**
	* @brief ͼ��任��Ĺ��캯��
	* @param[in] *pSrc ͼ��ָ��
	* @param[in] nWidth ͼ����
	* @param[in] nHeight ͼ��߶�
	* @param[in] nRowlen ͼ��ÿ���ֽ���
	* @param[in] nChannel ͼ��ͨ��
	*/
	ImageTransform(const float* pSrc, int nWidth, int nHeight, int nRowlen, int nChannel) 
	{
		m_pSrc = pSrc;
		m_nWidth = nWidth;
		m_nHeight = nHeight;
		m_nRowlen = nRowlen;
		m_nChannel = nChannel;
		data = NULL;
	}

	~ImageTransform() { if (data) delete [] data; }

	/// Ѱ���ĸ����������
	inline float FindMaxBetween4Numbers(float x, float y, float z, float w) const 
	{
		float Max1, Max2;
		Max1 = x > y ? x : y;
		Max2 = z > w ? z : w;
		return (Max1 > Max2 ? Max1 : Max2);
	}


	/// Ѱ���ĸ�������С��
	inline float FindMinBetween4Numbers(float x, float y, float z, float w) const 
	{
		float Min1, Min2;
		Min1 = x < y ? x : y;
		Min2 = z < w ? z : w;
		return (Min1 < Min2 ? Min1 : Min2);
	}

	// ͼ����ת
	float* ImageRotate(const PositionTransform &pt, int &NewWidth, int &NewHeight, CLogoRect &dstArea) const ;

	// ͼ�����Ȥ����
	float* ImageRoi(const CLogoRect &roi) const ;

	// ͼ������
	float* ImageZoom(int NewWidth, int NewHeight) const ;
};
