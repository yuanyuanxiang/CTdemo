#ifndef FUNCTIONS_H
#define FUNCTIONS_H


#include <cmath>
#include <fstream>
using namespace std;

// ��ȡ����(x, y)����ֵ
float GetPositionValue(float *pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, int nCurChannel, int x, int y);

// ��ֵ������(x, y)����ֵ,˫���Բ�ֵ
float biLinearInterp(float *pSrc, int &nWidth, int &nHegiht, int &nRowlen, int &nChannel, int nCurChannel, float x, float y);

// ������(x, y)��תangle�Ƕ�,����ԭ���ɺ�������������
void PositionTransform(float &x, float &y, float theta, float x0, float y0);

// ������(x, y)��תangle�Ƕ�, ������ԭ����ת
void PositionTransform(float &x, float &y, float theta);

// �ҵ��ĸ���������
float FindMaxBetween4Numbers(float x, float y, float z, float w);

// �ҵ��ĸ�������С��
float FindMinBetween4Numbers(float x, float y, float z, float w);

// ����˺���
float ConvKernel(float x, float w0);

// ƽ�������
void Convolute(float* pDst, float* pSrc, int nWidth, int nHeight, float delta_r, float w0);

// ���Բ�ֵ
float LinearInterp(float* pPrj, int nWidth, int nHeight, int x, float y);

// ƽ������ͶӰ
void BackProject(float* pDst, float* pPrj, int nWidth, int nHeight, int nRays, int nAngles, float delta_r, float delta_fai);

// ���������
void Convolute(float* pDst, float* pSrc, int nWidth, int nHeight, float delta_r, float w0, float R, float D);

// ��������ͶӰ
void BackProject(float* pDst, float* pPrj, int nWidth, int nHeight, int nRays, int nAngles, float delta_r, float delta_fai, float R, float D);

// Ϊ8λɫͼ��������ɫ��
void SetColorTabFor8BitImage(CImage *pImage);

// ��������д���ı��ĵ�
bool Write2File(float* pSrc, int nWidth, int nHeight, CString path);

// ��ȡֱ�ߵ��Ա���
float LineGetXValue(float &k, float &c, float y);

// ��ȡֱ�ߵĺ���ֵ
float LineGetYValue(float &k, float &c, float x);

// ͼ����߻���
float LineIntegrate(float* pSrc, int &Width, int &Height, int &Rowlen, int &Channel, int CurChannel, int Xmin, int Ymin, int Xmax, int Ymax, float &k, float &c);

// ͼ��radon�任
void ImageRadon(float* pDst, float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, int nCurChannel, float angles_separation, int nAnglesNum, float rays_separation, int nRaysNum);

// ͼ����ĳ�������߻���
void ImageIntegrate(float* pDst, int nLength, float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, int nCurChannel, float angle, float sub_pixel);

// ͼ����ת(�����½�Ϊԭ��)
float* ImageRotate(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, float angle, int &NewWidth, int &NewHeight, int &NewRowlen);

// ͼ����ת�������������
float* ImageRotate(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, float angle, float x0, float y0, int &NewWidth, int &NewHeight, int &NewRowlen);

// ͼ����ת
float* ImageRotate(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, float angle, float x0, float y0, int &Xmin, int &Ymin, int &Xmax, int &Ymax, int &NewWidth, int &NewHeight, int &NewRowlen);

// ͼ������
float* ImageZoom(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, int NewWidth, int NewHeight);


#endif