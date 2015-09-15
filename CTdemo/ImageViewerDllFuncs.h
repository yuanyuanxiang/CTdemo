#include "stdafx.h"

/****************************************************************
	һ���ĵ�˵��
		��̬���ӿ� ImageViewer.dll �ӿں�������

	������ �� ��
		�׶�ʦ����ѧ Ԭ���� 2015��4��22��

	����ʹ�÷���

		1���������ӿ�:	LoadImageViewerDll(HINSTANCE&);
		2��Ѱ�Һ���:	FindImageViewerFun(HINSTANCE&, TYPE*);
		3�����ú���:	ImageViewerProc(TYPE*, int, int, int);
		4���ͷ����ӿ�:	FreeLibrary(HINSTANCE).
	�ġ���㷽��
		ֱ�ӵ���:		PopImageViewerDlg(T*, int, int, int);
*****************************************************************/

bool LoadImageViewerDll(HINSTANCE &hDll)
{
	hDll = LoadLibrary(_T("ImageViewer.dll"));
	if (hDll == NULL)
		return false;
	return true;
}

// 1 CImage*
typedef void (*lpImageViewer)(CImage*);
lpImageViewer ImageViewer;
bool InitImageViewerFun(HINSTANCE &hDll)
{
	ImageViewer = (lpImageViewer)GetProcAddress(hDll, "ImageViewer");
	if (ImageViewer == NULL)
		return false;
	return true;
}

// 2 BYTE*
typedef void (*lpBYTEViewer)(BYTE*, int, int, int);
lpBYTEViewer BYTEViewer;
bool FindImageViewerFun(HINSTANCE &hDll, BYTE*)
{
	BYTEViewer = (lpBYTEViewer)GetProcAddress(hDll, "BYTEViewer");
	if (BYTEViewer == NULL)
		return false;
	return true;
}

// char*
typedef void (*lpCharViewer)(char*, int, int, int);
lpCharViewer CharViewer;
bool FindImageViewerFun(HINSTANCE &hDll, char*)
{
	CharViewer = (lpCharViewer)GetProcAddress(hDll, "BYTEViewer");
	if (CharViewer == NULL)
		return false;
	return true;
}

// 3 int*
typedef void (*lpIntViewer)(int*, int, int, int);
lpIntViewer IntViewer;
bool FindImageViewerFun(HINSTANCE &hDll, int*)
{
	IntViewer = (lpIntViewer)GetProcAddress(hDll, "IntViewer");
	if (IntViewer == NULL)
		return false;
	return true;
}

// 4 float*
typedef void (*lpFloatViewer)(float*, int, int, int);
lpFloatViewer FloatViewer;
bool FindImageViewerFun(HINSTANCE &hDll, float*)
{
	FloatViewer = (lpFloatViewer)GetProcAddress(hDll, "FloatViewer");
	if (FloatViewer == NULL)
		return false;
	return true;
}

// 5 double*
typedef void (*lpDoubleViewer)(double*, int, int, int);
lpDoubleViewer DoubleViewer;
bool FindImageViewerFun(HINSTANCE &hDll, double*)
{
	DoubleViewer = (lpDoubleViewer)GetProcAddress(hDll, "DoubleViewer");
	if (DoubleViewer == NULL)
		return false;
	return true;
}

// 6 CImage*
typedef void (*lpCImageViewer)(CImage*, int, int, int);
lpCImageViewer CImageViewer;
bool FindImageViewerFun(HINSTANCE &hDll, CImage*)
{
	CImageViewer = (lpCImageViewer)GetProcAddress(hDll, "CImageViewer");
	if (CImageViewer == NULL)
		return false;
	return true;
}

// Ϊ�˷���ʹ�ö��������أ�ptr - ����ʾ������, width - ���, height - �߶�, rowlen - ÿ���ֽ�����
void ImageViewerProc(BYTE* ptr, int width, int height, int rowlen)	{	BYTEViewer(ptr, width, height, rowlen);}
void ImageViewerProc(char* ptr, int width, int height, int rowlen)	{	CharViewer(ptr, width, height, rowlen);}
void ImageViewerProc(int* ptr, int width, int height, int rowlen)	{	IntViewer(ptr, width, height, rowlen);}
void ImageViewerProc(float* ptr, int width, int height, int rowlen)	{	FloatViewer(ptr, width, height, rowlen);}
void ImageViewerProc(double* ptr, int width, int height, int rowlen){	DoubleViewer(ptr, width, height, rowlen);}
void ImageViewerProc(CImage* ptr, int width, int height, int rowlen){	CImageViewer(ptr, width, height, rowlen);}

// ����ͼ������Ի���ptr - ����ʾ������, width - ���, height - �߶�, rowlen - ÿ���ֽ�����
template <typename Type> void PopImageViewerDlg(Type* ptr, int width, int height, int rowlen)
{
	HINSTANCE hDll = NULL;
	if (LoadImageViewerDll(hDll) == false)
	{
		AfxMessageBox(_T("\"ImageViewer.dll\"����û�гɹ���"));
		return;
	}
	if (FindImageViewerFun(hDll, ptr) == false)
	{
		AfxMessageBox(_T("\"ImageViewer.dll\"����Ѱ��ʧ�ܣ�"));
		FreeLibrary(hDll);
		return;
	}
	else
	{
		ImageViewerProc(ptr, width, height, rowlen);
	}
	FreeLibrary(hDll);
}