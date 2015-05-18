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


/* Radon变换.*/
void AmpRadon(float* pSrc, int src_width, int src_height, float* pDst, int pan_angles, int pan_rays, float scan_range, float R, float D);

/* 计算旋转之后的数据.*/
vector<float> _AMP_ComputeRotatedValues(array<float, 2> &src_array, int src_width, int src_height, float angle, int &Xmin, int &Ymin, int &Xmax, int &Ymax, int &NewWidth, int &NewHeight);

/* 获得直线的函数值.*/
float _AMP_LineGetXValue(float &k, float &c, float y);

/* 获得直线的自变量.*/
float _AMP_LineGetYValue(float &k, float &c, float x);

/* 图像沿某方向线积分.*/
float _AMP_LineIntegrate(vector<float> &pSrc, int &Xmin, int &Ymin, int &Xmax, int &Ymax, int &Width, int &Height, float &k, float &c);

/* AMP：坐标绕原点旋转变换.*/
void _AMP_PositionTransform(float &x, float &y, float theta) restrict(amp);

/* 坐标绕固定点旋转变换.*/
void _AMP_PositionTransform(float &x, float &y, float theta, float x0, float y0) restrict(amp);

/* AMP：图像双线性插值.*/
float _AMP_Interpolate(array<float, 2> &_ptrT, int row, int col, float x, float y) restrict(amp);

/* AMP：获取图像某行某列的值.*/
float _AMP_GetPositionValue(array<float, 2> &_ptrT, int row, int col, int x, int y) restrict(amp);