
// ******************************** //
// *********** ����DLL ************ //
// ******************************** //
// ********  2015��3��26��  ******* //
// *****  �׶�ʦ����ѧ Ԭ���� ***** //

#include <afxwin.h>
#include <atlimage.h>

// ��������1������˵����pImage - ��Ҫ��ʾ��ͼ��
extern "C" __declspec(dllexport) void ImageViewer(CImage* pImage);

// ��������2������˵����ptr��width��height��rowlen - ��Ҫ���ӻ������ݼ���ȡ��߶ȡ�ÿ���ֽ�����
extern "C" __declspec(dllexport) void BYTEViewer(BYTE* ptr, int width, int height, int rowlen);

// ��������3������˵����ptr��width��height��rowlen - ��Ҫ���ӻ������ݼ���ȡ��߶ȡ�ÿ���ֽ�����
extern "C" __declspec(dllexport) void IntViewer(int* ptr, int width, int height, int rowlen);

// ��������4������˵����ptr��width��height��rowlen - ��Ҫ���ӻ������ݼ���ȡ��߶ȡ�ÿ���ֽ�����
extern "C" __declspec(dllexport) void FloatViewer(float* ptr, int width, int height, int rowlen);

// ��������5������˵����ptr��width��height��rowlen - ��Ҫ���ӻ������ݼ���ȡ��߶ȡ�ÿ���ֽ�����
extern "C" __declspec(dllexport) void DoubleViewer(double* ptr, int width, int height, int rowlen);

// ��������6������˵����pImage - ��Ҫ��ʾ��ͼ�� Ϊ��ʹ��ʽ������1-5��ʽ���ݡ�
extern "C" __declspec(dllexport) void CImageViewer(CImage* pImage, int, int, int);