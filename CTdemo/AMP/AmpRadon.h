#pragma once;

#include "..\IntSection.h"
#include <amp.h>
#include <amp_math.h>
#include <iostream>
#include <vector>
#include "..\Functions.h"
using namespace std;
using namespace concurrency;
using namespace concurrency::fast_math;


/* Radon�任.*/
void AmpRadon(float* pSrc, int src_width, int src_height, float* pDst, int pan_angles, int pan_rays, float scan_range, float R, float D);

/* ������ת֮�������.*/
vector<float> _AMP_ComputeRotatedValues(array<float, 2> &src_array, int src_width, int src_height, float angle, int &Xmin, int &Ymin, int &Xmax, int &Ymax, int &NewWidth, int &NewHeight);

/* ���ֱ�ߵĺ���ֵ.*/
float _AMP_LineGetXValue(float &k, float &c, float y);

/* ���ֱ�ߵ��Ա���.*/
float _AMP_LineGetYValue(float &k, float &c, float x);

/* ͼ����ĳ�����߻���.*/
float _AMP_LineIntegrate(vector<float> &pSrc, int &Xmin, int &Ymin, int &Xmax, int &Ymax, int &Width, int &Height, float &k, float &c);

/* AMP��������ԭ����ת�任.*/
void _AMP_PositionTransform(float &x, float &y, float theta) restrict(amp);

/* �����ƹ̶�����ת�任.*/
void _AMP_PositionTransform(float &x, float &y, float theta, float x0, float y0) restrict(amp);

/* AMP��ͼ��˫���Բ�ֵ.*/
float _AMP_Interpolate(array<float, 2> &_ptrT, int row, int col, float x, float y) restrict(amp);

/* AMP����ȡͼ��ĳ��ĳ�е�ֵ.*/
float _AMP_GetPositionValue(array<float, 2> &_ptrT, int row, int col, int x, int y) restrict(amp);