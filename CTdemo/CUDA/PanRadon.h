
#include "..\IntSection.h"
#include <iostream>
#include <vector>
using namespace std;

#define SAFE_DELETE(p) { if(p != NULL){ delete [] p; p = NULL; } }	//安全删除指针p

/* AMP：Radon变换.*/
void PanRadon(float* pSrc, int src_width, int src_height, float* pDst, int pan_angles, int pan_rays, float scan_range, float R, float D);

/* 计算旋转之后的数据.*/
float* ComputeRotatedValues(float* pSrc, int src_width, int src_height, float angle, int &Xmin, int &Ymin, int &Xmax, int &Ymax, int &NewWidth, int &NewHeight);

/* 获得直线的函数值.*/
float cudaLineGetXValue(float &k, float &c, float y);

/* 获得直线的自变量.*/
float cudaLineGetYValue(float &k, float &c, float x);

/* 图像沿某方向线积分.*/
float LineIntegrate(float* pSrc, int Xmin, int Ymin, int Xmax, int Ymax, int Width, int Height, float &k, float &c);

/* 坐标绕原点旋转变换.*/
void PositionTransform(float &x, float &y, float theta);

/* 坐标绕固定点旋转变换.*/
void PositionTransform(float &x, float &y, float theta, float x0, float y0);

/* 图像双线性插值.*/
float biLinearInterp(float *_ptrT, int row, int col, float x, float y);

/* 获取图像某行某列的值.*/
float GetPositionValue(float *_ptrT, int row, int col, int x, int y);

/* 从四个数字获取最大值.*/
float FindMaxBetween4Numbers(float x, float y, float z, float w);

/* 从四个数字获取最小值.*/
float FindMinBetween4Numbers(float x, float y, float z, float w);