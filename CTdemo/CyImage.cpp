#include "stdafx.h"
#include "CyImage.h"
#include <new>
#include <math.h>

#include <algorithm>
using namespace std;

#include "FileIO.h"
#include "ImageTransform.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/** - swap ����int��Ԫ�صĴ��� - */
void swap(int &nElem1, int &nElem2)
{
	int temp(nElem1);
	nElem1 = nElem2;
	nElem2 = temp;
}


/** - Ϊ8λɫͼ��������ɫ�� - */
void SetColorTabFor8BitImage(CImage *pImage)
{
	ASSERT(pImage && !pImage->IsNull() && 8 == pImage->GetBPP());
	if (8 != pImage->GetBPP())
		return;
	RGBQUAD ColorTab[256];
	for (int i = 0; i < 256; i++)
	{
		ColorTab[i].rgbBlue = ColorTab[i].rgbGreen = ColorTab[i].rgbRed = i;
		ColorTab[i].rgbReserved = 0;
	}
	pImage->SetColorTable(0, 256, ColorTab);
}


// ��file path��ȡfile name(Ĭ�ϴ���׺)
CString GetFileName(CString strFilePath, BOOL bHasPostfix)
{
	int nLength = strFilePath.GetLength();
	int nPos = strFilePath.ReverseFind('\\');
	CString name = strFilePath.Right(nLength - nPos - 1);
	// ��ȡû�к�׺���ļ���
	if (bHasPostfix == FALSE)
	{
		int nDotPos = name.ReverseFind('.');
		name = name.Left(nDotPos);
	}
	return name;
}


// ��file path��ȡfile ext
CString GetFileExt(CString strFilePath)
{
	int nLength = strFilePath.GetLength();
	int nPos = strFilePath.ReverseFind('.');
	CString ext = strFilePath.Right(nLength - nPos - 1);
	return ext;
}


// ����תΪCString
CString Num2String(int nNum)
{
	CString str;
	str.Format(_T("%d"), nNum);
	return str;
}


/** ��Ŀ��ͼ���ڳ�һ�������Դͼ���滻��LOGO����Ϊ3ͨ�� */
void ReplacedLogo(CyImage* pDstImage, CyImage* pSrcImage, CLogoRect LogoRect)
{
	// ����һ��Ϊ�վͷ���
	if (pSrcImage->IsNull() || pSrcImage->GetWidth() < 23 || pDstImage->IsNull())
		return;
	// ��Logo̫��ʱ����
	if (LogoRect.Width() > pDstImage->GetWidth() || LogoRect.Height() > pDstImage->GetHeight())
		return;
	float *pDst = pDstImage->GetFloatDataHead();
	float *pSrc = pSrcImage->Zoom(LogoRect.Width(), LogoRect.Height(), TRUE);
	int nDstRowlen = pDstImage->GetFloatDataRowlen();
	int nSrcRowlen = LogoRect.Width() * 3;
	int nLogoHeight = LogoRect.Height();
	for (int i = 0; i < nLogoHeight; ++i)
	{
		float* pCur = pDst + LogoRect.left * 3 + (i + LogoRect.top) * nDstRowlen;
		memcpy(pCur, pSrc + i * nSrcRowlen, nSrcRowlen * sizeof(float));
	}
	SAFE_DELETE(pSrc);
	pDstImage->MemcpyFloatToByte();
	return;
}


/** ��Ŀ��ͼ���ڳ�һ�������Դͼ���ϣ�dst_rateָ��DstImage��ռ���� */
void MixedLogo(CyImage* pDstImage, CyImage* pSrcImage, CLogoRect LogoRect, float dst_rate)
{
	// ����һ��Ϊ�վͷ���
	if (pDstImage->IsNull() || pSrcImage->IsNull())
		return;
	if (LogoRect.Width() > pDstImage->GetWidth() || LogoRect.Height() > pDstImage->GetHeight())
		return;
	float *pDst = pDstImage->GetFloatDataHead();
	float *pSrc = pSrcImage->Zoom(LogoRect.Width(), LogoRect.Height(), TRUE);
	int src_width, src_height;
	pSrcImage->GetInfomation(src_width, src_height);
	int nDstRowlen = pDstImage->GetFloatDataRowlen();
	int nSrcRowlen = LogoRect.Width() * 3;
	int nLogoWidth = LogoRect.Width();
	int nLogoHeight = LogoRect.Height();
	for (int i = 0; i < nLogoHeight; ++i)
	{
		int y = i * nSrcRowlen, y0 = (i + LogoRect.top) * nDstRowlen;
		for (int j = 0; j < nLogoWidth; ++j)
		{
			int x = 3 * j;
			float* pCur = pDst + (j + LogoRect.left) * 3 + y0;
			pCur[0] = dst_rate * pCur[0] + (1 - dst_rate) * pSrc[0 + x + y];
			pCur[1] = dst_rate * pCur[1] + (1 - dst_rate) * pSrc[1 + x + y];
			pCur[2] = dst_rate * pCur[2] + (1 - dst_rate) * pSrc[2 + x + y];
		}
	}
	SAFE_DELETE(pSrc);
	pDstImage->MemcpyFloatToByte();
	return;
}


/** - Ĭ�ϵĹ��캯�� - */
CyImage::CyImage()
{
	CImage::CImage();
	m_pFloatData = NULL;
	m_fMaximum = 0;
	m_fMinimum = 0;
}


/** - �������� - */
CyImage::~CyImage()
{
	SAFE_DELETE(m_pFloatData);
	CImage::~CImage();
}


/** - �������캯�� - */
CyImage::CyImage(const CyImage &pImage)
{
	m_pFloatData = NULL;
	m_fMaximum = 0;
	m_fMinimum = 0;
	BOOL nRet = Create(pImage.GetWidth(), pImage.GetHeight(), pImage.GetBPP(), 0);
	ASSERT(nRet);
	float *pDst = GetFloatDataHead();
	float *pSrc = pImage.GetFloatDataHead();
	memcpy(pDst, pSrc, GetHeight() * GetFloatDataRowlen() * sizeof(float));
	MemcpyFloatToByte();
}


/** - ��ֵ�������� - */
CyImage CyImage::operator = (const CyImage &pImage)
{
	// �Լ����Լ���ֵ
	if (&pImage == this)
		return *this;
	Destroy();
	m_pFloatData = NULL;
	m_fMaximum = 0;
	m_fMinimum = 0;
	BOOL nRet = Create(pImage.GetWidth(), pImage.GetHeight(), pImage.GetBPP(), 0);
	ASSERT(nRet);
	float *pDst = GetFloatDataHead();
	float *pSrc = pImage.GetFloatDataHead();
	memcpy(pDst, pSrc, GetHeight() * GetFloatDataRowlen() * sizeof(float));
	MemcpyFloatToByte();
	return *this;
}


/** - ��ȡ�������ݵ�ͷָ�� - */
float* CyImage::GetFloatDataHead() const
{
	return m_pFloatData;
}


/** - ��ȡͼ��ÿ�и������ݸ��� - */
int CyImage::GetFloatDataRowlen() const
{
	return GetWidth() * GetChannel();
}


/** - Ϊͼ�񿽱��������� - 
* @param[in] *pSrc ����Դ
* @param[in] nRowlen ����Դÿ�и������
* @param[in] nHeight ����Դ�ĸ߶�
* @return ����
*/
BOOL CyImage::SetFloatData(float* pSrc, int nRowlen, int nHeight)
{
	if (m_pFloatData == NULL || nRowlen != GetFloatDataRowlen() || nHeight != GetHeight())
		return FALSE;

	memcpy(m_pFloatData, pSrc, nRowlen * nHeight * sizeof(float));

	return TRUE;
}


/** - ��ȡ�������ݵ����ֵ - */
float CyImage::GetMaximum() const
{
	return m_fMaximum;
}


/** - ��ȡ�������ݵ���Сֵ - */
float CyImage::GetMinimum() const
{
	return m_fMinimum;
}


/** - ���㸡�����ݵ������Сֵ - */
void CyImage::ComputeMinMax()
{
	// Ѱ�������Сֵ
	pair<float*, float*> minmax_pair = minmax_element(m_pFloatData, 
		m_pFloatData + GetHeight() * GetFloatDataRowlen());

	m_fMinimum = *minmax_pair.first;
	m_fMaximum = *minmax_pair.second;
}


/** - ���ͼ�����ݿ���׵�ַ - */
BYTE* CyImage::GetHeadAddress() const
{
	return (BYTE*) GetBits() + GetPitch() * (GetHeight() - 1);
}


/** - ���ͼ��ĳһ�е��׵�ַ - */
BYTE* CyImage::GetLineAddress(int nLineID) const
{
	return GetHeadAddress() + nLineID * abs(GetPitch());
}


/** - ���ͼ���ͨ������ - */
int CyImage::GetChannel() const
{
	return GetBPP() / 8;
}


/** - ���ͼ������ÿ�е��ֽ��� - */
int CyImage::GetRowlen() const
{
	return abs(GetPitch());
}


/** - ���ͼ�������ܵ��ֽ��� - */
int CyImage::GetLength() const
{
	return GetHeight() * abs(GetPitch());
}


/** - ���ͼ��ĶԽ��ߵĳ��� - */
int CyImage::GetDiagLength() const
{
	return (int)ceil(sqrt(1.0f * GetWidth() * GetWidth() + GetHeight() * GetHeight()));
}


/** - ���ͼ��Ŀ�ȡ��߶� - */
void CyImage::GetInfomation(int &nWidth, int &nHeight) const
{
	nWidth = GetWidth();
	nHeight = GetHeight();
}


/** - ���ͼ��Ŀ�ȡ��߶ȡ�ÿ���ֽ��� - */
void CyImage::GetInfomation(int &nWidth, int &nHeight, int &nRowlen) const
{
	nWidth = GetWidth();
	nHeight = GetHeight();
	nRowlen = abs(GetPitch());
}


/** - ���ͼ��Ŀ�ȡ��߶ȡ�ÿ���ֽ�����ÿ�����ֽ��� - */
void CyImage::GetInfomation(int &nWidth, int &nHeight, int &nRowlen, int &nBPP) const
{
	nWidth = GetWidth();
	nHeight = GetHeight();
	nRowlen = abs(GetPitch());
	nBPP = GetBPP();
}


/** - ���ͼ��Ŀ�ȡ��߶ȡ�ÿ���ֽ�����ÿ�����ֽ�����ͨ������ - */
void CyImage::GetInfomation(int &nWidth, int &nHeight, int &nRowlen, int &nBPP, int &nChannel) const
{
	nWidth = GetWidth();
	nHeight = GetHeight();
	nRowlen = abs(GetPitch());
	nBPP = GetBPP();
	nChannel = nBPP / 8;
}


/** - ��ʼ���������� - */
BOOL CyImage::InitFloatData()
{
	SAFE_DELETE(m_pFloatData);
	int nHeight = GetHeight();
	int nRowlen = GetFloatDataRowlen();
	try
	{
		m_pFloatData = new float[nHeight * nRowlen];
	}
	catch (const std::bad_alloc)
	{
		return FALSE;
	}
	memset(m_pFloatData, 0, nHeight * nRowlen * sizeof(float));
	return TRUE;
}


/** - ��������Դ����ͼ�� - 
* @param[in] *pSrc Դ����
* @param[in] nWidth ͼ����
* @param[in] nHeight ͼ��߶�
* @param[in] nBPP λ���
* @return �ɹ���ʧ��
* @note pSrc�����ݳ��������������ͼ���ֽ���һ�£���:
		length(pSrc) = ��������ͼ���ܵ��ֽ���
*/
BOOL CyImage::Create(const BYTE* pSrc, int nWidth, int nHeight, int nBPP) throw()
{
	Destroy();// ������ͼ������
	if (FALSE == CImage::Create(nWidth, nHeight, nBPP, 0UL))
		return FALSE;
	if (nBPP == 8)
		SetColorTabFor8BitImage(this);
	memcpy(GetHeadAddress(), pSrc, GetLength() * sizeof(BYTE));
	InitFloatData();
	MemcpyByteToFloat();
	return TRUE;
}


/** - ����ͼ�� - 
* @param[in] nWidth ͼ����
* @param[in] nHeight ͼ��߶�
* @param[in] nBPP ͼ��λ���
* @param[in] dwFlags ͼ���ֵ
* @return �ɹ���ʧ��
*/
BOOL CyImage::Create(int nWidth, int nHeight, int nBPP, DWORD dwFlags) throw()
{
	Destroy();
	if (FALSE == CImage::Create(nWidth, nHeight, nBPP, dwFlags))
		return FALSE;
	if (nBPP == 8)
		SetColorTabFor8BitImage(this);
	InitFloatData();
	MemcpyByteToFloat();
	return TRUE;
}


/** - ������Դ����ͼ�� - 
* @param[in] *pSrc ����Դ
* @param[in] nWidth ͼ����
* @param[in] nHeight ͼ��߶�
* @param[in] nRowlen ����Դÿ�����ݸ���
* @return �ɹ���ʧ��
*/
BOOL CyImage::Create(const float* pSrc, int nWidth, int nHeight, int nRowlen) throw()
{
	Destroy();
	int nBPP = 8 * nRowlen / nWidth;
	if (FALSE == CImage::Create(nWidth, nHeight, nBPP))
		return FALSE;
	if (nBPP == 8)
		SetColorTabFor8BitImage(this);
	InitFloatData();
	memcpy(m_pFloatData, pSrc, nHeight * nRowlen * sizeof(float));
	MemcpyFloatToByte();
	return TRUE;
}


/** - ��ImageSrc����ͼ�� - */
BOOL CyImage::Create(const ImageSrc *pSrc) throw()
{
	Destroy();// ������ͼ������
	if (FALSE == CImage::Create(pSrc->GetWidth(), pSrc->GetHeight(), pSrc->GetBPP(), 0UL))
		return FALSE;
	if (pSrc->GetBPP() == 8)
		SetColorTabFor8BitImage(this);
	memcpy(GetHeadAddress(), pSrc->GetHeadAddress(), GetLength() * sizeof(BYTE));
	InitFloatData();
	MemcpyByteToFloat();
	return TRUE;
}


/** - ����ͼ�� - */
HRESULT CyImage::Load(LPCTSTR pszFileName) throw()
{
	// ����ļ�ΪTXT
	CString strPathName(pszFileName);
	CString strExt = GetFileExt(strPathName);
	strExt.MakeUpper();
	if (strExt == _T("TXT"))
	{
		int nWidth = 0, nHeight = 0, nRowlen = 0, nChannel = 0;
		USES_CONVERSION;
		BYTE *pSrc = ReadTxt(W2A(strPathName), nWidth, nHeight, nRowlen, nChannel);
		if (pSrc == NULL)
			return E_FAIL;
		Create(nWidth, nHeight, 8 * nChannel, 0UL);
		memcpy(GetHeadAddress(), pSrc, nRowlen * nHeight);
		MemcpyByteToFloat();
		SAFE_DELETE(pSrc);
		return S_OK;
	}
	// �ݻٵ�ǰͼ��
	Destroy();
	// ���ؽ�ͼ��
	HRESULT hr = CImage::Load(pszFileName);
	if (hr != S_OK)
		return hr;
	InitFloatData();
	MemcpyByteToFloat();
	return S_OK;
}


/** - ͬ������ - */
void CyImage::MemcpyByteToFloat()
{
	BYTE* pHead = GetHeadAddress();
	float fRate = (m_fMaximum - m_fMinimum) / 255.f;
	float fMinimum = m_fMinimum;
	if (m_fMaximum == m_fMinimum)
	{
		fRate = 1.f;
		fMinimum = 0.f;
	}

	int nWidth = GetWidth();
	int nHeight = GetHeight();
	int nChannel = GetChannel();
	int nRowlen = GetRowlen();
	int nFloatRowlen = GetFloatDataRowlen();
	if (NULL == m_pFloatData)
	{
		m_pFloatData = new float[nHeight * nFloatRowlen];
	}
	for (int i = 0; i < nWidth; ++i)
	{
		int x = i * nChannel;
		for (int j = 0; j < nHeight; ++j)
		{
			int y = j * nRowlen, z = j * nFloatRowlen;
			for (int k = 0; k < nChannel; ++k)
				m_pFloatData[k + x + z] = fRate * pHead[k + x + y] + fMinimum;
		}
	}
	ComputeMinMax();
}


/** - ͬ������ - */
void CyImage::MemcpyFloatToByte()
{
	ComputeMinMax();
	BYTE* pHead = GetHeadAddress();
	// ������������ǳ���
	if (m_fMaximum == m_fMinimum)
	{
		TRACE(" * ���棺��������ȫ����һ����!\n");
		BYTE val = 0;
		if (0 <= m_fMinimum && m_fMinimum <= 255)
		{
			val = (BYTE)m_fMinimum;
		}
		memset(pHead, val, GetHeight() * GetRowlen() * sizeof(BYTE));
		return;
	}

	float fRate = 255.f / (m_fMaximum - m_fMinimum);

	int nWidth = GetWidth();
	int nHeight = GetHeight();
	int nChannel = GetChannel();
	int nRowlen = GetRowlen();
	int nFloatRowlen = GetFloatDataRowlen();

	for (int i = 0; i < nWidth; ++i)
	{
		int x = i * nChannel;
		for (int j = 0; j < nHeight; ++j)
		{
			int y = j * nRowlen, z = j * nFloatRowlen;
			for (int k = 0; k < nChannel; ++k)
				pHead[k + x + y] = BYTE(fRate * (m_pFloatData[k + x + z] - m_fMinimum));
		}
	}
}


/** - ͬ������ - 
* @param[in] lower ����
* @param[in] upper ����
* @return ��
* @note �ú����Ὣ�����Ԫ����Ϊ�߽�ֵ
*/
void CyImage::MemcpyFloatToByteBounded(float lower, float upper)
{
	BYTE* pHead = GetHeadAddress();

	int nWidth = GetWidth();
	int nHeight = GetHeight();
	int nChannel = GetChannel();
	int nRowlen = GetRowlen();
	int nFloatRowlen = GetFloatDataRowlen();

	for (int i = 0; i < nWidth; ++i)
	{
		int x = i * nChannel;
		for (int j = 0; j < nHeight; ++j)
		{
			int y = j * nRowlen, z = j * nFloatRowlen;
			for (int k = 0; k < nChannel; ++k)
			{
				if (m_pFloatData[k + x + z] < lower)
					pHead[k + x + j * nRowlen] = (BYTE)lower;
				else if (m_pFloatData[k + x + z] > upper)
					pHead[k + x + y] = (BYTE)upper;
				else
					pHead[k + x + y] = (BYTE)m_pFloatData[k + x + z];
			}
		}
	}
}


/** - ��ͼ����תһ���Ƕ� - */
void CyImage::Rotate(float degree)
{
	float angle = RAD(degree);
	int NewWidth = 0;
	int NewHeight = 0;
	int NewRowlen = 0;
	InitFloatData();
	MemcpyByteToFloat();
	float *pDst = ImageRotate(m_pFloatData, GetWidth(), GetHeight(), 
		GetFloatDataRowlen(), GetChannel(), angle,NewWidth, NewHeight, NewRowlen);
	Create(pDst, NewWidth, NewHeight, NewRowlen);
	SAFE_DELETE(pDst);
}


/** - ��תͼ�񣬲�������ת�õ���ͼ������ - 
* @param[in] angle ��ת�Ƕ�
* @param[out] &NewWidth ��ͼ����
* @param[out] &NewHeight ��ͼ��߶�
* @param[out] &NewRowlen ��ͼ��ÿ�и���Ԫ�ظ���
* @return ��ͼ�����ݿ�ͷָ��
*/
float* CyImage::Rotate(float angle, int &NewWidth, int &NewHeight, int &NewRowlen)
{
	return ImageRotate(m_pFloatData, GetWidth(), GetHeight(), GetFloatDataRowlen(), 
		GetChannel(), angle, NewWidth, NewHeight, NewRowlen);
}


/** - ��תͼ�񣬲�������ת�õ���ͼ������ -
* @param[in] angle ��ת�Ƕ�
* @param[in] x0 ��ת����
* @param[in] y0 ��ת����
* @param[out] &NewWidth ��ͼ����
* @param[out] &NewHeight ��ͼ��߶�
* @param[out] &NewRowlen ��ͼ��ÿ�и���Ԫ�ظ���
* @return ��ͼ�����ݿ�ͷָ��
*/
float* CyImage::Rotate(float angle, float x0, float y0, int &NewWidth, int &NewHeight, int &NewRowlen)
{
	return ImageRotate(m_pFloatData, GetWidth(), GetHeight(), 
		GetFloatDataRowlen(), GetChannel(), angle, x0, y0, NewWidth, NewHeight, NewRowlen);
}


/** - ��תͼ�񣬲�������ת�õ���ͼ������ -
* @param[in] angle ��ת�Ƕ�
* @param[in] x0 ��ת����
* @param[in] y0 ��ת����
* @param[in] Xmin ͼ��߽�
* @param[in] Xmax ͼ��߽�
* @param[in] Ymin ͼ��߽�
* @param[in] YMax ͼ��߽�
* @param[out] &NewWidth ��ͼ����
* @param[out] &NewHeight ��ͼ��߶�
* @param[out] &NewRowlen ��ͼ��ÿ�и���Ԫ�ظ���
* @return ��ͼ�����ݿ�ͷָ��
*/
float* CyImage::Rotate(float angle, float x0, float y0, int &Xmin, int &Ymin, int &Xmax, int &Ymax, 
					   int &NewWidth, int &NewHeight, int &NewRowlen)
{
	return ImageRotate(m_pFloatData, GetWidth(), GetHeight(), GetFloatDataRowlen(), GetChannel(), 
		angle, x0, y0, Xmin, Ymin, Xmax, Ymax, NewWidth, NewHeight, NewRowlen);
}


/** - ����ͼ�� -
* @param[in] NewWidth ��ͼ����
* @param[in] NewHeight ��ͼ��߶�
* @param[in] bNeededReturn ��Ҫ������ͼ�����ݿ�ͷָ��
* @return ��ͼ�����ݿ�ͷָ��
*/
float* CyImage::Zoom(int NewWidth, int NewHeight, int bNeededReturn)
{
	return ImageZoom(m_pFloatData, GetWidth(), GetHeight(), GetFloatDataRowlen(), 
		GetChannel(), NewWidth, NewHeight);
}


/** - ����ͼ�� -
* @param[in] rate ���ű���
* @return ��
*/
void CyImage::Zoom(float rate)
{
	int NewWidth = int(GetWidth() * rate);
	int NewHeight = int(GetHeight() * rate);
	Zoom(NewWidth, NewHeight);
}


/** - ����ͼ�� -
* @param[in] NewWidth ��ͼ����
* @param[in] NewHeight ��ͼ��߶�
* @return ��
*/
void CyImage::Zoom(int nNewWidth, int nNewHeight)
{
	if (nNewWidth == GetWidth() && nNewHeight == GetHeight())
		return;
	float *pDst = ImageZoom(m_pFloatData, GetWidth(), GetHeight(), GetFloatDataRowlen(), 
		GetChannel(), nNewWidth, nNewHeight);
	Create(pDst, nNewWidth, nNewHeight, nNewWidth * GetChannel());
	SAFE_DELETE(pDst);
}


/** - ͼ��ת��Ϊ8λ�Ҷ�ͼ - */
void CyImage::ToGray()
{
	ChangeBPP(8);
}


/** - ����ͼ�� - */
HRESULT CyImage::Save(LPCTSTR pszFileName, REFGUID guidFileType) const throw()
{
	// ���Ա���Ϊtxt�ļ�
	if (GetFileExt(pszFileName).CompareNoCase(_T("txt")) == 0)
	{
		USES_CONVERSION;
		if (WriteTxt(W2A(pszFileName), GetHeadAddress(), GetWidth(), GetHeight(), GetChannel()))
			return S_OK;
	}
	HRESULT hr = CImage::Save(pszFileName, guidFileType);
	return hr;
}


#ifdef _AFX // MFC �����±���


/** - ����ͼ�� - ����ѡ���ļ��Ի��� */
HRESULT CyImage::Load(CWnd* pParentWnd) throw()
{
	// ������
	CString strFilter = L"����ͼ��|*.BMP;*.DIB;*.RLE;*.JPG;*.JPEG;*.JPE;*.JFIF;*.GIF;*.TIF;"\
		L"*.TIFF;*.PNG;*.ICO|BMP (*.BMP;*.DIB;*.RLE)|*.BMP;*.DIB;*.RLE|JPEG (*.JPG;*.JPEG;"	\
		L"*.JPE;*.JFIF)|*.JPG;*.JPEG;*.JPE;*.JFIF|GIF (*.GIF)|*.GIF|ͼ�� (*.ICO)|*.ICO|�����ļ�|*.*||";

	// �ļ��Ի���
	CFileDialog hFileDlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, strFilter, pParentWnd);
	hFileDlg.m_ofn.nFilterIndex = 1;
	hFileDlg.m_ofn.lStructSize = sizeof(OPENFILENAME);
	hFileDlg.m_ofn.lpstrTitle = TEXT("��ͼ��\0");
	hFileDlg.m_ofn.nMaxFile = MAX_PATH;
	if (hFileDlg.DoModal() != IDOK)
		return E_FAIL;

	// ��������Load����
	CString strFilePath = hFileDlg.GetPathName();
	return Load(strFilePath);
}


/** - ����ͼ�� - */
BOOL CyImage::Save(REFGUID guidFileType) const throw()
{
	return Save((CWnd*)NULL, guidFileType);
}


/** - ����ͼ�� - 
* @details 
guidFileType
The file type to save the image as. Can be one of the following:
ImageFormatBMP   An uncompressed bitmap image.
ImageFormatPNG   A Portable Network Graphic (PNG) compressed image.
ImageFormatJPEG   A JPEG compressed image.
ImageFormatGIF   A GIF compressed image.
Note   For a complete list of constants, see Image File Format Constants in the Platform SDK.
*/
BOOL CyImage::Save(CWnd* pParentWnd, REFGUID guidFileType) const throw()
{
	// ������
	CString strFilter = L"����ͼ��|*.BMP;*.DIB;*.RLE;*.JPG;*.JPEG;*.JPE;*."	\
		L"JFIF;*.GIF;*.TIF;*.TIFF;*.PNG;*.ICO|BMP (*.BMP;*.DIB;*.RLE)|*.BMP;"\
		L"*.DIB;*.RLE|JPEG (*.JPG;*.JPEG;*.JPE;*.JFIF)|*.JPG;*.JPEG;*.JPE;"	\
		L"*.JFIF|GIF (*.GIF)|*.GIF|ͼ�� (*.ICO)|*.ICO|�����ļ�|*.*||";

	// ��ȡϵͳʱ��
	SYSTEMTIME CurTime;
	CString strDate, strTime;
	GetLocalTime(&CurTime);
	strDate.Format(_T("%4d��%2d��%2d��"), CurTime.wYear, CurTime.wMonth, CurTime.wDay);
	strTime.Format(_T("%2dʱ%2d��%2d��"), CurTime.wHour, CurTime.wMinute, CurTime.wSecond);

	// �ļ��Ի���
	CFileDialog hFileDlg(FALSE, _T(".BMP"), strDate + strTime, OFN_FILEMUSTEXIST 
		| OFN_PATHMUSTEXIST | OFN_READONLY, strFilter, pParentWnd);
	hFileDlg.m_ofn.nFilterIndex = 1;
	hFileDlg.m_ofn.lStructSize = sizeof(OPENFILENAME);
	hFileDlg.m_ofn.lpstrTitle = TEXT("����ͼ��\0");
	hFileDlg.m_ofn.nMaxFile = MAX_PATH;
	if (hFileDlg.DoModal() != IDOK)
		return TRUE;

	// ��ȡ����·��
	CString strFile = hFileDlg.GetFileName();
	CString strPath = hFileDlg.GetPathName();
	CString strExt = hFileDlg.GetFileExt();
	if (strFile.IsEmpty())
		strPath = strPath + strDate + strTime;
	if (strExt.IsEmpty())
	{
		switch (hFileDlg.m_ofn.nFilterIndex)
		{
		case 1 :strExt = _T("BMP"); break;
		case 2 :strExt = _T("BMP"); break;
		case 3 :strExt = _T("JPG"); break;
		case 4 :strExt = _T("GIF"); break;
		case 5 :strExt = _T("ICO"); break;
		case 6 :strExt = _T("JPG"); break;
		default:strExt = _T("JPG"); break;
		}
		strPath = strPath + _T(".") + strExt;
	}
	
	return SUCCEEDED(Save(strPath, guidFileType));
}


#endif // _AFX


/** - ��ȫ������������λͼ - */
void CyImage::Destroy() throw()
{
	if (!IsNull())
	{
		CImage::Destroy();
		SAFE_DELETE(m_pFloatData);
	}
}


/** - ˮƽ��תͼ�� - */
void CyImage::FlipH()
{
	int nWidth = GetWidth();
	int nHeight = GetHeight();
	int nChannel = GetChannel();
	int nRowlen = GetRowlen();

	BYTE* temp = new BYTE[nHeight * nRowlen];
	BYTE* head = GetHeadAddress();
	memcpy(temp, head, nHeight * nRowlen * sizeof(BYTE));

	for (int r = 0; r < nHeight; ++r)
	{
		int y = r * nRowlen;
		for (int c = 0; c < nWidth; ++c)
		{
			memcpy(head + c *nChannel + y, temp + (nWidth - 1 - c) * nChannel + y, 
				nChannel * sizeof(BYTE));
		}
	}
	SAFE_DELETE(temp);
}


/** - ��ֱ��תͼ�� - */
void CyImage::FlipV()
{
	int nHeight = GetHeight();
	int nRowlen = GetRowlen();

	BYTE* temp = new BYTE[nHeight * nRowlen * sizeof(BYTE)];
	BYTE* head = GetHeadAddress();
	memcpy(temp, head, nHeight * nRowlen * sizeof(BYTE));

	for (int r = 0; r < nHeight; ++r)
	{
		memcpy(head + r * nRowlen, temp + (nHeight - 1 - r) * nRowlen, nRowlen * sizeof(BYTE));
	}
	SAFE_DELETE(temp);
}


/** - ת��ͼ�� - */
void CyImage::Transpose()
{
	int nWidth = GetWidth();
	int nHeight = GetHeight();
	int nRowlen = GetRowlen();
	int nChannel = GetChannel();
	InitFloatData();
	MemcpyByteToFloat();
	int nFloatRowlen = GetFloatDataRowlen();

	float* temp = new float[nHeight * nFloatRowlen];
	memcpy(temp, m_pFloatData, nHeight * nFloatRowlen * sizeof(float));
	BYTE* temp2 = new BYTE[nHeight * nRowlen];
	memcpy(temp2, GetHeadAddress(), nHeight * nRowlen * sizeof(BYTE));
	Create(nHeight, nWidth, nChannel * 8, 0UL);

	int nNewWidth = GetWidth();
	int nNewHeight = GetHeight();
	int nNewFloatRowlen = GetFloatDataRowlen();
	BYTE *pHead = GetHeadAddress();
	int nNewRowlen = GetRowlen();
	for (int i = 0; i < nNewHeight; ++i)
	{
		int y = i * nNewRowlen, z = i * nNewFloatRowlen;
		for (int j = 0; j < nNewWidth; ++j)
		{
			int x = j * nChannel, x0 = i * nChannel;
			int y0 = j * nRowlen, z0 = j * nFloatRowlen;
			for (int k = 0; k < nChannel; ++k)
			{
				m_pFloatData[k + x + z] = temp[k + x0 + z0];
				pHead[k + x + y] = temp2[k + x0 + y0];
			}
		}
	}
	SAFE_DELETE(temp);
	SAFE_DELETE(temp2);
}


/** - ��ֱ��ͼ�������� - */
void CyImage::GetCluster(int cluster[256]) const
{
	int hist[256];
	GetGrayHist(hist);

	for (int i = 0; i < 256; ++i)
	{
		cluster[i] = i;
	}

	for (int i = 0; i < 256; i++)
	{
		for (int j = i; j < 256; ++j)
		{
			if (hist[j] > hist[i])
			{
				// ��������
				swap(hist[j], hist[i]);
				swap(cluster[j], cluster[i]);
			}
		}
	}
}


/** - ��ȡ�Ҷ�ֱ��ͼ - */
BOOL CyImage::GetGrayHist(int hist[256]) const
{
	ZeroMemory(hist, 256 * sizeof(int));
	int nChannel = GetChannel();
	int nWidth = GetWidth();
	int nHeight = GetHeight();
	int nRowlen = GetRowlen();

	BYTE *pSrc = GetHeadAddress();
	if (nChannel == 1)
	{
		for (int i = 0; i < nHeight; ++i)
		{
			int y = i * nRowlen;
			for (int j = 0; j < nWidth; ++j)
			{
				hist[pSrc[j * nChannel + y]]++;
			}
		}
	}
	else if (nChannel == 3 || nChannel == 4)
	{
		for (int i = 0; i < nHeight; ++i)
		{
			int y = i * nRowlen;
			for (int j = 0; j < nWidth; ++j)
			{
				int index = RGB2GRAY (pSrc[2 + j * nChannel + y], 
					pSrc[1 + j * nChannel + y], pSrc[j * nChannel + y]);
				hist[index]++;
			}
		}
	}
	return TRUE;
}


/** - ��ȡͼ��ֱ��ͼ - */
BOOL CyImage::GetHistogram(int hist[4][256]) const
{
	ZeroMemory(hist, 4 * 256 * sizeof(int));
	int nChannel = GetChannel();
	int nWidth = GetWidth();
	int nHeight = GetHeight();
	int nRowlen = GetRowlen();

	BYTE *pSrc = GetHeadAddress();
	if (nChannel == 1)
	{
		for (int i = 0; i < nHeight; ++i)
		{
			int y = i * nRowlen;
			for (int j = 0; j < nWidth; ++j)
			{
				hist[0][pSrc[j * nChannel + y]]++;
				hist[1][pSrc[j * nChannel + y]]++;
				hist[2][pSrc[j * nChannel + y]]++;
				hist[3][pSrc[j * nChannel + y]]++;
			}
		}
	}
	else if (nChannel == 3 || nChannel == 4)
	{
		for (int i = 0; i < nHeight; ++i)
		{
			int y = i * nRowlen;
			for (int j = 0; j < nWidth; ++j)
			{
				int x = j * nChannel;
				hist[2][pSrc[x + y]]++;
				hist[1][pSrc[1 + x + y]]++;
				hist[0][pSrc[2 + x + y]]++;
				int index = RGB2GRAY(pSrc[2 + x + y], pSrc[1 + x + y], pSrc[x + y]);
				hist[3][index]++;
			}
		}
	}
	return TRUE;
}


/** - �ı�ͼ��ͨ���� -  bpp��Ŀ��ͼ���λɫ - 
�޸ĳɹ�����TRUE��δ�޸ķ���FALSE��
*/
BOOL CyImage::ChangeBPP(UINT bpp)
{
	if(bpp != 8 && bpp != 24 && bpp != 32)
		return FALSE;

	int nBitCount = GetBPP();
	if (nBitCount == bpp) return FALSE;

	switch (nBitCount)
	{
	case 1:
		if (bpp == 8)     Bpp1To8();
		else if(bpp == 24)Bpp1To24();
		else if(bpp == 32)Bpp1To32();
		break;
	case 8:
		if (bpp == 24)    Bpp8To24();
		else if(bpp == 32)Bpp8To32();
		break;
	case 16:
		break;
	case 24:
		if (bpp == 8)     Bpp24To8();
		else if(bpp == 32)Bpp24To32();
		break;
	case 32:
		if (bpp == 8)     Bpp32To8();
		else if(bpp == 24)Bpp32To24();
		break;
	default:
		break;
	}
	MemcpyByteToFloat();
	return TRUE;
}


/// 8λɫת24λɫ
void CyImage::Bpp8To24()
{
	LONG lWidth = GetWidth();
	LONG lHeight = GetHeight();
	int nChannel = 1, nChannelNew = 3;
	LONG lRowLen = abs(GetPitch()), lRowLenNew = WIDTHBYTES(24 * lWidth);

	BYTE *pSrc = new BYTE[lHeight * lRowLen];
	BYTE *temp = (BYTE*)GetBits() + (GetPitch() * (GetHeight() - 1));
	memcpy(pSrc, temp, lHeight * lRowLen);

	Create(lWidth, lHeight, 24);

	BYTE *pDst = (BYTE*)GetBits() + (GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		int y1 = i * lRowLen, y2 = i * lRowLenNew;
		for (int j = 0; j < lWidth; j++)
		{
			BYTE *pSrcTemp = pSrc + nChannel * j + y1;
			BYTE *pDstTemp = pDst + nChannelNew * j + y2;
			*(pDstTemp + 2) = *(pDstTemp + 1) = *pDstTemp = *pSrcTemp;
		}
	}
	SAFE_DELETE(pSrc);
}

/// 8λɫת32λɫ
void CyImage::Bpp8To32()
{
	LONG lWidth = GetWidth();
	LONG lHeight = GetHeight();
	int nChannel = 1, nChannelNew = 4;
	LONG lRowLen = abs(GetPitch()), lRowLenNew = WIDTHBYTES(32 * lWidth);

	BYTE *pSrc = new BYTE[lHeight * lRowLen];
	BYTE *temp = (BYTE*)GetBits() + (GetPitch() * (GetHeight() - 1));
	memcpy(pSrc, temp, lHeight * lRowLen);

	Create(lWidth, lHeight, 32);

	BYTE *pDst = (BYTE*)GetBits() + (GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		int y1 = i * lRowLen, y2 = i * lRowLenNew;
		for (int j = 0; j < lWidth; j++)
		{
			BYTE *pSrcTemp = pSrc + nChannel * j + y1;
			BYTE *pDstTemp = pDst + nChannelNew * j + y2;
			*(pDstTemp + 2) = *(pDstTemp + 1) = *pDstTemp = *pSrcTemp;
		}
	}
	SAFE_DELETE(pSrc);
}

/// 24λɫת8λɫ
void CyImage::Bpp24To8()
{
	LONG lWidth = GetWidth();
	LONG lHeight = GetHeight();
	int nChannel = 3, nChannelNew = 1;
	LONG lRowLen = abs(GetPitch()), lRowLenNew = WIDTHBYTES(8 * lWidth);

	BYTE *pSrc = new BYTE[lHeight * lRowLen];
	BYTE *temp = (BYTE*)GetBits() + (GetPitch() * (GetHeight() - 1));
	memcpy(pSrc, temp, lHeight * lRowLen);

	Create(lWidth, lHeight, 8);

	BYTE *pDst = (BYTE*)GetBits() + (GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		int y1 = i * lRowLen, y2 = i * lRowLenNew;
		for (int j = 0; j < lWidth; j++)
		{
			BYTE *pSrcTemp = pSrc + nChannel * j + y1;
			BYTE *pDstTemp = pDst + nChannelNew * j + y2;
			BYTE R = *(pSrcTemp + 2);
			BYTE G = *(pSrcTemp + 1);
			BYTE B = *pSrcTemp;
			*pDstTemp = RGB2GRAY(R, G, B);
		}
	}
	SAFE_DELETE(pSrc);
}

/// 24λɫת32λɫ
void CyImage::Bpp24To32()
{
	LONG lWidth = GetWidth();
	LONG lHeight = GetHeight();
	int nChannel = 3, nChannelNew = 4;
	LONG lRowLen = abs(GetPitch()), lRowLenNew = WIDTHBYTES(32 * lWidth);

	BYTE *pSrc = new BYTE[lHeight * lRowLen];
	BYTE *temp = (BYTE*)GetBits() + (GetPitch() * (GetHeight() - 1));
	memcpy(pSrc, temp, lHeight * lRowLen);

	Create(lWidth, lHeight, 32);

	BYTE *pDst = (BYTE*)GetBits() + (GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		int y1 = i * lRowLen, y2 = i * lRowLenNew;
		for (int j = 0; j < lWidth; j++)
		{
			BYTE *pSrcTemp = pSrc + nChannel * j + y1;
			BYTE *pDstTemp = pDst + nChannelNew * j + y2;
			*(pDstTemp + 2) = *(pSrcTemp + 2);
			*(pDstTemp + 1) = *(pSrcTemp + 1);
			*pDstTemp = *pSrcTemp;
		}
	}
	SAFE_DELETE(pSrc);
}

/// 32λɫת8λɫ
void CyImage::Bpp32To8()
{
	LONG lWidth = GetWidth();
	LONG lHeight = GetHeight();
	int nChannel = 4, nChannelNew = 1;
	LONG lRowLen = abs(GetPitch()), lRowLenNew = WIDTHBYTES(8 * lWidth);

	BYTE *pSrc = new BYTE[lHeight * lRowLen];
	BYTE *temp = (BYTE*)GetBits() + (GetPitch() * (GetHeight() - 1));
	memcpy(pSrc, temp, lHeight * lRowLen);

	Create(lWidth, lHeight, 8);

	BYTE *pDst = (BYTE*)GetBits() + (GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		int y1 = i * lRowLen, y2 = i * lRowLenNew;
		for (int j = 0; j < lWidth; j++)
		{
			BYTE *pSrcTemp = pSrc + nChannel * j + y1;
			BYTE *pDstTemp = pDst + nChannelNew * j + y2;
			BYTE R = *(pSrcTemp + 2);
			BYTE G = *(pSrcTemp + 1);
			BYTE B = *pSrcTemp;
			*pDstTemp = RGB2GRAY(R, G, B);
		}
	}
	SAFE_DELETE(pSrc);
}

/// 32λɫת24λɫ
void CyImage::Bpp32To24()
{
	LONG lWidth = GetWidth();
	LONG lHeight = GetHeight();
	int nChannel = 4, nChannelNew = 3;
	LONG lRowLen = abs(GetPitch()), lRowLenNew = WIDTHBYTES(24 * lWidth);

	BYTE *pSrc = new BYTE[lHeight * lRowLen];
	BYTE *temp = (BYTE*)GetBits() + (GetPitch() * (GetHeight() - 1));
	memcpy(pSrc, temp, lHeight * lRowLen);

	Create(lWidth, lHeight, 24);

	BYTE *pDst = (BYTE*)GetBits() + (GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		int y1 = i * lRowLen, y2 = i * lRowLenNew;
		for (int j = 0; j < lWidth; j++)
		{
			BYTE *pSrcTemp = pSrc + nChannel * j + y1;
			BYTE *pDstTemp = pDst + nChannelNew * j + y2;
			*(pDstTemp + 2) = *(pSrcTemp + 2);
			*(pDstTemp + 1) = *(pSrcTemp + 1);
			*pDstTemp = *pSrcTemp;
		}
	}
	SAFE_DELETE(pSrc);
}

/// ��������ͼ��ת��Ϊ8λɫͼ��
void CyImage::Bpp1To8()
{
	LONG lWidth = GetWidth();
	LONG lHeight = GetHeight();
	int nChannelNew = 1;
	LONG lRowLen = abs(GetPitch()), lRowLenNew = WIDTHBYTES(8 * lWidth);

	BYTE *pSrc = new BYTE[lHeight * lRowLen];
	BYTE *temp = (BYTE*)GetBits() + (GetPitch() * (GetHeight() - 1));
	memcpy(pSrc, temp, lHeight * lRowLen);

	Create(lWidth, lHeight, 8);

	BYTE *pDst = (BYTE*)GetBits() + (GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		int y1 = i * lRowLen, y2 = i * lRowLenNew;
		for (int j = 0; j < lWidth; j++)
		{
			int mod = j % 8;
			BYTE *pSrcTemp = pSrc + int(j / 8.f) + y1;
			// *pSrcTemp�ĵ�mod������λ
			BYTE temp = ( *pSrcTemp>>(7 - mod) ) & 1;
			BYTE *pDstTemp = pDst + nChannelNew * j + y2;
			*pDstTemp = 255 * temp;
		}
	}
	SAFE_DELETE(pSrc);
}

/// ��������ͼ��ת��Ϊ24λɫͼ��
void CyImage::Bpp1To24()
{
	LONG lWidth = GetWidth();
	LONG lHeight = GetHeight();
	int nChannelNew = 3;
	LONG lRowLen = abs(GetPitch()), lRowLenNew = WIDTHBYTES(24 * lWidth);

	BYTE *pSrc = new BYTE[lHeight * lRowLen];
	BYTE *temp = (BYTE*)GetBits() + (GetPitch() * (GetHeight() - 1));
	memcpy(pSrc, temp, lHeight * lRowLen);

	Create(lWidth, lHeight, 24);

	BYTE *pDst = (BYTE*)GetBits() + (GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		int y1 = i * lRowLen, y2 = i * lRowLenNew;
		for (int j = 0; j < lWidth; j++)
		{
			int mod = j % 8;
			BYTE *pSrcTemp = pSrc + int(j / 8.f) + y1;
			// *pSrcTemp�ĵ�mod������λ
			BYTE temp = ( *pSrcTemp>>(7 - mod) ) & 1;
			BYTE *pDstTemp = pDst + nChannelNew * j + y2;
			*pDstTemp = *(pDstTemp + 1) = *(pDstTemp + 2) = 255 * temp;
		}
	}
	SAFE_DELETE(pSrc);
}

/// ��������ͼ��ת��Ϊ32λɫͼ��
void CyImage::Bpp1To32()
{
	LONG lWidth = GetWidth();
	LONG lHeight = GetHeight();
	int nChannelNew = 4;
	LONG lRowLen = abs(GetPitch()), lRowLenNew = WIDTHBYTES(32 * lWidth);

	BYTE *pSrc = new BYTE[lHeight * lRowLen];
	BYTE *temp = (BYTE*)GetBits() + (GetPitch() * (GetHeight() - 1));
	memcpy(pSrc, temp, lHeight * lRowLen);

	Create(lWidth, lHeight, 32);

	BYTE *pDst = (BYTE*)GetBits() + (GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		int y1 = i * lRowLen, y2 = i * lRowLenNew;
		for (int j = 0; j < lWidth; j++)
		{
			int mod = j % 8;
			BYTE *pSrcTemp = pSrc + int(j / 8.f) + y1;
			// *pSrcTemp�ĵ�mod������λ
			BYTE temp = ( *pSrcTemp>>(7 - mod) ) & 1;
			BYTE *pDstTemp = pDst + nChannelNew * j + y2;
			*pDstTemp = *(pDstTemp + 1) = *(pDstTemp + 2) = 255 * temp;
		}
	}
	SAFE_DELETE(pSrc);
}


/** - ��������ͼ�� - ��Ҫdelete���� - */
CyImage* CyImage::MakeCopy()
{
	CyImage* pCopy = NULL;
	try
	{
		pCopy = new CyImage(*this);
	}
	catch (const std::bad_alloc)
	{
	}
	
	return pCopy;
}


/** - ��ͼ��ı����ָ���� - 
* @param[in] threahold ��ֵ
* @param[in] background ��������
* @return ����ͼ��
* @note �ú�������ֵ��Ҫdelete����
*/
CyImage* CyImage::GetBackground(float threahold, float3 background)
{
	CyImage* pBackground = MakeCopy();
	ASSERT(NULL != pBackground);

	int nWidth = pBackground->GetWidth();
	int nHeight = pBackground->GetHeight();
	int nChannel = pBackground->GetChannel();
	int nRowlen = pBackground->GetFloatDataRowlen();
	float* pHead = pBackground->GetFloatDataHead();
	float fGray = background.ToGray();
	float fMAX = pBackground->GetMaximum();
	switch (nChannel)
	{
	case 1:// 8λɫͼ��
		{
			for (int i = 0; i < nHeight; ++i)
			{
				int y = i * nRowlen;
				for (int j = 0; j < nWidth; ++j)
				{
					// ��ǰ��ɫȫ���óɰ�ɫ
					if (fabsf(pHead[j + y] - fGray) > threahold)
					{
						pHead[j + i * nRowlen] = fMAX;
					}
				}
			}
			break;
		}
	case 3:// ��ɫͼ��
	case 4:
		{
			for (int i = 0; i < nHeight; ++i)
			{
				int y = i * nRowlen;
				for (int j = 0; j < nWidth; ++j)
				{
					int x = j * nChannel;
					float3 current(pHead[2 + x + y], pHead[1 + x + y], pHead[x + y]);
					if (Distance(current, background) > threahold)
					{
						pHead[x + y] = pHead[1 + x + y] = pHead[2 + x + y] = fMAX;
					}
				}
			}
			break;
		}
	default:
		break;
	}
	pBackground->MemcpyFloatToByte();
	return pBackground;
}


/** - ���ݾ���������ȡ����Ȥ�����ͼ�� - */
CyImage* CyImage::ROI(CLogoRect rect)
{
	if (rect == CLogoRect(0, 0, 0, 0))
		return MakeCopy();
	float *pSrc = GetFloatDataHead();
	int nWidth = GetWidth();
	int nHeight = GetHeight();
	int nRowlen = GetRowlen();
	int nChannel = GetChannel();
	float *pDst = ImageCut(pSrc, nWidth, nHeight, nRowlen, nChannel, rect);
	CyImage *pImage = NULL;
	try
	{
		pImage = new CyImage;
	}
	catch (const std::bad_alloc)
	{
		SAFE_DELETE(pDst);
		return pImage;
	}
	pImage->Create(pDst, nWidth, nHeight, nRowlen);
	SAFE_DELETE(pDst);
	return pImage;
}


/** - �ü�ͼ�� - */
void CyImage::Cut(CLogoRect rect)
{
	/// ���������������Ҫ�ü�
	if (rect == CLogoRect(0, 0, 0, 0))
		return;
	if (rect.left == 0 && rect.top == 0 && rect.right == GetWidth() && rect.bottom == GetHeight())
		return;
	/// �����������ø���Ȥ���򣬲���ֵ����ǰͼ��
	int nWidth = GetWidth();
	int nHeight = GetHeight();
	int nRowlen = GetRowlen();
	float *pDst = ImageCut(GetFloatDataHead(), nWidth, nHeight, nRowlen, GetChannel(), rect);
	Create(pDst, nWidth, nHeight, nRowlen);
	SAFE_DELETE(pDst);
}