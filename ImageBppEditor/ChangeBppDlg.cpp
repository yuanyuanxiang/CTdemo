// ChangeBppDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ChangeBppDlg.h"

// CChangeBppDlg �Ի���

IMPLEMENT_DYNAMIC(CChangeBppDlg, CDialogEx)

CChangeBppDlg::CChangeBppDlg(CWnd* pParent) : CDialogEx(CChangeBppDlg::IDD, pParent)
{
	m_pImage = NULL;
	m_nTargetBpp = 24;
}

CChangeBppDlg::CChangeBppDlg(CImage* pImage, CWnd* pParent) : CDialogEx(CChangeBppDlg::IDD, pParent)
{
	m_pImage = pImage;
	m_nTargetBpp = 24;
}

CChangeBppDlg::~CChangeBppDlg()
{
	SAFE_DELETE(m_pStaticBpp);
	SAFE_DELETE(m_pComboBpp);
	SAFE_DELETE(m_pBnOK);
	SAFE_DELETE(m_pBnCancel);
}

void CChangeBppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_BPP, *m_pStaticBpp);
	DDX_Control(pDX, IDC_COMBO_BPP, *m_pComboBpp);
}

BEGIN_MESSAGE_MAP(CChangeBppDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_BPP, &CChangeBppDlg::OnSelchangeComboBpp)
	ON_BN_CLICKED(IDOK, &CChangeBppDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CChangeBppDlg::OnBnClickCancel)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CChangeBppDlg ��Ϣ�������

void CChangeBppDlg::OnSelchangeComboBpp()
{
	int nSel = m_pComboBpp->GetCurSel();
	switch (nSel)
	{
	case 0 : m_nTargetBpp = 8; break;
	case 1 : m_nTargetBpp = 24;break;
	case 2 : m_nTargetBpp = 32;break;
	default:                   break;
	}
}

void CChangeBppDlg::OnBnClickedOk()
{
	ChangeBPP(m_nTargetBpp);

	::SendMessage(GetSafeHwnd(), WM_CLOSE, 0, 0);

	CDialogEx::OnOK();
}

void CChangeBppDlg::OnBnClickCancel()
{
	::SendMessage(GetSafeHwnd(), WM_CLOSE, 0, 0);

	CDialogEx::OnCancel();
}

BOOL CChangeBppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CString sCurBpp;
	sCurBpp.Format(L"%d", m_pImage->GetBPP());
	m_pStaticBpp->SetWindowText(_T("��ǰ:") + sCurBpp + _T("λ"));

	m_pComboBpp->InsertString(0, L"8λɫ");
	m_pComboBpp->InsertString(1, L"24λɫ");
	m_pComboBpp->InsertString(2, L"32λɫ");
	m_pComboBpp->SetCurSel(1);

	SetIcon(AfxGetApp()->LoadIconW(IDI_DLG_ICON), TRUE);

	::SetWindowPos(this->m_hWnd, HWND_TOP, 100, 100, 240, 150, SWP_SHOWWINDOW);

	return TRUE;  // return TRUE unless you set the focus to a control
}

// �ı�ͼ��ͨ����, bpp��Ŀ��ͼ���λɫ.
BOOL CChangeBppDlg::ChangeBPP(UINT bpp)
{
	if(bpp != 8 && bpp != 24 && bpp != 32)
		return FALSE;

	int nBitCount = m_pImage->GetBPP();
	if (nBitCount == bpp) return TRUE;

	switch (nBitCount)
	{
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
	return TRUE;
}

// 8λɫת24λɫ
void CChangeBppDlg::Bpp8To24()
{
	LONG lWidth = m_pImage->GetWidth();
	LONG lHeight = m_pImage->GetHeight();
	int nChannel = 1, nChannelNew = 3;
	LONG lRowLen = abs(m_pImage->GetPitch()), lRowLenNew = WIDTHBYTES(24 * lWidth);

	BYTE *pSrc = new BYTE[lHeight * lRowLen];
	BYTE *temp = (BYTE*)m_pImage->GetBits() + (m_pImage->GetPitch() * (m_pImage->GetHeight() - 1));
	memcpy(pSrc, temp, lHeight * lRowLen);

	SafeDestroy(m_pImage);
	m_pImage->Create(lWidth, lHeight, 24);

	BYTE *pDst = (BYTE*)m_pImage->GetBits() + (m_pImage->GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		for (int j = 0; j < lWidth; j++)
		{
			BYTE *pSrcTemp = pSrc + nChannel * j + i * lRowLen;
			BYTE *pDstTemp = pDst + nChannelNew * j + i * lRowLenNew;
			*(pDstTemp + 2) = *(pDstTemp + 1) = *pDstTemp = *pSrcTemp;
		}
	}
}

// 8λɫת32λɫ
void CChangeBppDlg::Bpp8To32()
{
	LONG lWidth = m_pImage->GetWidth();
	LONG lHeight = m_pImage->GetHeight();
	int nChannel = 1, nChannelNew = 4;
	LONG lRowLen = abs(m_pImage->GetPitch()), lRowLenNew = WIDTHBYTES(32 * lWidth);

	BYTE *pSrc = new BYTE[lHeight * lRowLen];
	BYTE *temp = (BYTE*)m_pImage->GetBits() + (m_pImage->GetPitch() * (m_pImage->GetHeight() - 1));
	memcpy(pSrc, temp, lHeight * lRowLen);

	SafeDestroy(m_pImage);
	m_pImage->Create(lWidth, lHeight, 32);

	BYTE *pDst = (BYTE*)m_pImage->GetBits() + (m_pImage->GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		for (int j = 0; j < lWidth; j++)
		{
			BYTE *pSrcTemp = pSrc + nChannel * j + i * lRowLen;
			BYTE *pDstTemp = pDst + nChannelNew * j + i * lRowLenNew;
			*(pDstTemp + 2) = *(pDstTemp + 1) = *pDstTemp = *pSrcTemp;
		}
	}
}

// 24λɫת8λɫ
void CChangeBppDlg::Bpp24To8()
{
	LONG lWidth = m_pImage->GetWidth();
	LONG lHeight = m_pImage->GetHeight();
	int nChannel = 3, nChannelNew = 1;
	LONG lRowLen = abs(m_pImage->GetPitch()), lRowLenNew = WIDTHBYTES(8 * lWidth);

	BYTE *pSrc = new BYTE[lHeight * lRowLen];
	BYTE *temp = (BYTE*)m_pImage->GetBits() + (m_pImage->GetPitch() * (m_pImage->GetHeight() - 1));
	memcpy(pSrc, temp, lHeight * lRowLen);

	SafeDestroy(m_pImage);
	m_pImage->Create(lWidth, lHeight, 8);
	RGBQUAD ColorTab[256];
	for(int i = 0; i<256; i++)
	{
		ColorTab[i].rgbBlue = ColorTab[i].rgbGreen = ColorTab[i].rgbRed = i;
	}
	m_pImage->SetColorTable(0, 256, ColorTab);

	BYTE *pDst = (BYTE*)m_pImage->GetBits() + (m_pImage->GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		for (int j = 0; j < lWidth; j++)
		{
			BYTE *pSrcTemp = pSrc + nChannel * j + i * lRowLen;
			BYTE *pDstTemp = pDst + nChannelNew * j + i * lRowLenNew;
			BYTE R = *(pSrcTemp + 2);
			BYTE G = *(pSrcTemp + 1);
			BYTE B = *pSrcTemp;
			*pDstTemp = (9798 * R + 19235 * G + 3735 * B) / 32768;
		}
	}
}

// 24λɫת32λɫ
void CChangeBppDlg::Bpp24To32()
{
	LONG lWidth = m_pImage->GetWidth();
	LONG lHeight = m_pImage->GetHeight();
	int nChannel = 3, nChannelNew = 4;
	LONG lRowLen = abs(m_pImage->GetPitch()), lRowLenNew = WIDTHBYTES(32 * lWidth);

	BYTE *pSrc = new BYTE[lHeight * lRowLen];
	BYTE *temp = (BYTE*)m_pImage->GetBits() + (m_pImage->GetPitch() * (m_pImage->GetHeight() - 1));
	memcpy(pSrc, temp, lHeight * lRowLen);

	SafeDestroy(m_pImage);
	m_pImage->Create(lWidth, lHeight, 32);

	
	BYTE *pDst = (BYTE*)m_pImage->GetBits() + (m_pImage->GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		for (int j = 0; j < lWidth; j++)
		{
			BYTE *pSrcTemp = pSrc + nChannel * j + i * lRowLen;
			BYTE *pDstTemp = pDst + nChannelNew * j + i * lRowLenNew;
			*(pDstTemp + 2) = *(pSrcTemp + 2);
			*(pDstTemp + 1) = *(pSrcTemp + 1);
			*pDstTemp = *pSrcTemp;
		}
	}
}

// 32λɫת8λɫ
void CChangeBppDlg::Bpp32To8()
{
	LONG lWidth = m_pImage->GetWidth();
	LONG lHeight = m_pImage->GetHeight();
	int nChannel = 4, nChannelNew = 1;
	LONG lRowLen = abs(m_pImage->GetPitch()), lRowLenNew = WIDTHBYTES(8 * lWidth);

	BYTE *pSrc = new BYTE[lHeight * lRowLen];
	BYTE *temp = (BYTE*)m_pImage->GetBits() + (m_pImage->GetPitch() * (m_pImage->GetHeight() - 1));
	memcpy(pSrc, temp, lHeight * lRowLen);

	SafeDestroy(m_pImage);
	m_pImage->Create(lWidth, lHeight, 8);

	RGBQUAD ColorTab[256];
	for(int i = 0; i<256; i++)
	{
		ColorTab[i].rgbBlue = ColorTab[i].rgbGreen = ColorTab[i].rgbRed = i;
	}
	m_pImage->SetColorTable(0, 256, ColorTab);

	BYTE *pDst = (BYTE*)m_pImage->GetBits() + (m_pImage->GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		for (int j = 0; j < lWidth; j++)
		{
			BYTE *pSrcTemp = pSrc + nChannel * j + i * lRowLen;
			BYTE *pDstTemp = pDst + nChannelNew * j + i * lRowLenNew;
			BYTE R = *(pSrcTemp + 2);
			BYTE G = *(pSrcTemp + 1);
			BYTE B = *pSrcTemp;
			*pDstTemp = (9798 * R + 19235 * G + 3735 * B) / 32768;
		}
	}
}

// 32λɫת24λɫ
void CChangeBppDlg::Bpp32To24()
{
	LONG lWidth = m_pImage->GetWidth();
	LONG lHeight = m_pImage->GetHeight();
	int nChannel = 4, nChannelNew = 3;
	LONG lRowLen = abs(m_pImage->GetPitch()), lRowLenNew = WIDTHBYTES(24 * lWidth);

	BYTE *pSrc = new BYTE[lHeight * lRowLen];
	BYTE *temp = (BYTE*)m_pImage->GetBits() + (m_pImage->GetPitch() * (m_pImage->GetHeight() - 1));
	memcpy(pSrc, temp, lHeight * lRowLen);

	SafeDestroy(m_pImage);
	m_pImage->Create(lWidth, lHeight, 24);

	BYTE *pDst = (BYTE*)m_pImage->GetBits() + (m_pImage->GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		for (int j = 0; j < lWidth; j++)
		{
			BYTE *pSrcTemp = pSrc + nChannel * j + i * lRowLen;
			BYTE *pDstTemp = pDst + nChannelNew * j + i * lRowLenNew;
			*(pDstTemp + 2) = *(pSrcTemp + 2);
			*(pDstTemp + 1) = *(pSrcTemp + 1);
			*pDstTemp = *pSrcTemp;
		}
	}
}

// ��������ͼ��ת��Ϊ8λɫͼ��
void CChangeBppDlg::Bpp1To8()
{
	LONG lWidth = m_pImage->GetWidth();
	LONG lHeight = m_pImage->GetHeight();
	int nChannelNew = 1;
	LONG lRowLen = abs(m_pImage->GetPitch()), lRowLenNew = WIDTHBYTES(8 * lWidth);

	BYTE *pSrc = new BYTE[lHeight * lRowLen];
	BYTE *temp = (BYTE*)m_pImage->GetBits() + (m_pImage->GetPitch() * (m_pImage->GetHeight() - 1));
	memcpy(pSrc, temp, lHeight * lRowLen);

	SafeDestroy(m_pImage);
	m_pImage->Create(lWidth, lHeight, 8);

	RGBQUAD ColorTab[256];
	for(int i = 0; i<256; i++)
	{
		ColorTab[i].rgbBlue = ColorTab[i].rgbGreen = ColorTab[i].rgbRed = i;
	}
	m_pImage->SetColorTable(0, 256, ColorTab);

	BYTE *pDst = (BYTE*)m_pImage->GetBits() + (m_pImage->GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		for (int j = 0; j < lWidth; j++)
		{
			int mod = j % 8;
			BYTE *pSrcTemp = pSrc + int(j / 8.f) + i * lRowLen;
			// *pSrcTemp�ĵ�mod������λ
			BYTE temp = ( *pSrcTemp>>(7 - mod) ) & 1;
			BYTE *pDstTemp = pDst + nChannelNew * j + i * lRowLenNew;
			*pDstTemp = 255 * temp;
		}
	}
}

// ��������ͼ��ת��Ϊ24λɫͼ��
void CChangeBppDlg::Bpp1To24()
{
	LONG lWidth = m_pImage->GetWidth();
	LONG lHeight = m_pImage->GetHeight();
	int nChannelNew = 3;
	LONG lRowLen = abs(m_pImage->GetPitch()), lRowLenNew = WIDTHBYTES(24 * lWidth);

	BYTE *pSrc = new BYTE[lHeight * lRowLen];
	BYTE *temp = (BYTE*)m_pImage->GetBits() + (m_pImage->GetPitch() * (m_pImage->GetHeight() - 1));
	memcpy(pSrc, temp, lHeight * lRowLen);

	SafeDestroy(m_pImage);
	m_pImage->Create(lWidth, lHeight, 24);

	BYTE *pDst = (BYTE*)m_pImage->GetBits() + (m_pImage->GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		for (int j = 0; j < lWidth; j++)
		{
			int mod = j % 8;
			BYTE *pSrcTemp = pSrc + int(j / 8.f) + i * lRowLen;
			// *pSrcTemp�ĵ�mod������λ
			BYTE temp = ( *pSrcTemp>>(7 - mod) ) & 1;
			BYTE *pDstTemp = pDst + nChannelNew * j + i * lRowLenNew;
			*pDstTemp = *(pDstTemp + 1) = *(pDstTemp + 2) = 255 * temp;
		}
	}
}

// ��������ͼ��ת��Ϊ32λɫͼ��
void CChangeBppDlg::Bpp1To32()
{
	LONG lWidth = m_pImage->GetWidth();
	LONG lHeight = m_pImage->GetHeight();
	int nChannelNew = 4;
	LONG lRowLen = abs(m_pImage->GetPitch()), lRowLenNew = WIDTHBYTES(32 * lWidth);

	BYTE *pSrc = new BYTE[lHeight * lRowLen];
	BYTE *temp = (BYTE*)m_pImage->GetBits() + (m_pImage->GetPitch() * (m_pImage->GetHeight() - 1));
	memcpy(pSrc, temp, lHeight * lRowLen);

	SafeDestroy(m_pImage);
	m_pImage->Create(lWidth, lHeight, 32);

	BYTE *pDst = (BYTE*)m_pImage->GetBits() + (m_pImage->GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		for (int j = 0; j < lWidth; j++)
		{
			int mod = j % 8;
			BYTE *pSrcTemp = pSrc + int(j / 8.f) + i * lRowLen;
			// *pSrcTemp�ĵ�mod������λ
			BYTE temp = ( *pSrcTemp>>(7 - mod) ) & 1;
			BYTE *pDstTemp = pDst + nChannelNew * j + i * lRowLenNew;
			*pDstTemp = *(pDstTemp + 1) = *(pDstTemp + 2) = 255 * temp;
		}
	}
}

int CChangeBppDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pStaticBpp = new CStatic;
	m_pComboBpp = new CComboBox;
	m_pBnOK = new CButton;
	m_pBnCancel = new CButton;

	int BnWidth = 80;
	int BnHeight = 30;
	m_pStaticBpp->Create(_T(""), WS_CHILD | WS_VISIBLE, CRect(20, 20, 20 + BnWidth, 20 + BnHeight), this, IDC_STATIC_BPP);
	m_pComboBpp->Create(WS_CHILD | WS_VISIBLE | CBS_AUTOHSCROLL | CBS_DROPDOWNLIST | WS_EX_TOPMOST, CRect(40 + BnWidth, 20, 40 + 2 * BnWidth, 2 + BnHeight), this, IDC_COMBO_BPP);
	m_pBnOK->Create(_T("ȷ��"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(20, 40 + BnHeight, 20 + BnWidth, 40 + 2 * BnHeight), this, IDOK);
	m_pBnCancel->Create(_T("ȡ��"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(40 + BnWidth, 40 + BnHeight, 40 + 2 * BnWidth, 40 + 2 * BnHeight), this, IDCANCEL);

	// ��������
	m_Font.CreatePointFont(120, _T("����"));
	m_pStaticBpp->SetFont(&m_Font);
	m_pComboBpp->SetFont(&m_Font);
	m_pBnOK->SetFont(&m_Font);
	m_pBnCancel->SetFont(&m_Font);
	return 0;
}

// ɾ����CImage������λͼ
void SafeDestroy(CImage* pImage)
{
	if (pImage != NULL && !pImage->IsNull())
		pImage->Destroy();
}