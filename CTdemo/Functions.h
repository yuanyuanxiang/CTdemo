#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <cmath>
#include <fstream>
#include <vector>
using namespace std;

// ������
#define CONVOLUTE_KERNEL_COSINE		0
#define CONVOLUTE_KERNEL_RL			1
#define CONVOLUTE_KERNEL_SL			2
#define CONVOLUTE_KERNEL_HAMMING	3

// ��ȡ����(x, y)����ֵ
float GetPositionValue(float *pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, int nCurChannel, int x, int y);

// ��ֵ������(x, y)����ֵ,˫���Բ�ֵ
float biLinearInterp(float *pSrc, int &nWidth, int &nHegiht, int &nRowlen, int &nChannel, int nCurChannel, float x, float y);

// ������(x, y)��תangle�Ƕ�,����ԭ���ɺ�������������
void PositionTransform(float &x, float &y, float theta, float x0, float y0);

// ������(x, y)��תangle�Ƕ�, ������ԭ����ת
void PositionTransform(float &x, float &y, float theta);

// ������(x, y)��תangle�Ƕ�,����ԭ���ɺ�������������
void PositionTransform(float &x, float &y, float cos_sin[2], float x0, float y0);

// ������(x, y)��תangle�Ƕ�, ������ԭ����ת
void PositionTransform(float &x, float &y, float cos_sin[2]);

// ������(x, y)��תangle�Ƕ�,����ԭ���ɺ�������������
void PositionTransform(float &x, float &y, float cos_theta, float sin_theta, float x0, float y0);

// ������(x, y)��תangle�Ƕ�, ������ԭ����ת
void PositionTransform(float &x, float &y, float cos_theta, float sin_theta);

// �ҵ��ĸ���������
float FindMaxBetween4Numbers(float x, float y, float z, float w);

// �ҵ��ĸ�������С��
float FindMinBetween4Numbers(float x, float y, float z, float w);

// ����˺���
float CosineKernel(float x, float w0);

// R_L������:n-̽������ţ�d-̽�������
float R_LKernel(int n, float d);

// S_L������:n-̽������ţ�d-̽�������
float S_LKernel(int n, float d);

// Hamming������:n-̽������ţ�d-̽�������
float HammingKernel(int n, float d, float a);

// ƽ�������
void Convolute(float* pDst, float* pSrc, int nWidth, int nHeight, float delta_r, float w0, int nConvKernel = CONVOLUTE_KERNEL_COSINE);

// һά���Բ�ֵ
float LinearInterp(float* pPrj, int nWidth, float x);

// ��ά���Բ�ֵ
float LinearInterp(float* pPrj, int nWidth, int nHeight, int x, float y);

// ƽ������ͶӰ
void BackProject(float* pDst, float* pPrj, int nWidth, int nHeight, int nRays, int nAngles, float delta_r, float delta_fai);

// ���������
void Convolute(float* pDst, float* pSrc, int nWidth, int nHeight, float delta_r, float w0, float R, float D, int nConvKernel = CONVOLUTE_KERNEL_COSINE);

// ��������ͶӰ
void BackProject(float* pDst, float* pPrj, int nWidth, int nHeight, int nRays, int nAngles, float delta_r, float delta_fai, float R, float D);

// Ϊ8λɫͼ��������ɫ��
void SetColorTabFor8BitImage(CImage *pImage);

// ��������д���ı��ĵ�
bool Write2Txt(float* pSrc, int nWidth, int nHeight, CString path);

bool ReadTxt(float* &pDst, int &nWidth, int &nHeight, CString path);

// ��������д��ר��RAW�ļ�
bool Write2Raw(float* pSrc, int nWidth, int nHeight, CString path);

// ��ȡר��raw��ʽ�ļ�
bool ReadPropRaw(float* &pDst, int &nWidth, int &nHeight, CString path);

// ��ȡһ���������
bool ReadRaw(float* &pDst, int &nWidth, int &nHeight, CString path);

// ��ȡֱ�ߵ��Ա���
float LineGetXValue(float &k, float &c, float y);

// ��ȡֱ�ߵĺ���ֵ
float LineGetYValue(float &k, float &c, float x);

// ͼ����߻���
float LineIntegrate(float* pSrc, int &Width, int &Height, int &Rowlen, int &Channel, int CurChannel, int Xmin, int Ymin, int Xmax, int Ymax, float &k, float &c);

// ͼ��radon�任
void ImageRadon(float* pDst, float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, int nCurChannel, float angles_separation, int nAnglesNum, float rays_separation, int nRaysNum);

// ͼ����ĳ�������߻���
void ImageIntegrate(float* pDst, float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, int nCurChannel, float angle, int &nLength);

// ͼ����ת(�����½�Ϊԭ��)
float* ImageRotate(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, float angle, int &NewWidth, int &NewHeight, int &NewRowlen);

// ͼ����ת�������������
float* ImageRotate(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, float angle, float x0, float y0, int &NewWidth, int &NewHeight, int &NewRowlen);

// ͼ����ת
float* ImageRotate(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, float angle, float x0, float y0, int &Xmin, int &Ymin, int &Xmax, int &Ymax, int &NewWidth, int &NewHeight, int &NewRowlen);

// ͼ������
float* ImageZoom(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, int NewWidth, int NewHeight);

// ����ͼ��Ŀ����߶ȼ������ѵ����߲����������ο���MATLAB
int ComputeRaysNum(int nWidth, int nHeight);

// ����ͼ��
float* DiffImage(float* pPrj, int nRays, int nAngles, float delta_r);

// ������ͶӰͼ��
void DBPImage(float* pDst, float* pPrj, int nWidth, int nHeight, int nRays, int nAngles, float delta_r, float delta_fai, float theta = 0.f);

// ϣ�����ؾ����
float HilbertKernel(float x);

// ��������ϣ��������任
void InverseHilbert(float* pDst, float* pSrc, int nWidth, int nHeight, float delta_r);

void *loadRawFile(CString filename, size_t size);

// ɨ��ĳ��Ŀ¼�����ļ�
vector<CString> ScanDiskFile(const CString strPath);

// ���ļ���ɨ��ĳһ���͵��ļ�����ŵ�������
void ScanFormatFile(CFileFind &find, vector<CString> &vStrAllFiles, CString strPath, CString format);


#endif