#ifndef FUNCTIONS_H
#define FUNCTIONS_H


#include <cmath>
#include <fstream>
using namespace std;

// 获取坐标(x, y)处的值
float GetPositionValue(float *pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, int nCurChannel, int x, int y);

// 插值出浮点(x, y)处的值,双线性插值
float biLinearInterp(float *pSrc, int &nWidth, int &nHegiht, int &nRowlen, int &nChannel, int nCurChannel, float x, float y);

// 对坐标(x, y)旋转angle角度,坐标原点由后两个参数给定
void PositionTransform(float &x, float &y, float theta, float x0, float y0);

// 对坐标(x, y)旋转angle角度, 绕坐标原点旋转
void PositionTransform(float &x, float &y, float theta);

// 找到四个数中最大的
float FindMaxBetween4Numbers(float x, float y, float z, float w);

// 找到四个数中最小的
float FindMinBetween4Numbers(float x, float y, float z, float w);

// 卷积核函数
float ConvKernel(float x, float w0);

// 平行束卷积
void Convolute(float* pDst, float* pSrc, int nWidth, int nHeight, float delta_r, float w0);

// 线性插值
float LinearInterp(float* pPrj, int nWidth, int nHeight, int x, float y);

// 平行束反投影
void BackProject(float* pDst, float* pPrj, int nWidth, int nHeight, int nRays, int nAngles, float delta_r, float delta_fai);

// 扇形束卷积
void Convolute(float* pDst, float* pSrc, int nWidth, int nHeight, float delta_r, float w0, float R, float D);

// 扇形束反投影
void BackProject(float* pDst, float* pPrj, int nWidth, int nHeight, int nRays, int nAngles, float delta_r, float delta_fai, float R, float D);

// 为8位色图像设置颜色表
void SetColorTabFor8BitImage(CImage *pImage);

// 将浮点数写入文本文档
bool Write2File(float* pSrc, int nWidth, int nHeight, CString path);

// 获取直线的自变量
float LineGetXValue(float &k, float &c, float y);

// 获取直线的函数值
float LineGetYValue(float &k, float &c, float x);

// 图像的线积分
float LineIntegrate(float* pSrc, int &Width, int &Height, int &Rowlen, int &Channel, int CurChannel, int Xmin, int Ymin, int Xmax, int Ymax, float &k, float &c);

// 图像radon变换
void ImageRadon(float* pDst, float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, int nCurChannel, float angles_separation, int nAnglesNum, float rays_separation, int nRaysNum);

// 图像沿某个方向线积分
void ImageIntegrate(float* pDst, int nLength, float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, int nCurChannel, float angle, float sub_pixel);

// 图像旋转(以左下角为原点)
float* ImageRotate(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, float angle, int &NewWidth, int &NewHeight, int &NewRowlen);

// 图像旋转，输出参数较少
float* ImageRotate(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, float angle, float x0, float y0, int &NewWidth, int &NewHeight, int &NewRowlen);

// 图像旋转
float* ImageRotate(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, float angle, float x0, float y0, int &Xmin, int &Ymin, int &Xmax, int &Ymax, int &NewWidth, int &NewHeight, int &NewRowlen);

// 图像缩放
float* ImageZoom(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, int NewWidth, int NewHeight);


#endif