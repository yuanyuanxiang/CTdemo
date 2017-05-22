#pragma once

/** 
* @file ImageTransform.h
* @brief ͼ�񼸺α任���������Կ�ƽ̨
* @author Ԭ����
*/

// ������(x, y)��תangle�Ƕ�,����ԭ���ɺ�������������
void PositionTransform(float &x, float &y, float theta, float x0, float y0);

// ������(x, y)��תangle�Ƕ�, ������ԭ��(0, 0)��ת
void PositionTransform(float &x, float &y, float theta);

// ������(x, y)��תangle�Ƕ�,����ԭ���ɺ�������������
void PositionTransform(float &x, float &y, float cos_sin[2], float x0, float y0);

// ������(x, y)��תangle�Ƕ�, ������ԭ��(0, 0)��ת
void PositionTransform(float &x, float &y, float cos_sin[2]);

// ������(x, y)��תangle�Ƕ�,����ԭ���ɺ�������������
void PositionTransform(float &x, float &y, float cos_theta, float sin_theta, float x0, float y0);

// ������(x, y)��תangle�Ƕ�, ������ԭ��(0, 0)��ת
void PositionTransform(float &x, float &y, float cos_theta, float sin_theta);

// �ҵ��ĸ���������
float FindMaxBetween4Numbers(float x, float y, float z, float w);

// �ҵ��ĸ�������С��
float FindMinBetween4Numbers(float x, float y, float z, float w);

// ͼ����ת
float* ImageRotate(float* pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, float angle, 
				   float x0, float y0, int &Xmin, int &Ymin, int &Xmax, int &Ymax, int &NewWidth, int &NewHeight, int &NewRowlen);

// ͼ��ü�
float* ImageCut(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int nChannel, CLogoRect rect);

// ͼ����ת(�����½�Ϊԭ��)
float* ImageRotate(float* pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, float angle, 
				   int &NewWidth, int &NewHeight, int &NewRowlen);

// ͼ����ת����ĳ��Ϊ����
float* ImageRotate(float* pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, float angle, 
				   float x0, float y0, int &NewWidth, int &NewHeight, int &NewRowlen);

// ͼ������
float* ImageZoom(float* pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, int NewWidth, int NewHeight);