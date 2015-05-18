#include "stdafx.h"
#include "DllExport.h"
#include "DlgImageViewer.h"
#include "ImageViewer.h"

// ȫ�ֱ���: ͼ��Ԥ���Ի���
//
// ע�⣺extern C �������������ģ�庯����
//


extern "C" __declspec(dllexport) void ImageViewer(CImage* pImage)
{
	CDlgImageViewer dlg(pImage);
	dlg.DoModal();
}


extern "C" __declspec(dllexport) void BYTEViewer(BYTE* ptr, int width, int height, int rowlen)
{
	CDlgImageViewer dlg(ptr, width, height, rowlen);
	dlg.DoModal();
}


extern "C" __declspec(dllexport) void IntViewer(int* ptr, int width, int height, int rowlen)
{
	CDlgImageViewer dlg(ptr, width, height, rowlen);
	dlg.DoModal();
}


extern "C" __declspec(dllexport) void FloatViewer(float* ptr, int width, int height, int rowlen)
{
	CDlgImageViewer dlg(ptr, width, height, rowlen);
	dlg.DoModal();
}


extern "C" __declspec(dllexport) void DoubleViewer(double* ptr, int width, int height, int rowlen)
{
	CDlgImageViewer dlg(ptr, width, height, rowlen);
	dlg.DoModal();
}


extern "C" __declspec(dllexport) void CImageViewer(CImage* pImage, int, int, int)
{
	CDlgImageViewer dlg(pImage);
	dlg.DoModal();
}