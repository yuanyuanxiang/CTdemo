// ChangeBppDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ChangeBppDlg.h"

// CChangeBppDlg 对话框

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

// CChangeBppDlg 消息处理程序

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
	m_pStaticBpp->SetWindowText(_T("当前:") + sCurBpp + _T("位"));

	m_pComboBpp->InsertString(0, L"8位色");
	m_pComboBpp->InsertString(1, L"24位色");
	m_pComboBpp->InsertString(2, L"32位色");
	m_pComboBpp->SetCurSel(1);

	SetIcon(AfxGetApp()->LoadIconW(IDI_DLG_ICON), TRUE);

	::SetWindowPos(this->m_hWnd, HWND_TOP, 100, 100, 240, 150, SWP_SHOWWINDOW);

	return TRUE;  // return TRUE unless you set the focus to a control
}

// 改变图像通道数, bpp：目标图像的位色.
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

// 8位色转24位色
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

// 8位色转32位色
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

// 24位色转8位色
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

// 24位色转32位色
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

// 32位色转8位色
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

// 32位色转24位色
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

// 将二进制图像转换为8位色图像
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
			// *pSrcTemp的第mod二进制位
			BYTE temp = ( *pSrcTemp>>(7 - mod) ) & 1;
			BYTE *pDstTemp = pDst + nChannelNew * j + i * lRowLenNew;
			*pDstTemp = 255 * temp;
		}
	}
}

// 将二进制图像转换为24位色图像
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
			// *pSrcTemp的第mod二进制位
			BYTE temp = ( *pSrcTemp>>(7 - mod) ) & 1;
			BYTE *pDstTemp = pDst + nChannelNew * j + i * lRowLenNew;
			*pDstTemp = *(pDstTemp + 1) = *(pDstTemp + 2) = 255 * temp;
		}
	}
}

// 将二进制图像转换为32位色图像
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
			// *pSrcTemp的第mod二进制位
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
	m_pBnOK->Create(_T("确定"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(20, 40 + BnHeight, 20 + BnWidth, 40 + 2 * BnHeight), this, IDOK);
	m_pBnCancel->Create(_T("取消"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(40 + BnWidth, 40 + BnHeight, 40 + 2 * BnWidth, 40 + 2 * BnHeight), this, IDCANCEL);

	// 字体设置
	m_Font.CreatePointFont(120, _T("宋体"));
	m_pStaticBpp->SetFont(&m_Font);
	m_pComboBpp->SetFont(&m_Font);
	m_pBnOK->SetFont(&m_Font);
	m_pBnCancel->SetFont(&m_Font);
	return 0;
}

// 删除和CImage关联的位图
void SafeDestroy(CImage* pImage)
{
	if (pImage != NULL && !pImage->IsNull())
		pImage->Destroy();
}