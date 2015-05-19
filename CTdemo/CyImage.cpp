#include "stdafx.h"
#include "CyImage.h"
#include "IntSection.h"
#include <vector>
#include <algorithm>
using namespace std;

// 不显示数据截断警告
#pragma warning(disable: 4244)

CyImage::CyImage()
{
	CImage::CImage();
	m_pfFloat = NULL;
	m_fMaximum = 0;
	m_fMinimum = 0;
	m_pyBits = NULL;
	m_nyWidth = 0;
	m_nyHeight = 0;
	m_nyRowlen = 0;
	m_nyRowlen2 = 0;
	m_nyBpp = 0;
	m_nyChannel = 0;
}


CyImage::~CyImage()
{
	SAFE_DELETE(m_pfFloat);
	CImage::~CImage();
}


void CyImage::UpdateInfomation()
{
	if (CHECK_IMAGE_NULL(this))
		return;
	if (!BitMapModified())
		return;
	GetInfomation(m_nyWidth, m_nyHeight, m_nyRowlen, m_nyBpp, m_nyChannel);
	m_nyRowlen2 = m_nyChannel * m_nyWidth;
	m_pyBits = GetHeadAddress();
	InitFloatData();
}


float* CyImage::GetFloatData()
{
	return m_pfFloat;
}


bool CyImage::SetFloatData(float* pSrc, int nRowlen, int nHeight)
{
	if (m_pfFloat == NULL || nRowlen != m_nyRowlen2 || nHeight != m_nyHeight)
		return false;

	for (int i = 0; i < nHeight; ++i)
	{
		memcpy(m_pfFloat, pSrc, nRowlen);
	}
	ComputeMinMax();
	return true;
}


float CyImage::GetMaximum()
{
	return m_fMaximum;
}


float CyImage::GetMinimum()
{
	return m_fMinimum;
}


void CyImage::ComputeMinMax()
{
	m_fMaximum = *m_pfFloat;
	m_fMinimum = *m_pfFloat;

	for (int j = 0; j < m_nyHeight; ++j)
	{
		for (int i = 0; i < m_nyRowlen2; ++i)
		{
			if (m_pfFloat[i + j * m_nyRowlen2] > m_fMaximum)
				m_fMaximum = m_pfFloat[i + j * m_nyRowlen2];
			if (m_pfFloat[i + j * m_nyRowlen2] < m_fMinimum)
				m_fMinimum = m_pfFloat[i + j * m_nyRowlen2];
		}
	}
}


BYTE* CyImage::GetHeadAddress()
{
	return (BYTE*) this->GetBits() + GetPitch() * (GetHeight() - 1);
}


BYTE* CyImage::GetLineAddress(int LineID)
{
	return GetHeadAddress() + LineID * abs(GetPitch());
}


int CyImage::GetChannel()
{
	return m_nyChannel;
}


int CyImage::GetRowlen()
{
	return m_nyRowlen;
}


int CyImage::GetDiagLength()
{
	return (int)ceil(sqrt(1.0f * m_nyWidth * m_nyWidth + m_nyHeight * m_nyHeight));
}


void CyImage::GetInfomation(int &nWidth, int &nHeight)
{
	nWidth = m_nyWidth;
	nHeight = m_nyHeight;
}


void CyImage::GetInfomation(int &nWidth, int &nHeight, int &nRowlen)
{
	nWidth = m_nyWidth;
	nHeight = m_nyHeight;
	nRowlen = m_nyRowlen;
}


void CyImage::GetInfomation(int &nWidth, int &nHeight, int &nRowlen, int &nBPP)
{
	nWidth = m_nyWidth;
	nHeight = m_nyHeight;
	nRowlen = m_nyRowlen;
	nBPP = m_nyBpp;
}


bool CyImage::BitMapModified()
{
	if (m_nyWidth != GetWidth() || m_nyHeight != GetHeight() || m_nyBpp != GetBPP() || m_pyBits != GetHeadAddress())
		return true;
	return false;
}


void CyImage::GetInfomation(int &nWidth, int &nHeight, int &nRowlen, int &nBPP, int &nChannel)
{
	nWidth = GetWidth();
	nHeight = GetHeight();
	nRowlen = abs(GetPitch());
	nBPP = GetBPP();
	nChannel = nBPP / 8;
}


bool CyImage::InitFloatData()
{
	SAFE_DELETE(m_pfFloat);
	try
	{
		m_pfFloat = new float[m_nyHeight * m_nyRowlen2];
	}
	catch (const std::bad_alloc &e)
	{
		return false;
	}
	ZeroMemory(m_pfFloat, m_nyHeight * m_nyRowlen2 * sizeof(float));
	return true;
}


BOOL CyImage::Create(int nWidth, int nHeight, int nBPP, DWORD dwFlags) throw()
{
	this->Destroy();
	BOOL result = CImage::Create(nWidth, nHeight, nBPP, dwFlags);
	if (result == 0)
		return result;
	if (nBPP == 8)
		SetColorTabFor8BitImage(this);
	UpdateInfomation();
	if (dwFlags != 0)
	{
		MemcpyByteToFloat();
		m_fMaximum = m_fMinimum = dwFlags;
	}
	return result;
}


BOOL CyImage::Create(float* pSrc, int nWidth, int nHeight, int nRowlen, DWORD dwFlags) throw()
{
	int nBPP = 8 * nRowlen / nWidth;
	BOOL result = CImage::Create(nWidth, nHeight, nBPP, dwFlags);
	if (result == 0)
		return result;
	if (nBPP == 8)
		SetColorTabFor8BitImage(this);
	UpdateInfomation();
	memcpy(m_pfFloat, pSrc, m_nyHeight * m_nyRowlen2 * sizeof(float));
	MemcpyFloatToByte();
	return result;
}


HRESULT CyImage::Load(LPCTSTR pszFileName) throw()
{
	HRESULT hr = FAILED(CImage::Load(pszFileName));
	if (hr != S_OK)
		return hr;
	UpdateInfomation();
	MemcpyByteToFloat();
	return hr;
}


void CyImage::MemcpyByteToFloat()
{
	BYTE* head = GetHeadAddress();

#pragma omp parallel for
	for (int i = 0; i < m_nyWidth; ++i)
	{
		for (int j = 0; j < m_nyHeight; ++j)
		{
			for (int k = 0; k < m_nyChannel; ++k)
				m_pfFloat[k + i * m_nyChannel + j * m_nyRowlen2] = head[k + i * m_nyChannel + j * m_nyRowlen];
		}
	}
	ComputeMinMax();
}


void CyImage::MemcpyFloatToByte()
{
	ComputeMinMax();
	BYTE* head = GetHeadAddress();
	if (m_fMaximum == m_fMinimum)
	{
		ZeroMemory(head, m_nyHeight * m_nyRowlen);
		return;
	}

#pragma omp parallel for
	for (int i = 0; i < m_nyWidth; ++i)
	{
		for (int j = 0; j < m_nyHeight; ++j)
		{
			for (int k = 0; k < m_nyChannel; ++k)
				head[k + i * m_nyChannel + j * m_nyRowlen] = 255.f * (m_pfFloat[k + i * m_nyChannel + j * m_nyRowlen2] - m_fMinimum) / (m_fMaximum - m_fMinimum);
		}
	}
}


void CyImage::MemcpyFloatToByteBounded(float lower, float upper)
{
	BYTE* head = GetHeadAddress();

#pragma omp parallel for
	for (int i = 0; i < m_nyWidth; ++i)
	{
		for (int j = 0; j < m_nyHeight; ++j)
		{
			for (int k = 0; k < m_nyChannel; ++k)
			{
				if (m_pfFloat[k + i * m_nyChannel + j * m_nyRowlen2] < lower)
					head[k + i * m_nyChannel + j * m_nyRowlen] = lower;
				else if (m_pfFloat[k + i * m_nyChannel + j * m_nyRowlen2] > upper)
					head[k + i * m_nyChannel + j * m_nyRowlen] = upper;
				else
					head[k + i * m_nyChannel + j * m_nyRowlen] = m_pfFloat[k + i * m_nyChannel + j * m_nyRowlen2];
			}
		}
	}
}


float CyImage::GetAt(int x, int y, int nCurChannel)
{
	return GetPositionValue(m_pfFloat, m_nyWidth, m_nyHeight, m_nyRowlen2, m_nyChannel, nCurChannel, x, y);
}


float CyImage::GetAt(float x, float y, int nCurChannel)
{
	return biLinearInterp(m_pfFloat, m_nyWidth, m_nyHeight, m_nyRowlen2, m_nyChannel, nCurChannel, x, y);
}


// 图像旋转angle角度之后图像每个像素的值。Width和Height描述了像素块宽和高。
float* CyImage::Rotate(float angle, int &NewWidth, int &NewHeight, int &NewRowlen)
{
	return ImageRotate(m_pfFloat, m_nyWidth, m_nyHeight, m_nyRowlen2, m_nyChannel, angle, NewWidth, NewHeight, NewRowlen);
}


float* CyImage::Rotate(float angle, float x0, float y0, int &NewWidth, int &NewHeight, int &NewRowlen)
{
	return ImageRotate(m_pfFloat, m_nyWidth, m_nyHeight, m_nyRowlen2, m_nyChannel, angle, x0, y0, NewWidth, NewHeight, NewRowlen);
}


float* CyImage::Rotate(float angle, float x0, float y0, int &Xmin, int &Ymin, int &Xmax, int &Ymax, int &NewWidth, int &NewHeight, int &NewRowlen)
{
	return ImageRotate(m_pfFloat, m_nyWidth, m_nyHeight, m_nyRowlen2, m_nyChannel, angle, x0, y0, Xmin, Ymin, Xmax, Ymax, NewWidth, NewHeight, NewRowlen);
}


// 缩放图像，缩放后宽度与高度由参数指定.
float* CyImage::Zoom(int NewWidth, int NewHeight)
{
	int NewRowlen = m_nyChannel * NewWidth;
	float* pDst = new float[NewRowlen * NewHeight];
	float wRatio = 1.f * m_nyWidth / NewWidth;
	float hRatio = 1.f * m_nyHeight / NewHeight;

#pragma omp parallel for
	for (int i = 0; i < NewWidth; ++i)
	{
		for (int j = 0; j < NewHeight; ++j)
		{
			for (int nChannel = 0; nChannel < m_nyChannel; ++nChannel)
				pDst[nChannel + i * m_nyChannel + j * NewRowlen] = GetAt(i * wRatio, j * hRatio, nChannel);
		}
	}

	return pDst;
}


HRESULT CyImage::Save(LPCTSTR pszFileName, REFGUID guidFileType) const throw()
{
	HRESULT hr = CImage::Save(pszFileName, guidFileType);
	return hr;
}


bool CyImage::Save(REFGUID guidFileType) const throw()
{
	return Save((CWnd*)NULL, guidFileType);
}


bool CyImage::Save(CWnd* pParentWnd, REFGUID guidFileType) const throw()
{
	// 过滤器
	CString strFilter = L"文本文档|*.TXT|所有图像|*.BMP;*.DIB;*.RLE;*.JPG;*.JPEG;*.JPE;*.JFIF;*.GIF;*.TIF;*.TIFF;*.PNG;*.ICO|BMP (*.BMP;*.DIB;*.RLE)|*.BMP;*.DIB;*.RLE|JPEG (*.JPG;*.JPEG;*.JPE;*.JFIF)|*.JPG;*.JPEG;*.JPE;*.JFIF|GIF (*.GIF)|*.GIF|图标 (*.ICO)|*.ICO|所有文件|*.*||";

	// 获取系统时间
	SYSTEMTIME CurTime;
	CString strDate, strTime;
	GetLocalTime(&CurTime);
	strDate.Format(_T("%4d年%2d月%2d日"), CurTime.wYear, CurTime.wMonth, CurTime.wDay);
	strTime.Format(_T("%2d时%2d分%2d秒"), CurTime.wHour, CurTime.wMinute, CurTime.wSecond);

	// 文件对话框
	CFileDialog hFileDlg(FALSE, _T(".BMP"), strDate + strTime, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_READONLY, strFilter, pParentWnd);
	hFileDlg.m_ofn.nFilterIndex = 3;
	hFileDlg.m_ofn.lStructSize = sizeof(OPENFILENAME);
	hFileDlg.m_ofn.lpstrTitle = TEXT("保存图像\0");
	hFileDlg.m_ofn.nMaxFile = MAX_PATH;
	if (hFileDlg.DoModal() != IDOK) return false;

	// 获取保存路径
	CString file = hFileDlg.GetFileName();
	CString path = hFileDlg.GetPathName();
	CString ext = hFileDlg.GetFileExt();
	if (file == _T(""))
		path = path + strDate + strTime;
	if (ext == _T(""))
	{
		switch (hFileDlg.m_ofn.nFilterIndex)
		{
		case 1 :ext = _T(".BMP"); break;
		case 2 :ext = _T(".TXT"); break;
		case 3 :ext = _T(".BMP"); break;
		case 4 :ext = _T(".JPG"); break;
		case 5 :ext = _T(".GIF"); break;
		case 6 :ext = _T(".ICO"); break;
		case 7 :ext = _T(".JPG"); break;
		default:ext = _T(".JPG"); break;
		}
		path = path + ext;
	}
	if (ext == _T("TXT") || ext == _T("txt"))
		return Write2File(m_pfFloat, m_nyRowlen2, m_nyHeight, path);
	else
		return SUCCEEDED(CImage::Save(path, guidFileType));
}


/*
原点是图像左下角顶点
float &k		直线斜率
float &c		直接与y轴截距
*/
float CyImage::Integrate(float &k, float &c, int nCurChannel) 
{
	return LineIntegrate(m_pfFloat, m_nyWidth, m_nyHeight, m_nyRowlen2, m_nyChannel, nCurChannel, 0, 0, m_nyWidth, m_nyHeight, k, c);
}


// 沿着各个方向投影，即计算投影图像
void CyImage::Radon(float* pDst, float angles_separation, int nAnglesNum, float rays_separation, int nRaysNum, int nCurChannel)
{
	ImageRadon(pDst, m_pfFloat, m_nyWidth, m_nyHeight, m_nyRowlen2, m_nyChannel, nCurChannel, angles_separation, nAnglesNum, rays_separation, nRaysNum);
}


// 沿单个方向线积分，单个方向的radon变换
void CyImage::DirIntegrate(float* pDst, int nLength, float angle, float sub_pixel, int nCurChannel)
{
	ImageIntegrate(pDst, nLength, m_pfFloat, m_nyWidth, m_nyHeight, m_nyRowlen2, m_nyChannel, nCurChannel, angle);
}


// 安全销毁所关联的位图
void CyImage::Destroy() throw()
{
	if (this != NULL && !this->IsNull())
	{
		CImage::Destroy();
	}
}