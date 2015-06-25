#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <cmath>
#include <fstream>
#include <vector>
using namespace std;

// 窗函数
#define CONVOLUTE_KERNEL_COSINE		0
#define CONVOLUTE_KERNEL_RL			1
#define CONVOLUTE_KERNEL_SL			2
#define CONVOLUTE_KERNEL_HAMMING	3

// 获取坐标(x, y)处的值
float GetPositionValue(float *pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, int nCurChannel, int x, int y);

// 插值出浮点(x, y)处的值,双线性插值
float biLinearInterp(float *pSrc, int &nWidth, int &nHegiht, int &nRowlen, int &nChannel, int nCurChannel, float x, float y);

// 对坐标(x, y)旋转angle角度,坐标原点由后两个参数给定
void PositionTransform(float &x, float &y, float theta, float x0, float y0);

// 对坐标(x, y)旋转angle角度, 绕坐标原点旋转
void PositionTransform(float &x, float &y, float theta);

// 对坐标(x, y)旋转angle角度,坐标原点由后两个参数给定
void PositionTransform(float &x, float &y, float cos_sin[2], float x0, float y0);

// 对坐标(x, y)旋转angle角度, 绕坐标原点旋转
void PositionTransform(float &x, float &y, float cos_sin[2]);

// 对坐标(x, y)旋转angle角度,坐标原点由后两个参数给定
void PositionTransform(float &x, float &y, float cos_theta, float sin_theta, float x0, float y0);

// 对坐标(x, y)旋转angle角度, 绕坐标原点旋转
void PositionTransform(float &x, float &y, float cos_theta, float sin_theta);

// 找到四个数中最大的
float FindMaxBetween4Numbers(float x, float y, float z, float w);

// 找到四个数中最小的
float FindMinBetween4Numbers(float x, float y, float z, float w);

// 卷积核函数
float CosineKernel(float x, float w0);

// R_L窗函数:n-探测器编号；d-探测器间距
float R_LKernel(int n, float d);

// S_L窗函数:n-探测器编号；d-探测器间距
float S_LKernel(int n, float d);

// Hamming窗函数:n-探测器编号；d-探测器间距
float HammingKernel(int n, float d, float a);

// 平行束卷积
void Convolute(float* pDst, float* pSrc, int nWidth, int nHeight, float delta_r, float w0, int nConvKernel = CONVOLUTE_KERNEL_COSINE);

// 一维线性插值
float LinearInterp(float* pPrj, int nWidth, float x);

// 二维线性插值
float LinearInterp(float* pPrj, int nWidth, int nHeight, int x, float y);

// 平行束反投影
void BackProject(float* pDst, float* pPrj, int nWidth, int nHeight, int nRays, int nAngles, float delta_r, float delta_fai);

// 扇形束卷积
void Convolute(float* pDst, float* pSrc, int nWidth, int nHeight, float delta_r, float w0, float R, float D, int nConvKernel = CONVOLUTE_KERNEL_COSINE);

// 扇形束反投影
void BackProject(float* pDst, float* pPrj, int nWidth, int nHeight, int nRays, int nAngles, float delta_r, float delta_fai, float R, float D);

// 为8位色图像设置颜色表
void SetColorTabFor8BitImage(CImage *pImage);

// 将浮点数写入文本文档
bool Write2Txt(float* pSrc, int nWidth, int nHeight, CString path);

bool ReadTxt(float* &pDst, int &nWidth, int &nHeight, CString path);

// 将浮点数写入专有RAW文件
bool Write2Raw(float* pSrc, int nWidth, int nHeight, CString path);

// 读取专有raw格式文件
bool ReadPropRaw(float* &pDst, int &nWidth, int &nHeight, CString path);

// 读取一般的裸数据
bool ReadRaw(float* &pDst, int &nWidth, int &nHeight, CString path);

// 获取直线的自变量
float LineGetXValue(float &k, float &c, float y);

// 获取直线的函数值
float LineGetYValue(float &k, float &c, float x);

// 图像的线积分
float LineIntegrate(float* pSrc, int &Width, int &Height, int &Rowlen, int &Channel, int CurChannel, int Xmin, int Ymin, int Xmax, int Ymax, float &k, float &c);

// 图像radon变换
void ImageRadon(float* pDst, float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, int nCurChannel, float angles_separation, int nAnglesNum, float rays_separation, int nRaysNum);

// 图像沿某个方向线积分
void ImageIntegrate(float* pDst, float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, int nCurChannel, float angle, int &nLength);

// 图像旋转(以左下角为原点)
float* ImageRotate(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, float angle, int &NewWidth, int &NewHeight, int &NewRowlen);

// 图像旋转，输出参数较少
float* ImageRotate(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, float angle, float x0, float y0, int &NewWidth, int &NewHeight, int &NewRowlen);

// 图像旋转
float* ImageRotate(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, float angle, float x0, float y0, int &Xmin, int &Ymin, int &Xmax, int &Ymax, int &NewWidth, int &NewHeight, int &NewRowlen);

// 图像缩放
float* ImageZoom(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, int NewWidth, int NewHeight);

// 根据图像的宽度与高度计算出最佳的射线采样条数，参考自MATLAB
int ComputeRaysNum(int nWidth, int nHeight);

// 导数图像
float* DiffImage(float* pPrj, int nRays, int nAngles, float delta_r);

// 导数反投影图像
void DBPImage(float* pDst, float* pPrj, int nWidth, int nHeight, int nRays, int nAngles, float delta_r, float delta_fai, float theta = 0.f);

// 希尔伯特卷积核
float HilbertKernel(float x);

// 有限区域希尔伯特逆变换
void InverseHilbert(float* pDst, float* pSrc, int nWidth, int nHeight, float delta_r);

void *loadRawFile(CString filename, size_t size);

// 扫描某个目录下面文件
vector<CString> ScanDiskFile(const CString strPath);

// 从文件夹扫描某一类型的文件，存放到容器中
void ScanFormatFile(CFileFind &find, vector<CString> &vStrAllFiles, CString strPath, CString format);


#endif