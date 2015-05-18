
#include "..\IntSection.h"
#include <iostream>
#include <vector>
using namespace std;

#define SAFE_DELETE(p) { if(p != NULL){ delete [] p; p = NULL; } }	//��ȫɾ��ָ��p

/* AMP��Radon�任.*/
void PanRadon(float* pSrc, int src_width, int src_height, float* pDst, int pan_angles, int pan_rays, float scan_range, float R, float D);

/* ������ת֮�������.*/
float* ComputeRotatedValues(float* pSrc, int src_width, int src_height, float angle, int &Xmin, int &Ymin, int &Xmax, int &Ymax, int &NewWidth, int &NewHeight);

/* ���ֱ�ߵĺ���ֵ.*/
float cudaLineGetXValue(float &k, float &c, float y);

/* ���ֱ�ߵ��Ա���.*/
float cudaLineGetYValue(float &k, float &c, float x);

/* ͼ����ĳ�����߻���.*/
float LineIntegrate(float* pSrc, int Xmin, int Ymin, int Xmax, int Ymax, int Width, int Height, float &k, float &c);

/* ������ԭ����ת�任.*/
void PositionTransform(float &x, float &y, float theta);

/* �����ƹ̶�����ת�任.*/
void PositionTransform(float &x, float &y, float theta, float x0, float y0);

/* ͼ��˫���Բ�ֵ.*/
float biLinearInterp(float *_ptrT, int row, int col, float x, float y);

/* ��ȡͼ��ĳ��ĳ�е�ֵ.*/
float GetPositionValue(float *_ptrT, int row, int col, int x, int y);

/* ���ĸ����ֻ�ȡ���ֵ.*/
float FindMaxBetween4Numbers(float x, float y, float z, float w);

/* ���ĸ����ֻ�ȡ��Сֵ.*/
float FindMinBetween4Numbers(float x, float y, float z, float w);