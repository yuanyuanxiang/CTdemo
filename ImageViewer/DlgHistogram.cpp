// DlgHistogram.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageViewer.h"
#include "DlgHistogram.h"
#include "afxdialogex.h"


// CDlgHistogram 对话框

IMPLEMENT_DYNAMIC(CDlgHistogram, CDialogEx)

CDlgHistogram::CDlgHistogram(CWnd* pParent) : CDialogEx(CDlgHistogram::IDD, pParent)
{
	m_pImage = NULL;
	m_fWidthRatio = m_fHeightRatio = 1.0;
	m_nChannelSelected = 0;
}


CDlgHistogram::~CDlgHistogram()
{
	// CImage指针来自外部，无需删除
}


void CDlgHistogram::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


void CDlgHistogram::SetImage(CImage* pImage)
{
	if (NULL == pImage)
	{
		TRACE("Set null pointer!\n");
	}
	m_pImage = pImage;
}


CImage* CDlgHistogram::GetImage()
{
	return m_pImage;
}

BEGIN_MESSAGE_MAP(CDlgHistogram, CDialogEx)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_COMMAND(ID_CHANNEL_RGB, &CDlgHistogram::OnChannelRgb)
	ON_COMMAND(ID_CHANNEL_RED, &CDlgHistogram::OnChannelRed)
	ON_COMMAND(ID_CHANNEL_GREEN, &CDlgHistogram::OnChannelGreen)
	ON_COMMAND(ID_CHANNEL_BLUE, &CDlgHistogram::OnChannelBlue)
	ON_UPDATE_COMMAND_UI(ID_CHANNEL_RGB, &CDlgHistogram::OnUpdateChannelRgb)
	ON_UPDATE_COMMAND_UI(ID_CHANNEL_RED, &CDlgHistogram::OnUpdateChannelRed)
	ON_UPDATE_COMMAND_UI(ID_CHANNEL_GREEN, &CDlgHistogram::OnUpdateChannelGreen)
	ON_UPDATE_COMMAND_UI(ID_CHANNEL_BLUE, &CDlgHistogram::OnUpdateChannelBlue)
	ON_COMMAND(ID_HK_HISTOGRAM_DLG_RGB, &CDlgHistogram::OnHkHistogramDlgRgb)
	ON_COMMAND(ID_HK_HISTOGRAM_DLG_RED, &CDlgHistogram::OnHkHistogramDlgRed)
	ON_COMMAND(ID_HK_HISTOGRAM_DLG_GREEN, &CDlgHistogram::OnHkHistogramDlgGreen)
	ON_COMMAND(ID_HK_HISTOGRAM_DLG_BLUE, &CDlgHistogram::OnHkHistogramDlgBlue)
END_MESSAGE_MAP()


// CDlgHistogram 消息处理程序


// 获取图像直方图
BOOL CDlgHistogram::GetHistogram()
{
	if (m_pImage == NULL)
		return FALSE;

	for (int i = 0; i < 4; ++i)
	{
		memset(m_pfHistogram[i], 0, 256 * sizeof(double));
		memset(m_pfTotalHist[i], 0, 256 * sizeof(double));
		memset(m_V4Transform[i], 0, 256 * sizeof(int));
	}
	// 计算直方图
	int nWidth, nHeight, nRowLen, nBpp, nChannel;
	BYTE *header = (BYTE *)m_pImage->GetBits() + m_pImage->GetPitch() * (m_pImage->GetHeight() - 1);

	nWidth = m_pImage->GetWidth();
	nHeight = m_pImage->GetHeight();
	nRowLen = abs(m_pImage->GetPitch());
	nBpp = m_pImage->GetBPP();
	nChannel = nBpp / 8;

	// 彩色图像
	if (nChannel >= 3)
	{
		for (int i = 0; i < nHeight; i++)
		{
			for (int j = 0; j < nWidth; j++)
			{
				BYTE* pixel = header + j * nChannel + i * nRowLen;
				m_pfHistogram[3][int(* pixel)]++;                                             //Blue
				m_pfHistogram[2][int(*(pixel+1))]++;                                          //Green
				m_pfHistogram[1][int(*(pixel+2))]++;                                          //Red
				if (*pixel == *(pixel+1) && *(pixel+1) == *(pixel+2))
				{
					m_pfHistogram[0][int(*pixel)]++;
				}
				else
				{
					m_pfHistogram[0][int(*pixel*0.299 + *(pixel+1)*0.587 + *(pixel+2)*0.114)]++;  //RGB
				}
			}
		}
	}
	// 灰度图像
	else if(nChannel == 1)
	{
		for (int i = 0; i < nHeight; i++)
		{
			for (int j = 0; j < nWidth; j++)
			{
				BYTE* pixel = header + j * nChannel + i * nRowLen;
				m_pfHistogram[0][int(*pixel)]++;
				m_pfHistogram[1][int(*pixel)]++;
				m_pfHistogram[2][int(*pixel)]++;
				m_pfHistogram[3][int(*pixel)]++;
			}
		}
	}
	else
	{
		MessageBox(_T("图像格式不被支持!"), _T("提示"), MB_OK);
		return FALSE;
	}

	// 归一化
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 256; j++)
		{
			// 计算每个灰度级的概率
			m_pfHistogram[i][j] /= (nWidth * nHeight);
			if (j != 0)
			{
				m_pfTotalHist[i][j] = m_pfTotalHist[i][j-1] + m_pfHistogram[i][j];
			}
			else
			{
				m_pfTotalHist[i][j] = m_pfHistogram[i][j];
			}
			// 计算HE映射表
			m_V4Transform[i][j] = 255 * m_pfTotalHist[i][j] + 0.5;
		}
	}

	return TRUE;
}


BOOL CDlgHistogram::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_bInitialStatus = GetHistogram();
	if (m_bInitialStatus == FALSE)
		MessageBox(_T("直方图初始化失败!"), _T("提示"), MB_OK);

	GetClientRect(&m_ClientRect);
	m_nWidth = m_ClientRect.Width()- 20;
	m_nHeight = m_ClientRect.Height() - 60;
	m_PaintRect.left = 10;
	m_PaintRect.top = 10;
	m_PaintRect.right = 10 + m_nWidth;
	m_PaintRect.bottom = m_ClientRect.bottom - 30;

	SetIcon(AfxGetApp()->LoadIconW(IDI_HISTOGRAM), TRUE);
	::SetWindowPos(GetSafeHwnd(), HWND_NOTOPMOST, 0, 0, 278, 278, SWP_SHOWWINDOW);
	CenterWindow();

	m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_HT_HISTOGRAM_DLG));

	return TRUE;  // return TRUE unless you set the focus to a control
}


/* 设置颜色条的颜色
	Channel		通道
	Strength	颜色强度
*/
void CDlgHistogram::SetColorBarColor(COLORREF &ref, int Channel, int Strength)
{
	switch(Channel)
	{
	case 0:
		Strength = 255 - Strength;
		ref = RGB(255 - Strength, 255 - Strength, 255 - Strength);
		break;
	case 1:
		ref = RGB(Strength, 0, 0);
		break;
	case 2:
		ref = RGB(0, Strength, 0);
		break;
	case 3:
		ref = RGB(0, 0, Strength);
		break;
	default:
		break;
	}
}


/* 设置直方图的颜色
	Channel		通道
	Strength	颜色强度
*/
void CDlgHistogram::SetHistgramColor(COLORREF &ref, int Channel, int Strength)
{
	switch(Channel)
	{
	case 0:
		ref = RGB(255 - Strength, 255 - Strength, 255 - Strength);
		break;
	case 1:
		ref = RGB(Strength, 0, 0);
		break;
	case 2:
		ref = RGB(0, Strength, 0);
		break;
	case 3:
		ref = RGB(0, 0, Strength);
		break;
	default:
		break;
	}
}


void CDlgHistogram::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CPen *pOldPen, newPen;
	COLORREF LineColor;

	// 水平方向颜色条
	for (int i = m_PaintRect.left, k = 0; i < m_PaintRect.right; i++, k++)
	{
		SetColorBarColor(LineColor, m_nChannelSelected, int(k/m_fWidthRatio));
		newPen.CreatePen(PS_SOLID, 1, LineColor);
		pOldPen = dc.SelectObject(&newPen);
		dc.MoveTo(i, m_PaintRect.bottom + 10);
		dc.LineTo(i, m_PaintRect.bottom + 20);
		dc.SelectObject(pOldPen);
		newPen.DeleteObject();
	}

	// 绘制直方图的区域
	dc.Rectangle(m_PaintRect);

	// 如果直方图没有初始化，返回
	if (m_bInitialStatus == FALSE)
		return;

	// 直方图
	SetHistgramColor(LineColor, m_nChannelSelected, 255);
	newPen.CreatePen(PS_SOLID, 1, LineColor);
	pOldPen = dc.SelectObject(&newPen);
	double rate = m_nWidth * m_nHeight * 0.2 / m_fWidthRatio;
	for (int i = 0; i <= m_nWidth; i++)
	{
		int height = m_ClientRect.bottom - 30 - m_pfHistogram[m_nChannelSelected][int(i / m_fWidthRatio)] * rate;
		if (height < 30) height = 30;
		dc.MoveTo(m_PaintRect.left + i, m_ClientRect.bottom - 30);
		dc.LineTo(m_PaintRect.left + i, height);
	}
	dc.SelectObject(pOldPen);
	newPen.DeleteObject();
}


void CDlgHistogram::OnSize(UINT nType, int cx, int cy)
{
	GetClientRect(&m_ClientRect);
	m_nWidth = m_ClientRect.Width()- 20;
	m_nHeight = m_ClientRect.Height() - 60;
	m_PaintRect.right = 10 + m_nWidth;
	m_PaintRect.bottom = m_ClientRect.bottom - 30;
	m_fWidthRatio = m_nWidth / 255.0;
	m_fHeightRatio = m_nHeight / 255.0;
	Invalidate(TRUE);
	CDialogEx::OnSize(nType, cx, cy);
}


void CDlgHistogram::OnChannelRgb()
{
	if (m_nChannelSelected == 0)
		return;
	m_nChannelSelected = 0;
	Invalidate(TRUE);
}


void CDlgHistogram::OnChannelRed()
{
	if (m_nChannelSelected == 1)
		return;
	m_nChannelSelected = 1;
	Invalidate(TRUE);
}


void CDlgHistogram::OnChannelGreen()
{
	if (m_nChannelSelected == 2)
		return;
	m_nChannelSelected = 2;
	Invalidate(TRUE);
}


void CDlgHistogram::OnChannelBlue()
{
	if (m_nChannelSelected == 3)
		return;
	m_nChannelSelected = 3;
	Invalidate(TRUE);
}


void CDlgHistogram::OnUpdateChannelRgb(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nChannelSelected == 0);
}


void CDlgHistogram::OnUpdateChannelRed(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nChannelSelected == 1);
}


void CDlgHistogram::OnUpdateChannelGreen(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nChannelSelected == 2);
}


void CDlgHistogram::OnUpdateChannelBlue(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nChannelSelected == 3);
}


BOOL CDlgHistogram::PreTranslateMessage(MSG* pMsg)
{
	if(::TranslateAccelerator(GetSafeHwnd(), m_hAccel, pMsg))
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDlgHistogram::OnHkHistogramDlgRgb()
{
	OnChannelRgb();
}


void CDlgHistogram::OnHkHistogramDlgRed()
{
	OnChannelRed();
}


void CDlgHistogram::OnHkHistogramDlgGreen()
{
	OnChannelGreen();
}


void CDlgHistogram::OnHkHistogramDlgBlue()
{
	OnChannelBlue();
}


BOOL CDlgHistogram::ContinueModal()
{
	CMenu* pMainMenu = GetMenu();
	CCmdUI cmdUI;
	if (pMainMenu != NULL)
	{
		for (UINT n = 0; n < pMainMenu->GetMenuItemCount(); ++n)
		{
			CMenu* pSubMenu = pMainMenu->GetSubMenu(n);
			cmdUI.m_nIndexMax = pSubMenu->GetMenuItemCount();
			for (UINT i = 0; i < cmdUI.m_nIndexMax; ++i)
			{
				cmdUI.m_nIndex = i;
				cmdUI.m_nID = pSubMenu->GetMenuItemID(i);
				cmdUI.m_pMenu = pSubMenu;
				cmdUI.DoUpdate(this, FALSE);
			}
		}
	}
	return CDialogEx::ContinueModal();
}