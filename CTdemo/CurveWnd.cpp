// AdjustCurveWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "CurveWnd.h"
#include "CurveDlg.h"
#include "CTdemoView.h"
#include "CTdemo.h"

#define BUTTON_WIDTH       40
#define BUTTON_HEIGHT      20

// 不显示数据截断警告
#pragma warning(disable: 4244)

// CCurveWnd

IMPLEMENT_DYNAMIC(CCurveWnd, CWnd)

// 构造
CCurveWnd::CCurveWnd()
{
	for (int i = 0; i<4; i++)
	{
		m_pfHistogram[i] = NULL;
	}
	m_pComboBoxRGB = NULL;
	m_pBnGroupBox = NULL;
	m_pBnChooseCurve = NULL;
	m_pBnChooseLine = NULL;
	m_pBnReset = NULL;
	m_pBnInverse = NULL;
	m_pBnImportCurve = NULL;
	m_pBnExportCurve = NULL;
	m_pBnOK = NULL;
	m_pBnCancel = NULL;
	m_pXPosition = NULL;
	m_pYPosition = NULL;
	m_pXlabel = NULL;
	m_pYlabel = NULL;
	m_pOnPaintRect = NULL;
	m_nChannelSelected = CHANNEL_RGB;//默认通道：RGB
	m_bUserWillDrag = FALSE;
	m_posCurrentPeg = NULL;
	m_nLinePattern = LINE_PATTERN_SPLINE;
	for (int i = 0; i < 4; ++i)
	{
		m_bInverseImage[i] = FALSE;
	}
	//图像信息
	m_pBits = NULL;
	m_dataSrc = NULL;
	m_curData = NULL;
	m_nImageWidth = 0;
	m_nImageHeight = 0;
	m_nPixelQuantity = 0;
	m_nbytesWidth = 0;
	m_nChannel = 0;
	m_nlenData = 0;
	//窗口
	m_pWndSize = NULL;
	//字体
	m_FontName = L"宋体";
	m_nFontSize = 100;
	//其他
	m_bModified = false;
	m_nNewRowlen = 0;
	m_nNewChannel = 0;
	m_nNewlenData = 0;
	//初始化图像指针
	m_pImage = NULL;
}

// 析构
CCurveWnd::~CCurveWnd()
{
	for (int i = 0; i<4; i++)
	{
		if (m_pfHistogram[i] != NULL)
		{
			delete [] m_pfHistogram[i];
		}

		m_V4Transform[i].~vector();

		POSITION pos = m_pPegsList[i].GetHeadPosition();
		while (pos != NULL)
		{
			CPoint *Temp = m_pPegsList[i].GetNext(pos).m_pt;
			delete Temp;
		}
		m_pPegsList[i].RemoveAll();
	}
	if (m_pComboBoxRGB != NULL)  {	delete m_pComboBoxRGB;}
	if (m_pBnGroupBox != NULL)   {	delete m_pBnGroupBox;}
	if (m_pBnChooseCurve != NULL){	delete m_pBnChooseCurve;}
	if (m_pBnChooseLine != NULL) {	delete m_pBnChooseLine;}
	if (m_pBnReset != NULL)      {	delete m_pBnReset;}
	if (m_pBnInverse != NULL)    {	delete m_pBnInverse;}
	if (m_pBnImportCurve != NULL){	delete m_pBnImportCurve;}
	if (m_pBnExportCurve != NULL){	delete m_pBnExportCurve;}
	if (m_pBnOK != NULL)         {	delete m_pBnOK;}
	if (m_pBnCancel != NULL)     {	delete m_pBnCancel;}
	if (m_pXPosition != NULL)    {	delete m_pXPosition;}
	if (m_pYPosition != NULL)    {	delete m_pYPosition;}
	if (m_pXlabel != NULL)       {	delete m_pXlabel;}
	if (m_pYlabel != NULL)       {	delete m_pYlabel;}
	if (m_pOnPaintRect != NULL)  {	delete m_pOnPaintRect;}
	if (m_dataSrc != NULL)       {  delete [] m_dataSrc;}
	if (m_curData != NULL)       {  delete [] m_curData;}
	if (m_pWndSize != NULL)      {  delete m_pWndSize;}
	if (m_pAppFont != NULL)      {	delete m_pAppFont;}
}

BEGIN_MESSAGE_MAP(CCurveWnd, CWnd)
	//创建、绘制消息映射
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	//鼠标消息映射
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
	//按钮消息映射
	ON_CBN_SELCHANGE(IDC_COMBOBOX_RGB, &CCurveWnd::OnComboboxRgb)
	ON_BN_CLICKED(IDB_CHOOSE_CURVE, OnChooseCurve)
	ON_BN_CLICKED(IDB_CHOOSE_LINE, &CCurveWnd::OnChooseLine)
	ON_BN_CLICKED(IDB_CurveWnd_RESET, &CCurveWnd::OnCmdCurveWndReset)
	ON_BN_CLICKED(IDB_CurveWnd_INVERSE, &CCurveWnd::OnCmdCurveWndInverse)
	ON_BN_CLICKED(IDB_IMPORT_CURVE, &CCurveWnd::OnCmdImportCurve)
	ON_BN_CLICKED(IDB_EXPORT_CURVE, &CCurveWnd::OnCmdExportCurve)
	ON_BN_CLICKED(IDB_CURVE_OK, &CCurveWnd::OnCmdCurveOk)
	ON_BN_CLICKED(IDB_CURVE_CANCEL, &CCurveWnd::OnCmdCurveCancel)
END_MESSAGE_MAP()

// CCurveWnd 消息处理程序

int CCurveWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pAppFont = new CFont;
	m_pAppFont->CreatePointFont(m_nFontSize, m_FontName);

	m_pWndSize = new CRect;
	GetClientRect(m_pWndSize);

	int nButtonPosX1 = m_pWndSize->right-50;
	int nButtonPosX2 = nButtonPosX1+BUTTON_WIDTH;
	d_type Space = m_pWndSize->Height()/9.0;

	//绘图区Rect
	m_pOnPaintRect = new CRect(40, 40, m_pWndSize->right-70, m_pWndSize->bottom-80);

	//宽高相对于255缩放比
	m_nRectWidth = m_pOnPaintRect->Width();
	m_nRectHeight = m_pOnPaintRect->Height();
	m_fWidthRatio = m_nRectWidth/255.0;
	m_fHeightRatio = m_nRectHeight/255.0;

	//Group Box
	m_pBnGroupBox = new CButton;
	m_pBnGroupBox->Create(L"通道", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, CRect(20, 20, m_pWndSize->right-60, m_pWndSize->bottom-60), this, IDB_GROUP_BOX);
	m_pBnGroupBox->SetFont(m_pAppFont, TRUE);

	//通道:RGB, 组合框下拉列表高度在此设置
	m_pComboBoxRGB = new CComboBox;
	m_pComboBoxRGB->Create(WS_CHILD | WS_VISIBLE | CBS_AUTOHSCROLL | CBS_DROPDOWNLIST | WS_EX_TOPMOST, CRect(70, 15, 130, 115), this, IDC_COMBOBOX_RGB);
	m_pComboBoxRGB->AddString(L"RGB");
	m_pComboBoxRGB->AddString(L"红");
	m_pComboBoxRGB->AddString(L"绿");
	m_pComboBoxRGB->AddString(L"蓝");
	m_pComboBoxRGB->SetCurSel(0);
	m_pComboBoxRGB->SetFont(m_pAppFont, TRUE);

	//选择曲线
	m_pBnChooseCurve = new CMFCButton;
	m_pBnChooseCurve->Create(L"曲线", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(nButtonPosX1, int(Space), nButtonPosX2, int(Space+BUTTON_HEIGHT)), this, IDB_CHOOSE_CURVE);
	m_pBnChooseCurve->SetFont(m_pAppFont, TRUE);
	m_pBnChooseCurve->SetTooltip(L"三次样条插值");
	m_pBnChooseCurve->EnableWindow(FALSE);

	//选择直线
	m_pBnChooseLine = new CMFCButton;
	m_pBnChooseLine->Create(L"直线", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(nButtonPosX1, int(2*Space), nButtonPosX2, int(2*Space+BUTTON_HEIGHT)), this, IDB_CHOOSE_LINE);
	m_pBnChooseLine->SetFont(m_pAppFont, TRUE);
	m_pBnChooseLine->SetTooltip(L"分段线性插值");
	m_pBnChooseLine->EnableWindow(TRUE);

	//重置
	m_pBnReset = new CMFCButton;
	m_pBnReset->Create(L"重置", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(nButtonPosX1, int(3*Space), nButtonPosX2, int(3*Space+BUTTON_HEIGHT)), this, IDB_CurveWnd_RESET);
	m_pBnReset->SetFont(m_pAppFont, TRUE);
	m_pBnReset->SetTooltip(L"撤销所有操作");

	//反转
	m_pBnInverse = new CMFCButton;
	m_pBnInverse->Create(L"反转", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(nButtonPosX1, int(4*Space), nButtonPosX2, int(4*Space+BUTTON_HEIGHT)), this, IDB_CurveWnd_INVERSE);
	m_pBnInverse->SetFont(m_pAppFont, TRUE);
	m_pBnInverse->SetTooltip(L"反转图像");

	//导入曲线
	m_pBnImportCurve = new CMFCButton;
	m_pBnImportCurve->Create(L"导入", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(nButtonPosX1, int(5*Space), nButtonPosX2, int(5*Space+BUTTON_HEIGHT)), this, IDB_IMPORT_CURVE);
	m_pBnImportCurve->SetFont(m_pAppFont, TRUE);
	m_pBnImportCurve->SetTooltip(L"导入已有曲线");

	//导出曲线
	m_pBnExportCurve = new CMFCButton;
	m_pBnExportCurve->Create(L"导出", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(nButtonPosX1, int(6*Space), nButtonPosX2, int(6*Space+BUTTON_HEIGHT)), this, IDB_EXPORT_CURVE);
	m_pBnExportCurve->SetFont(m_pAppFont, TRUE);
	m_pBnExportCurve->SetTooltip(L"导出变换曲线");

	//X坐标
	m_pXPosition = new CEdit;
	m_pXPosition->Create(WS_CHILD | WS_VISIBLE, CRect(nButtonPosX1+20, int(7*Space), nButtonPosX2, int(7*Space+BUTTON_HEIGHT)), this, IDE_CurveWnd_XPOSITION);
	m_pXPosition->SetFont(m_pAppFont, TRUE);

	//Y坐标
	m_pYPosition = new CEdit;
	m_pYPosition->Create(WS_CHILD | WS_VISIBLE, CRect(nButtonPosX1+20, int(8*Space), nButtonPosX2, int(8*Space+BUTTON_HEIGHT)), this, IDE_CurveWnd_YPOSITION);
	m_pYPosition->SetFont(m_pAppFont, TRUE);

	//X标签
	m_pXlabel = new CStatic;
	m_pXlabel->Create(L"X:", WS_CHILD | WS_VISIBLE, CRect(nButtonPosX1, int(7*Space), nButtonPosX2-30, int(7*Space+BUTTON_HEIGHT)), this, IDS_CurveWnd_XLABEL);
	m_pXlabel->SetFont(m_pAppFont, TRUE);

	//Y标签
	m_pYlabel = new CStatic;
	m_pYlabel->Create(L"Y:", WS_CHILD | WS_VISIBLE, CRect(nButtonPosX1, int(8*Space), nButtonPosX2-30, int(8*Space+BUTTON_HEIGHT)), this, IDS_CurveWnd_YLABEL);
	m_pYlabel->SetFont(m_pAppFont, TRUE);

	//OK
	m_pBnOK = new CMFCButton;
	m_pBnOK->Create(L"确定", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(40, m_pWndSize->bottom-40, 40+BUTTON_WIDTH, m_pWndSize->bottom-40+BUTTON_HEIGHT), this, IDB_CURVE_OK);
	m_pBnOK->SetFont(m_pAppFont, TRUE);
	m_pBnOK->SetTooltip(L"保存修改");

	//CANCEL
	m_pBnCancel = new CMFCButton;
	m_pBnCancel->Create(L"取消", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(m_pWndSize->right-70-BUTTON_WIDTH, m_pWndSize->bottom-40, m_pWndSize->right-70, m_pWndSize->bottom-40+BUTTON_HEIGHT), this, IDB_CURVE_CANCEL);
	m_pBnCancel->SetFont(m_pAppFont, TRUE);
	m_pBnCancel->SetTooltip(L"取消修改");
	m_pBnCancel->EnableWindow(FALSE);

	//*** m_pImage ***//
	UpdateImageInfo();

	//初始化pegs
	InitPegs();

	//初始化直方图
	for (int i = 0; i<4; i++)
	{
		m_pfHistogram[i] = new d_type[256];
		ZeroMemory(m_pfHistogram[i], 256 * sizeof(d_type));
	}

	//计算获得直方图
	GetHistogram();

	return 0;
}

//获取父文档
CCurveDlg* CCurveWnd::GetDocument()
{
	return (CCurveDlg *)GetParent();
}

//获取主视图
CCTdemoView* CCurveWnd::GetView()
{
	CCurveDlg* pParent = (CCurveDlg*)GetParent();
	CCurveView* pView = (CCurveView*)GetParent();
	CCTdemoDoc* pDoc = pView->GetMainDoc();
	return (CCTdemoView*)pDoc->GetView(RUNTIME_CLASS(CCTdemoView));
}

CCTdemoDoc* CCurveWnd::GetMainDoc()
{
	CCurveDlg* pParent = (CCurveDlg*)GetParent();
	CCurveView* pView = (CCurveView*)GetParent();
	CCTdemoDoc* pDoc = pView->GetMainDoc();
	return pDoc;
}

//设置图像指针,更新直方图
void CCurveWnd::SetImage(CImage* pImage, BOOL bRePaint)
{
	// 如果图像指针没有变化，返回
	// 虽然指针没有变，但bRePaint仍可以强制更新直方图
	if (m_pImage == pImage && bRePaint == FALSE)
		return;
	// 更换图像，默认不保存操作
	OnCmdCurveCancel();
	m_pImage = pImage;
	InitPegs();
	if (UpdateImageInfo() != true)
	{
		DetectDataChanged();
	}
	GetHistogram();
	ShowGrayTransform();
	Invalidate(TRUE);
}

//获取直方图
BOOL CCurveWnd::GetHistogram()
{
	if (m_dataSrc == NULL) return FALSE;

	//计算直方图
	BYTE *Pixel;
	for (int i = 0; i<m_nImageHeight; i++)
	{
		for (int j = 0; j<m_nImageWidth; j++)
		{//RGB24各分量的排列顺序为：BGR, RGB32各分量的排列顺序为：BGRA
			Pixel = m_dataSrc + i * m_nNewRowlen + j * m_nNewChannel;
			m_pfHistogram[3][int(*Pixel)]++;                                              //Blue
			m_pfHistogram[2][int(*(Pixel+1))]++;                                          //Green
			m_pfHistogram[1][int(*(Pixel+2))]++;                                          //Red
			if (*Pixel == *(Pixel+1) && *(Pixel+1) == *(Pixel+2))
			{//此处解决直方图中某些灰度缺失的问题,2014.4.14
				m_pfHistogram[0][int(*Pixel)]++;                                              //RGB
			}
			else
			{
				m_pfHistogram[0][int(*Pixel*0.299 + *(Pixel+1)*0.587 + *(Pixel+2)*0.114)]++;  //RGB
			}
		}
	}

	//归一化
//#pragma omp parallel for
	for (int i = 0; i<4; i++)
	{
		for (int j = 0; j<256; j++)
		{
			m_pfHistogram[i][j] = m_pfHistogram[i][j]/m_nPixelQuantity;
		}
	}

	return TRUE;
}

// 初始化pegs
void CCurveWnd::InitPegs()
{
	for (int i = 0; i < 4; i++)
	{
		POSITION pos = m_pPegsList[i].GetHeadPosition();
		while (pos != NULL)
		{
			CPoint *Temp = m_pPegsList[i].GetNext(pos).m_pt;
			delete Temp;
		}
		m_pPegsList[i].RemoveAll();

		peg pBegin;
		pBegin.m_dRatioX = 0;
		pBegin.m_dRatioY = 1;
		pBegin.m_pt = new CPoint(m_pOnPaintRect->left, m_pOnPaintRect->bottom);

		peg pEnd;
		pEnd.m_dRatioX = 1;
		pEnd.m_dRatioY = 0;
		pEnd.m_pt = new CPoint(m_pOnPaintRect->right, m_pOnPaintRect->top);
		m_pPegsList[i].AddTail(pBegin);
		m_pPegsList[i].AddTail(pEnd);
	}
}

//更新图像信息
void CCurveWnd::UpdateExtraInfo()
{
	// 辅助信息
	m_nPixelQuantity = m_nImageWidth * m_nImageHeight;
	m_nbytesWidth = abs(m_pImage->GetPitch());
	m_nlenData = m_nbytesWidth * m_nImageHeight;
	m_nNewRowlen = m_nbytesWidth;
	m_nNewChannel = m_nChannel;
	m_nNewlenData = m_nlenData;
}

//看是否已存在,若存在则要替换
BOOL CCurveWnd::PegIsExist(CPoint& point)
{
	//看是否已存在
	POSITION prev, pos = m_pPegsList[m_nChannelSelected].GetHeadPosition();
	d_type errorX = max(m_nRectWidth/100.0, 5.0), errorY = max(m_nRectHeight/100.0, 5.0);
	while (pos != NULL)
	{
		prev = pos;
		peg& Temp = m_pPegsList[m_nChannelSelected].GetNext(pos);
		d_type err = fabs( 1.0*point.x - Temp.m_pt->x );//x坐标之差
		if (Temp.m_pt->x == m_pOnPaintRect->left || Temp.m_pt->x == m_pOnPaintRect->right)
		{//对于边界点：边界点的x坐标不能被修改
			if (err<errorX)
			{
				m_posCurrentPeg = prev;
				Temp.m_pt->y = point.y;
				Temp.m_dRatioY = 1.0*(point.y-m_pOnPaintRect->top)/m_nRectHeight;
				break;
			}
		}
		else if (err == 0) //x坐标相等,此处有问题。这可能导致图像画不出来。2014.4.23备注
		{
			return -1;
		}
		else if ( err < errorX && fabs( 1.0*point.y - Temp.m_pt->y ) < errorY)
		{//对于非边界点：两个pegs之间间隔很小，便不能新增peg,此时替换peg
			m_posCurrentPeg = prev;
			*(Temp.m_pt) = point;
			Temp.m_dRatioX = 1.0*(point.x-m_pOnPaintRect->left)/m_nRectWidth;
			Temp.m_dRatioY = 1.0*(point.y-m_pOnPaintRect->top)/m_nRectHeight;
			break;
		}
	}
	if (pos != NULL)
	{//peg已经存在
		return TRUE;
	}
	return FALSE;
}

//增加一个新的peg
POSITION CCurveWnd::AddNewPeg(CPoint& point)
{
	//请先判断peg不存在
	POSITION prev, pos = m_pPegsList[m_nChannelSelected].GetHeadPosition();
	while (pos != NULL)
	{
		prev = pos;
		peg& Temp = m_pPegsList[m_nChannelSelected].GetNext(pos);
		if (point.x < Temp.m_pt->x )
		{
			//将peg插到Temp前面
			peg NewPoint;
			NewPoint.m_pt = new CPoint(point);
			NewPoint.m_dRatioX = 1.0*(point.x-m_pOnPaintRect->left)/m_nRectWidth;
			NewPoint.m_dRatioY = 1.0*(point.y-m_pOnPaintRect->top)/m_nRectHeight;
			m_pPegsList[m_nChannelSelected].InsertBefore(prev, NewPoint);
			m_pPegsList[m_nChannelSelected].GetPrev(prev);//新增peg的位置
			m_posCurrentPeg = prev;
			return prev;
		}
	}
	return NULL;
}

//用户想改变某个peg，表现为点击鼠标的位置离某个peg很接近
BOOL CCurveWnd::RePlacePeg(CPoint& point)
{
	POSITION prev, pos = m_pPegsList[m_nChannelSelected].GetHeadPosition();
	while (pos != NULL)
	{
		prev = pos;
		peg& Temp = m_pPegsList[m_nChannelSelected].GetNext(pos);
		if (fabs(1.0*point.x - Temp.m_pt->x)<max(m_nRectWidth/100.0, 3.0) || fabs(1.0*point.y - Temp.m_pt->y)<max(m_nRectHeight/100.0, 3.0))
		{
			//将prev位置的peg删掉
			if (Temp.m_pt->x != m_pOnPaintRect->left && Temp.m_pt->x != m_pOnPaintRect->right)
			{//边界点不能被删除
				delete m_pPegsList[m_nChannelSelected].GetAt(prev).m_pt;
				m_pPegsList[m_nChannelSelected].RemoveAt(prev);
			}
			return TRUE;
		}
	}
	return FALSE;
}

//用户想增加一个peg
BOOL CCurveWnd::UserWillDrag(CPoint& point)
{
	d_type error = point.y - m_V4Transform[m_nChannelSelected][point.x - m_pOnPaintRect->left];
	if (error < max(m_nRectHeight/100.0, 16.0)) return TRUE;
	return FALSE;
}

//样条函数:根据m_pPegList插值出[0,255]的灰度变换
void CCurveWnd::SplineFunc()
{
    //原型: VECTOR Spline(VECTOR Node, VECTOR Value, VECTOR x0, d_type c1, d_type c2, int CASE = 2)
	VECTOR Node, Value, x0, y0;
	POSITION pos = m_pPegsList[m_nChannelSelected].GetHeadPosition();
	while (pos != NULL)
	{
		CPoint *Temp = m_pPegsList[m_nChannelSelected].GetNext(pos).m_pt;
		Node.push_back(d_type(Temp->x));
		Value.push_back(d_type(Temp->y));
	}
	for (int i = m_pOnPaintRect->left; i<=m_pOnPaintRect->right; i++)
	{
		x0.push_back(d_type(i));
	}
	y0 = Spline(Node, Value, x0, 0, 0);
	for (int i = m_pOnPaintRect->left, j = 0; i<=m_pOnPaintRect->right; i++, j++)
	{
		m_V4Transform[m_nChannelSelected][j] = y0[j];
	}
	Threshold();
	Node.~vector();
	Value.~vector();
	x0.~vector();
	y0.~vector();
	return;
}

//分段线性函数:根据m_pPegList插值出[0,255]的灰度变换
void CCurveWnd::PieceWiseFunc()
{
	POSITION pos = m_pPegsList[m_nChannelSelected].GetHeadPosition();
	while (pos != NULL)
	{
		CPoint *Temp = m_pPegsList[m_nChannelSelected].GetNext(pos).m_pt;
		if (pos != NULL)
		{
			CPoint *Next = m_pPegsList[m_nChannelSelected].GetAt(pos).m_pt;
			d_type Slope = d_type(Temp->y - Next->y)/(Temp->x - Next->x);
			for (int i = Temp->x; i<=Next->x; i++)
			{
				m_V4Transform[m_nChannelSelected][i-m_pOnPaintRect->left] = Slope*(i - Temp->x) + Temp->y;
			}
		}
		else
		{
			break;
		}
	}
}

//光标在绘图区域之内
BOOL CCurveWnd::CursorIsInPaintRect(CPoint& point)
{
	if (point.x >= m_pOnPaintRect->left && point.x <= m_pOnPaintRect->right && point.y >= m_pOnPaintRect->top && point.y <= m_pOnPaintRect->bottom) return TRUE;
	return FALSE;
}

//转换坐标，坐标原点为Dialog的左上角顶点
CPoint CCurveWnd::TransPositionToDialog(CPoint& point)
{
	return CPoint(point.x + m_pOnPaintRect->left, m_pOnPaintRect->bottom - point.y);
}

//转换坐标，坐标原点为Rect的左下角顶点
CPoint CCurveWnd::TransPositionToRectangle(CPoint& point)
{
	return CPoint(point.x - m_pOnPaintRect->left, m_pOnPaintRect->bottom - point.y);
}

//根据插值出来的灰度变换曲线作灰度变换
BOOL CCurveWnd::GrayTransform()
{
	//每次灰度变换时，需要重新载入图像源数据
	//防止每次灰度变换的效果叠加产生不良效果
	memcpy(m_curData, m_dataSrc, m_nNewlenData);
	//直方图预操作，使用后必须释放TempCell的内存
	d_type* TempCell = PreTransform();
	BYTE* Pixel;
	int it_k = min(3, m_nNewChannel);
	if (m_nChannelSelected == CHANNEL_RGB)
	{//RGB通道
//#pragma omp parallel for
		for (int i = 0; i<m_nImageHeight; i++)
		{
			for (int j = 0; j<m_nImageWidth; j++)
			{
				Pixel = m_curData + i * m_nNewRowlen + j * m_nNewChannel;
				for (int k = 0; k < it_k; k++)
				{//解决内存溢出错误,2014.4.17
					*(Pixel+k) = TempCell[int(*(Pixel+k))];
				}
			}
		}
	}
	else
	{//其他通道
//#pragma omp parallel for
		for (int i = 0; i<m_nImageHeight; i++)
		{
			for (int j = 0; j<m_nImageWidth; j++)
			{
				Pixel = m_curData + i * m_nNewRowlen + j * m_nNewChannel;
				*(Pixel+3-m_nChannelSelected) = TempCell[int(*(Pixel+3-m_nChannelSelected))];
			}
		}
	}

	delete [] TempCell;
	return TRUE;
}

//对超出Rect的变换值进行处理
void CCurveWnd::Threshold()
{
//#pragma omp parallel for
	for (int i = 0; i<m_nRectWidth+1; i++)
	{
		if(m_V4Transform[m_nChannelSelected][i] < m_pOnPaintRect->top)
		{
			m_V4Transform[m_nChannelSelected][i] = m_pOnPaintRect->top;
			continue;
		}
		if(m_V4Transform[m_nChannelSelected][i] > m_pOnPaintRect->bottom)
		{//bottom-1退而求其次，解决top和bottom绘制不出直线的问题.2014.4.15
			m_V4Transform[m_nChannelSelected][i] = m_pOnPaintRect->bottom - 1;
			continue;
		}
	}
}

//获取当前灰度值的变换值
d_type CCurveWnd::GetCurrentGrayTransform(d_type gray)
{
	// 算出整数部分和小数部分
	int x0 = int (gray), x1 = x0 + 1;
	if (x0 >= m_nRectWidth)
		x1 = m_nRectWidth;
	d_type alpha = gray - x0;
	return (1 - alpha) * m_V4Transform[m_nChannelSelected][x0] + alpha * m_V4Transform[m_nChannelSelected][x1];
}

//对m_pnTransform预操作,计算了256个数据，对应于0到255的灰度变换值
//使用后必须使用delete释放内存
d_type* CCurveWnd::PreTransform()
{
	d_type *Result = new d_type[256];
//#pragma omp parallel for
	for (int j = 0; j < 256; j++)
	{
		// 2015.5.16 此处可能应该采用插值的办法
		Result[j] = (m_pOnPaintRect->bottom - GetCurrentGrayTransform(j * m_fWidthRatio)) / m_fHeightRatio;
		ASSERT(0 <= Result[j] && Result[j] < 256);
	}
	return Result;
}

//图像反转
void CCurveWnd::InverseTransform()
{
	if (m_bInverseImage[m_nChannelSelected] == FALSE)
		return;

	POSITION pos = m_pPegsList[m_nChannelSelected].GetHeadPosition();
	while (pos != NULL)
	{
		peg& Temp = m_pPegsList[m_nChannelSelected].GetNext(pos);
		
		int gray = m_pOnPaintRect->bottom - Temp.m_pt->y;
		Temp.m_pt->y = m_pOnPaintRect->top + gray;
		Temp.m_dRatioY = 1.0*gray/m_nRectHeight;
	}
}

// 所有通道变换结果叠加,立即刷新图像
void CCurveWnd::ImshowImmediately()
{
	// 更新图像信息
	UpdateImageInfo();
	// 将原始数据备份
	BYTE* temp_src = new BYTE[m_nNewlenData];
	memcpy(temp_src, m_dataSrc, m_nNewlenData);
	// Reshape放在这里是有必要的
	ReshapePegs(true);
	// 变换并刷新视图
	int Swap = m_nChannelSelected;
	for (m_nChannelSelected = 1; m_nChannelSelected < 4; m_nChannelSelected++)
	{
		ShowGrayTransform();
		GrayTransform();
		memcpy(m_dataSrc, m_curData, m_nNewlenData);
	}
	// 对RGB通道最后变换
	m_nChannelSelected = 0;
	ShowGrayTransform();
	GrayTransform();
	// 应用到图像
	m_nChannelSelected = Swap;
	ApplyToImage(m_curData);
	// 刷新图像显示
	CCTdemoView* pView = GetView();
	pView->Invalidate();
	// 恢复原始数据
	memcpy(m_dataSrc, temp_src, m_nNewlenData);
	if (temp_src != NULL)
		delete [] temp_src;
}

//先根据pegs做插值，然后绘制变换曲线
void CCurveWnd::ShowGrayTransform()
{
	switch (m_nLinePattern)
	{//看要画什么曲线
	case LINE_PATTERN_SPLINE:
		SplineFunc();
		break;
	case LINE_PATTERN_LINEAR:
		PieceWiseFunc();
		break;
	default:
		SplineFunc();
		break;
	}
	InvalidateRect(m_pOnPaintRect);
}

//绘制
void CCurveWnd::OnPaint()
{
	//在指定区域rect里面绘制直方图
	CPaintDC dc(this);
	CBitmap MemBitmap;
	CDC MemDC;
	CRect WndSize;
	GetClientRect(&WndSize);
	MemDC.CreateCompatibleDC(&dc);
	MemBitmap.CreateCompatibleBitmap(&dc, WndSize.Width(), WndSize.Height());

	CBitmap* pOldBmp = MemDC.SelectObject(&MemBitmap);
	MemDC.FillSolidRect(m_pOnPaintRect, RGB(240, 240, 240));
	CPen *pOldPen, newPen;
	COLORREF LineColor;

	/* //#pragma omp parallel for 不能在此处随便用*/
	//水平方向颜色条
	int k = 0;
	for (int i = m_pOnPaintRect->left; i<m_pOnPaintRect->right; i++)
	{
		LineColor = ColorSetting(m_nChannelSelected, int(k/m_fWidthRatio), 1);
		newPen.CreatePen(PS_SOLID, 1, LineColor);
		pOldPen = MemDC.SelectObject(&newPen);
		MemDC.MoveTo(i, m_pOnPaintRect->bottom + 5);
		MemDC.LineTo(i, m_pOnPaintRect->bottom + 10);
		MemDC.SelectObject(pOldPen);
		newPen.DeleteObject();
		k++;
	}
	//垂直方向颜色条
	k = 0;
	for (int j = m_pOnPaintRect->top; j<m_pOnPaintRect->bottom; j++)
	{

		LineColor = ColorSetting(m_nChannelSelected, int(k/m_fHeightRatio), 1);
		newPen.CreatePen(PS_SOLID, 1, LineColor);
		pOldPen = MemDC.SelectObject(&newPen);
		MemDC.MoveTo(m_pOnPaintRect->left - 10, m_pOnPaintRect->bottom-j);
		MemDC.LineTo(m_pOnPaintRect->left - 5, m_pOnPaintRect->bottom-j);
		MemDC.SelectObject(pOldPen);
		newPen.DeleteObject();
		k++;
	}

	if (!CHECK_IMAGE_NULL(m_pImage))
	{
		//直方图
		LineColor = ColorSetting(m_nChannelSelected, 128);
		newPen.CreatePen(PS_SOLID, 1, LineColor);
		pOldPen = MemDC.SelectObject(&newPen);
		d_type rate = 0.20*m_nRectHeight*m_nRectWidth/m_fWidthRatio;//使直方图尽量布满坐标系并且便于观察
		// 应该遍历完直方图，而不是遍历完绘图区域
		int nTemp;
		if (m_fWidthRatio <= 1)
		{
			for (int i = 0; i < 256; i++)
			{
				nTemp = m_pOnPaintRect->left + i * m_fWidthRatio - 1;
				MemDC.MoveTo(nTemp, m_pOnPaintRect->bottom);
				MemDC.LineTo(nTemp, int(m_pOnPaintRect->bottom - m_pfHistogram[m_nChannelSelected][i]*rate));
			}
		}
		else
		{
			for (int i = 0; i <= m_nRectWidth; i++)
			{
				nTemp = m_pOnPaintRect->left + i - 1;
				MemDC.MoveTo(nTemp, m_pOnPaintRect->bottom);
				MemDC.LineTo(nTemp, int(m_pOnPaintRect->bottom - m_pfHistogram[m_nChannelSelected][int(i/m_fWidthRatio)]*rate));
			}
			// 对于灰度值集中在255的图像务必需要下述代码
			nTemp = m_pOnPaintRect->right - m_fWidthRatio;
			for (int i = 0; i <= m_fWidthRatio; i++)
			{
				nTemp++;
				MemDC.MoveTo(nTemp, m_pOnPaintRect->bottom);
				MemDC.LineTo(nTemp, int(m_pOnPaintRect->bottom - m_pfHistogram[m_nChannelSelected][255] * rate));
			}
		}
		newPen.DeleteObject();

		//变换曲线
		LineColor = ColorSetting(m_nChannelSelected, 255);
		newPen.CreatePen(PS_SOLID, 1, LineColor);
		pOldPen = MemDC.SelectObject(&newPen);
		for (int i = 1; i<=m_nRectWidth; i++)
		{
			MemDC.MoveTo(m_pOnPaintRect->left + i-1, m_V4Transform[m_nChannelSelected][i-1]);
			MemDC.LineTo(m_pOnPaintRect->left + i, m_V4Transform[m_nChannelSelected][i]);
		}
		MemDC.SelectObject(pOldPen);
		newPen.DeleteObject();

		//pegs
		CBrush *pOldBrush, newBrush;
		newPen.CreatePen(PS_SOLID, 1, RGB(0, 128, 128));
		pOldPen = MemDC.SelectObject(&newPen);
		POSITION pos = m_pPegsList[m_nChannelSelected].GetHeadPosition();
		while (pos != NULL)
		{
			if (pos != m_posCurrentPeg)
			{//非当前拽动的peg
				CPoint *Temp = m_pPegsList[m_nChannelSelected].GetNext(pos).m_pt;
				newBrush.CreateSolidBrush(RGB(0, 128, 128));
				pOldBrush = MemDC.SelectObject(&newBrush);
				MemDC.Rectangle(CRect(CPoint(Temp->x - 3, Temp->y - 3), CPoint(Temp->x + 3, Temp->y + 3)));
				MemDC.SelectObject(pOldBrush);
				newBrush.DeleteObject();
			}
			else
			{//当前拽动的peg
				CPoint *Temp = m_pPegsList[m_nChannelSelected].GetNext(pos).m_pt;
				newBrush.CreateSolidBrush(RGB(255, 0, 0));
				pOldBrush = MemDC.SelectObject(&newBrush);
				MemDC.Rectangle(CRect(CPoint(Temp->x - 3, Temp->y - 3), CPoint(Temp->x + 3, Temp->y + 3)));
				MemDC.SelectObject(pOldBrush);
				newBrush.DeleteObject();
			}
		}
		MemDC.SelectObject(pOldPen);
		newPen.DeleteObject();
	}

	//对角线,应该画在直方图后面，以免被覆盖
	newPen.CreatePen(PS_SOLID, 1, RGB(100, 100, 100));
	pOldPen = MemDC.SelectObject(&newPen);
	if (m_bInverseImage[m_nChannelSelected])
	{//图像已反转,对角线要变化,2014.4.15
		MemDC.MoveTo(m_pOnPaintRect->left, m_pOnPaintRect->top);
		MemDC.LineTo(m_pOnPaintRect->right, m_pOnPaintRect->bottom);
	}
	else
	{
		MemDC.MoveTo(m_pOnPaintRect->left, m_pOnPaintRect->bottom);
		MemDC.LineTo(m_pOnPaintRect->right, m_pOnPaintRect->top);
	}
	MemDC.SelectObject(pOldPen);
	newPen.DeleteObject();

	//绘制直方图外围矩形
	newPen.CreatePen(PS_SOLID, 1, RGB(64, 64, 64));
	pOldPen = MemDC.SelectObject(&newPen);
	{
		MemDC.MoveTo(m_pOnPaintRect->left, m_pOnPaintRect->top - 1);
		MemDC.LineTo(m_pOnPaintRect->left, m_pOnPaintRect->bottom);
		MemDC.MoveTo(m_pOnPaintRect->left, m_pOnPaintRect->bottom);
		MemDC.LineTo(m_pOnPaintRect->right, m_pOnPaintRect->bottom);
		MemDC.MoveTo(m_pOnPaintRect->right, m_pOnPaintRect->bottom);
		MemDC.LineTo(m_pOnPaintRect->right, m_pOnPaintRect->top - 1);
		MemDC.MoveTo(m_pOnPaintRect->right, m_pOnPaintRect->top - 1);
		MemDC.LineTo(m_pOnPaintRect->left, m_pOnPaintRect->top - 1);
	}
	MemDC.SelectObject(pOldPen);
	newPen.DeleteObject();

	//拷贝,宽度、高度为矩形宽度高度加1是为了显示矩形边框
	dc.BitBlt(m_pOnPaintRect->left, m_pOnPaintRect->top - 1, m_nRectWidth+1, m_nRectHeight+1, &MemDC, m_pOnPaintRect->left, m_pOnPaintRect->top - 1, SRCCOPY);
	dc.BitBlt(m_pOnPaintRect->left - 10, m_pOnPaintRect->top, 5, m_nRectHeight, &MemDC, m_pOnPaintRect->left - 10, m_pOnPaintRect->top, SRCCOPY);    //垂直 color bar
	dc.BitBlt(m_pOnPaintRect->left, m_pOnPaintRect->bottom + 5, m_nRectWidth, 5, &MemDC, m_pOnPaintRect->left, m_pOnPaintRect->bottom + 5, SRCCOPY); //水平 color bar
	MemDC.SelectObject(pOldBmp);
	pOldBmp->DeleteObject();
	MemBitmap.DeleteObject();
}

// 曲线：使用三次样条函数插值
void CCurveWnd::OnChooseCurve()
{
	if (CHECK_IMAGE_NULL(m_pImage)) return;
	m_nLinePattern = LINE_PATTERN_SPLINE;
	SplineFunc();
	ImshowImmediately();
	//刷新视图
	InvalidateRect(m_pOnPaintRect);
	m_pBnChooseCurve->EnableWindow(FALSE);
	m_pBnChooseLine->EnableWindow(TRUE);
	//刷新图像(2014年9月6日)
	//GetView()->Invalidate();
}

// 直线：使用分段线性函数插值
void CCurveWnd::OnChooseLine()
{
	if (CHECK_IMAGE_NULL(m_pImage)) return;
	m_nLinePattern = LINE_PATTERN_LINEAR;
	PieceWiseFunc();
	ImshowImmediately();
	//刷新视图
	InvalidateRect(m_pOnPaintRect);
	m_pBnChooseCurve->EnableWindow(TRUE);
	m_pBnChooseLine->EnableWindow(FALSE);
}

// 切换通道，注意：这会对图像保存一次状态
void CCurveWnd::OnComboboxRgb()
{
	m_nChannelSelected = m_pComboBoxRGB->GetCurSel();
	//修改peg的坐标
	ReshapePegs();
	//修改灰度变换函数
	ReshapeTransform();
	// 颜色条也更新
	CRect rect = CRect(m_pOnPaintRect->left - 10, m_pOnPaintRect->top, m_pOnPaintRect->right, m_pOnPaintRect->bottom + 10);
	InvalidateRect(rect);
	//（1）下面解决导入曲线后切换通道不画变换函数的问题,2014.4.14
	if (!CHECK_IMAGE_NULL(m_pImage))
		ImshowImmediately();
	//GrayTransform();//（2）导致直方图出现“断裂带”.2014.6.11
	//摒弃该方案，尝试新的方案解决问题（1）
}

// 重置对当前图像的更改
void CCurveWnd::OnCmdCurveWndReset()
{
	if (CHECK_IMAGE_NULL(m_pImage)) return;
	for (int i = 0; i < 4; ++i)
	{
		m_bInverseImage[i] = FALSE;
	}
	//更新视图
	memcpy(m_curData, m_dataSrc, m_nNewlenData);
	ApplyToImage(m_curData);
	CCTdemoView* pView = GetView();
	pView->Invalidate(TRUE);
	//更新直方图
	InitPegs();
	GetHistogram();
	ShowGrayTransform();
	Invalidate(TRUE);
	//ImshowImmediately();//解决重置失效问题,2014.4.15
}

// 反转图像
void CCurveWnd::OnCmdCurveWndInverse()
{
	if (CHECK_IMAGE_NULL(m_pImage)) return;
	// TODO: 在此添加命令处理程序代码
	m_bInverseImage[m_nChannelSelected] = !m_bInverseImage[m_nChannelSelected];
	ImshowImmediately();//解决翻转失效问题,2014.4.15
}

// 导入曲线
void CCurveWnd::OnCmdImportCurve()
{
	if (CHECK_IMAGE_NULL(m_pImage)) return;
	CString FilePath = L"";
	CFileDialog hFileDlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_READONLY, TEXT("curve(*.cv)|*.cv||"), NULL);
	hFileDlg.m_ofn.hwndOwner = m_hWnd;
	hFileDlg.m_ofn.lStructSize = sizeof(OPENFILENAME);
	hFileDlg.m_ofn.lpstrTitle = TEXT("导入曲线\0");
	hFileDlg.m_ofn.nMaxFile = MAX_PATH;
	if(hFileDlg.DoModal() == IDOK)
	{
		FilePath = hFileDlg.GetPathName();
	}
	else return;

	CFile file;
	if(!file.Open(FilePath, CFile::modeRead))
	{
		MessageBox(L"打开文件\"" + FilePath + L"\"" + L"失败!", L"出现错误", MB_OK | MB_ICONERROR);
		file.Close();
		return ;
	}
	CArchive ar(&file, CArchive::load);
	//重置pegs
	for (int i = 0, PegsQuantity; i<4; i++)
	{
		POSITION pos = m_pPegsList[i].GetHeadPosition();
		while (pos != NULL)
		{
			CPoint* pt = m_pPegsList[i].GetNext(pos).m_pt;
			delete pt;
		}
		m_pPegsList[i].RemoveAll();
		ar>>PegsQuantity;
		for(int j = 0; j<PegsQuantity; j++)
		{
			peg Temp;
			Temp.m_pt = new CPoint();
			Temp.m_dRatioX = Temp.m_dRatioY = -1;
			try
			{
				ar>>*(Temp.m_pt);
				ar>>Temp.m_dRatioX;
				ar>>Temp.m_dRatioY;
			}
			catch (CMemoryException* e)	{}
			catch (CFileException* e)	{}
			catch (CException* e)		{}
			if (Temp.m_dRatioX < 0 || Temp.m_dRatioY < 0)
			{
				MessageBox(L"\"" + FilePath + L"\"" + L"不是有效的曲线文件!", L"出现错误", MB_OK | MB_ICONERROR);
				ar.Close();
				file.Close();
				InitPegs();
				return;
			}
			m_pPegsList[i].AddTail(Temp);
		}
	}
	ar.Close();
	file.Close();
	ImshowImmediately();
}

// 导出曲线
void CCurveWnd::OnCmdExportCurve()
{
	if (CHECK_IMAGE_NULL(m_pImage)) return;
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	CString CurTime;
	CurTime.Format(L"%04d%02d%02d%02d%02d%02d", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond);
	CString FilePath = L"";

	CFileDialog dlg(FALSE, NULL, CurTime, NULL, TEXT("curve(*.cv)|*.cv||"), NULL);
	dlg.m_ofn.hwndOwner = m_hWnd;
	dlg.m_ofn.lStructSize = sizeof(OPENFILENAME);
	dlg.m_ofn.lpstrTitle = TEXT("保存曲线\0");
	dlg.m_ofn.nMaxFile = MAX_PATH;

	if ( dlg.DoModal() != IDOK)
		return;
	else
	{
		FilePath += dlg.GetPathName();
		if (FilePath.Right(3) != L".cv")
		{
			FilePath += L".cv";
		}
	}

	CFile file;
	file.Open(FilePath, CFile::modeCreate | CFile::modeWrite);
	CArchive ar(&file, CArchive::store);
	for (int i = 0; i<4; i++)
	{
		POSITION pos = m_pPegsList[i].GetHeadPosition();
		ar<<m_pPegsList[i].GetSize();
		while (pos != NULL)
		{
			peg& Temp = m_pPegsList[i].GetNext(pos);
			ar<<*(Temp.m_pt);
			ar<<Temp.m_dRatioX;
			ar<<Temp.m_dRatioY;
		}
	}
	ar.Close();
	file.Close();
	MessageBox(L"曲线已存入:" + FilePath, L"系统提示", MB_OK | MB_ICONINFORMATION);
}

// 确定
void CCurveWnd::OnCmdCurveOk()
{
	CCTdemoView* pView = GetView();
	CCTdemoDoc* pDoc = GetMainDoc();
	memcpy(m_dataSrc, m_curData, m_nNewlenData);
	InitPegs();
	GetHistogram();
	ImshowImmediately();
	// 更新浮点数据
	pView->m_pCurrent->MemcpyByteToFloat();
	// 检查源图像是否更改
	if (pView->m_pCurrent == pDoc->m_pImage)
		pDoc->SetModifiedFlag(TRUE);
}

// 取消
void CCurveWnd::OnCmdCurveCancel()
{
	//恢复图像并退出
	if (!CHECK_IMAGE_NULL(m_pImage))
	{
		CCTdemoView* pView = GetView();
		//8位色图像不能直接调用memcpy
		ApplyToImage(m_dataSrc);
		pView->Invalidate();
	}
}

void CCurveWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (CHECK_IMAGE_NULL(m_pImage)) return;
	//按下鼠标左键，判断用户行为
	if (CursorIsInPaintRect(point))
	{
		RePlacePeg(point);//先看看是否修改peg
		m_bUserWillDrag = UserWillDrag(point);
	}
	CWnd::OnLButtonDown(nFlags, point);
}

void CCurveWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (CHECK_IMAGE_NULL(m_pImage)) return;
	if (CursorIsInPaintRect(point) && m_bUserWillDrag)
	{//鼠标拖放结束，且终点不在pegs里面
		if (PegIsExist(point) == FALSE)
		{
			AddNewPeg(point);
		}
		ImshowImmediately();
		//更新m_bUserWillDrag
		m_bUserWillDrag = FALSE;
	}

	ReleaseCapture();
	CWnd::OnLButtonUp(nFlags, point);
}

void CCurveWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (CHECK_IMAGE_NULL(m_pImage)) return;
	if (CursorIsInPaintRect(point))
	{
		//显示当前灰度变换：输入/输出
		CString xPos, yPos;
		CPoint AfterTransform = TransPositionToRectangle(point);
		xPos.Format(L"%d", int(AfterTransform.x/m_fWidthRatio));
		yPos.Format(L"%d", int(AfterTransform.y/m_fHeightRatio));
		SetDlgItemText(IDE_CurveWnd_XPOSITION, xPos);
		SetDlgItemText(IDE_CurveWnd_YPOSITION, yPos);

		if (m_bUserWillDrag)
		{
			//AddNewPeg仅仅是为了拖动鼠标时能看到效果
			POSITION pos = AddNewPeg(point);
			ShowGrayTransform();
			UpdateWindow();//必须加上这句,才能看到当前拖动的peg
			//必须先释放指针指向的内存，否则将内存泄露
			if (pos != NULL)
			{
				delete m_pPegsList[m_nChannelSelected].GetAt(pos).m_pt;
				m_pPegsList[m_nChannelSelected].RemoveAt(pos);
			}
		}
	}
	else
	{
		SetDlgItemText(IDE_CurveWnd_XPOSITION, L"");
		SetDlgItemText(IDE_CurveWnd_YPOSITION, L"");
	}
	CWnd::OnMouseMove(nFlags, point);
}

// *** 三次样条函数 ***
VECTOR CCurveWnd::Spline(VECTOR& Node, VECTOR& Value, VECTOR& x0, d_type c1, d_type c2, int CASE)
{
	//CASE：边界条件的情形
	//c1：边界条件1
	//c2：边界条件2
	VECTOR u, Lamda, d, Mj, h, f1, s;
	d_type temp, f2;
	int i, j, n = Node.size(), sta = 0;
	if (n == 0) return VECTOR(0);
	for(j = 1; j<n; j++)
	{
		h.push_back(Node[j]-Node[j-1]);
		f1.push_back((Value[j]-Value[j-1])/h[j-1]);
	}
	u.push_back(0);
	Lamda.push_back(0);
	d.push_back(0);
	for(j = 1; j<n-1; j++)
	{
		u.push_back(h[j-1] / (h[j-1]+h[j]));
		Lamda.push_back(h[j] / (h[j-1]+h[j]));
		f2 = (f1[j]-f1[j-1]) / (Node[j+1]-Node[j-1]);
		d.push_back(6*f2);
	}

	//边界条件
	switch(CASE)
	{
	case 1:
		u.push_back(1);
		Lamda[0] = 1;
		d[0] = 6*(f1[0]-c1)/h[0];
		d.push_back(6*(c2-f1[n-2])/h[n-2]);
		break;
	case 2:
		u.push_back(0);
		Lamda[0] = 0;
		d[0] = 2*c1;
		d.push_back(2*c2);
		break;
	default:
		return VECTOR(0);
	}

	VECTOR b;
	for(i = 0; i<n; i++)
		b.push_back(2.0);
	u[0] = 0;
	Lamda.push_back(0);
	//追赶法求解
	Chase(u, b, Lamda, d, Mj, n);
	for (i = 0; i<int(x0.size()); i++)
	{
		if(x0[i]<Node[0])
		{
			j = 0;
			temp = Mj[j]*pow(Node[j+1]-x0[i],3)/(6*h[j])+Mj[j+1]*pow(x0[i]-Node[j],3)/(6*h[j])+(Value[j]-Mj[j]*pow(h[j],2)/6)*(Node[j+1]-x0[i])/h[j]+(Value[j+1]-Mj[j+1]*pow(h[j],2)/6)*(x0[i]-Node[j])/h[j];
			s.push_back(temp);
			continue;
		}
		if(x0[i]>Node[n-1])
		{
			j = n-2;
			temp = Mj[j]*pow(Node[j+1]-x0[i],3)/(6*h[j])+Mj[j+1]*pow(x0[i]-Node[j],3)/(6*h[j])+(Value[j]-Mj[j]*pow(h[j],2)/6)*(Node[j+1]-x0[i])/h[j]+(Value[j+1]-Mj[j+1]*pow(h[j],2)/6)*(x0[i]-Node[j])/h[j];
			s.push_back(temp);
			continue;
		}
		for(j = 0; j<n-1; j++)
		{
			if(Node[j]<=x0[i] && x0[i]<=Node[j+1])
			{
				temp= Mj[j]*pow(Node[j+1]-x0[i],3)/(6*h[j])+Mj[j+1]*pow(x0[i]-Node[j],3)/(6*h[j])+(Value[j]-Mj[j]*pow(h[j],2)/6)*(Node[j+1]-x0[i])/h[j]+(Value[j+1]-Mj[j+1]*pow(h[j],2)/6)*(x0[i]-Node[j])/h[j];
				s.push_back(temp);
				break;
			}
		}
	}

	u.~vector();
	Lamda.~vector();
	d.~vector();
	Mj.~vector();
	h.~vector();
	f1.~vector();
	b.~vector();
	return s;
}

// 三次样条函数：非向量方法
d_type* CCurveWnd::Spline(d_type* Node, d_type* Value, int Node_Num, d_type* x0, int x0_len, d_type c1, d_type c2, int CASE)
{
	//CASE：边界条件的情形
	//c1：边界条件1
	//c2：边界条件2
	d_type *u, *Lamda, *d, *Mj, *h, *f1, *s;
	d_type temp, f2;
	int i, j, k, n = Node_Num, sta = 0;

	h = new d_type[n - 1];
	f1 = new d_type[n - 1];
	for(j = 1; j < n; j++)
	{
		h[j-1] = Node[j] - Node[j-1];
		f1[j-1] = (Value[j]-Value[j-1]) / h[j-1];
	}

	u = new d_type[n];
	Lamda = new d_type[n];
	d = new d_type[n];
	u[0] = Lamda[0] = d[0] = 0;
	for(j = 1; j < n-1; j++)
	{
		u[j] = h[j-1] / (h[j-1]+h[j]);
		Lamda[j] = h[j] / (h[j-1]+h[j]);
		f2 = (f1[j]-f1[j-1]) / (Node[j+1]-Node[j-1]);
		d[j] = 6 * f2;
	}

	//边界条件
	switch(CASE)
	{
	case 1:
		u[j] = 1;
		Lamda[0] = 1;
		d[0] = 6*(f1[0]-c1) / h[0];
		d[j] = (6*(c2-f1[n-2]) / h[n-2]);
		break;
	case 2:
		u[j] = 0;
		Lamda[0] = 0;
		d[0] = 2 * c1;
		d[j] = 2 * c2;
		break;
	default:
		SAFE_DELETE(h);
		SAFE_DELETE(f1);
		SAFE_DELETE(u);
		SAFE_DELETE(Lamda);
		SAFE_DELETE(d);
		return NULL;
	}

	d_type* b = new d_type[n];
	for(i = 0; i < n; i++)
		b[i] = 2.0;
	u[0] = 0;
	Lamda[j] = 0;
	//追赶法求解
	Mj = new d_type[n - 1];
	Chase(u, b, Lamda, d, Mj, n);
	s = new d_type[n];
	for (i = 0, k = 0; i < x0_len; i++)
	{
		if(x0[i] < Node[0])
		{
			j = 0;
			temp = Mj[j]*pow(Node[j+1]-x0[i],3)/(6*h[j])+Mj[j+1]*pow(x0[i]-Node[j],3)/(6*h[j])+(Value[j]-Mj[j]*pow(h[j],2)/6)*(Node[j+1]-x0[i])/h[j]+(Value[j+1]-Mj[j+1]*pow(h[j],2)/6)*(x0[i]-Node[j])/h[j];
			s[k++] = temp;
			continue;
		}
		if(x0[i] > Node[n-1])
		{
			j = n-2;
			temp = Mj[j]*pow(Node[j+1]-x0[i],3)/(6*h[j])+Mj[j+1]*pow(x0[i]-Node[j],3)/(6*h[j])+(Value[j]-Mj[j]*pow(h[j],2)/6)*(Node[j+1]-x0[i])/h[j]+(Value[j+1]-Mj[j+1]*pow(h[j],2)/6)*(x0[i]-Node[j])/h[j];
			s[k++] = temp;
			continue;
		}
		for(j = 0; j < n-1; j++)
		{
			if(Node[j]<=x0[i] && x0[i]<=Node[j+1])
			{
				temp= Mj[j]*pow(Node[j+1]-x0[i],3)/(6*h[j])+Mj[j+1]*pow(x0[i]-Node[j],3)/(6*h[j])+(Value[j]-Mj[j]*pow(h[j],2)/6)*(Node[j+1]-x0[i])/h[j]+(Value[j+1]-Mj[j+1]*pow(h[j],2)/6)*(x0[i]-Node[j])/h[j];
				s[k++] = temp;
				break;
			}
		}
	}

	SAFE_DELETE(u);
	SAFE_DELETE(Lamda);
	SAFE_DELETE(d);
	SAFE_DELETE(Mj);
	SAFE_DELETE(h);
	SAFE_DELETE(f1);
	SAFE_DELETE(b);
	return s;
}

//追赶法
void CCurveWnd::Chase(VECTOR& a, VECTOR& b, VECTOR& c, VECTOR& f, VECTOR& x, int n)
{
	//a,b,c：三对角线元素
	//x：方程的解
	//n：矩阵的行
	int i;
	VECTOR belta;
	d_type tmp;
	//追的过程
	belta.push_back(c[0]/b[0]);
	x.push_back(f[0]/b[0]);
	for(i = 1; i<n-1; i++)
	{
		tmp = b[i]-a[i]*belta[i-1];
		belta.push_back(c[i]/tmp);
		x.push_back((f[i]-a[i]*x[i-1])/tmp);
	}
	tmp = b[i]-a[i]*belta[i-1];
	x.push_back((f[i]-a[i]*x[i-1])/tmp);
	//赶的过程
	for(i = n-2; i>=0; i--)
	{
		x[i] = x[i]-belta[i]*x[i+1];
	}
	belta.~vector();
}

// 追赶法：非向量版本
void CCurveWnd::Chase(d_type* a, d_type* b, d_type* c, d_type* f, d_type* x, int n)
{
	//a,b,c：三对角线元素
	//x：方程的解
	//n：矩阵的行
	int i;
	d_type* belta = new d_type[n - 1];
	d_type tmp;
	//追的过程
	belta[0] = c[0]/b[0];
	x[0] = f[0]/b[0];
	for(i = 1; i<n-1; i++)
	{
		tmp = b[i]-a[i]*belta[i-1];
		belta[i] = c[i]/tmp;
		x[0] = (f[i]-a[i]*x[i-1])/tmp;
	}
	tmp = b[i]-a[i]*belta[i-1];
	x[i] = (f[i]-a[i]*x[i-1])/tmp;
	//赶的过程
	for(i = n-2; i>=0; i--)
	{
		x[i] = x[i] - belta[i]*x[i+1];
	}
	SAFE_DELETE(belta);
}

//本来是设置通道颜色,为了使颜色条也能使用这个函数,WhereUse此参数仅仅为了节省代码量
COLORREF CCurveWnd::ColorSetting(int Channel, int Strength, int WhereUse)
{
	//Channel:通道
	//Strength:颜色强度
	COLORREF LineColor;
	switch(Channel)
	{
	case 0:
		if (WhereUse)
		{
			Strength = 255 - Strength;
		}
		LineColor = RGB(255-Strength, 255-Strength, 255-Strength);
		break;
	case 1:
		LineColor = RGB(Strength, 0, 0);
		break;
	case 2:
		LineColor = RGB(0, Strength, 0);
		break;
	case 3:
		LineColor = RGB(0, 0, Strength);
		break;
	default:
		break;
	}
	return LineColor;
}

void CCurveWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (CHECK_IMAGE_NULL(m_pImage)) return;
	if (CursorIsInPaintRect(point))
	{
		if (PegIsExist(point) == TRUE)
		{
			delete m_pPegsList[m_nChannelSelected].GetAt(m_posCurrentPeg).m_pt;
			m_pPegsList[m_nChannelSelected].RemoveAt(m_posCurrentPeg);

			ImshowImmediately();
		}
	}
	CWnd::OnRButtonUp(nFlags, point);
}

void CCurveWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	GetClientRect(m_pWndSize);

	//绘图区Rect
	m_pOnPaintRect->right = m_pWndSize->right-70;
	m_pOnPaintRect->bottom = m_pWndSize->bottom-80;

	//宽高相对于255缩放比
	m_nRectWidth = m_pOnPaintRect->Width();
	m_nRectHeight = m_pOnPaintRect->Height();
	m_fWidthRatio = m_nRectWidth/255.0;
	m_fHeightRatio = m_nRectHeight/255.0;

	int nButtonPosX1 = m_pWndSize->right-50;
	int nButtonPosX2 = nButtonPosX1+BUTTON_WIDTH;
	d_type Space = m_pWndSize->Height()/9.0;

	CWnd* hWnd;

	//Group Box
	hWnd = GetDlgItem(IDB_GROUP_BOX);
	if (hWnd != NULL)
	{
		hWnd->MoveWindow(20, 20, m_pWndSize->right-80, m_pWndSize->bottom-80);
	}
	//选择曲线
	ReShapeButtons(IDB_CHOOSE_CURVE, nButtonPosX1, nButtonPosX2, Space, 1);
	//选择直线
	ReShapeButtons(IDB_CHOOSE_LINE, nButtonPosX1, nButtonPosX2, Space, 2);
	//重置
	ReShapeButtons(IDB_CurveWnd_RESET, nButtonPosX1, nButtonPosX2, Space, 3);
	//反转
	ReShapeButtons(IDB_CurveWnd_INVERSE, nButtonPosX1, nButtonPosX2, Space, 4);
	//导入曲线
	ReShapeButtons(IDB_IMPORT_CURVE, nButtonPosX1, nButtonPosX2, Space, 5);
	//导出曲线
	ReShapeButtons(IDB_EXPORT_CURVE, nButtonPosX1, nButtonPosX2, Space, 6);
	//X坐标
	hWnd = GetDlgItem(IDE_CurveWnd_XPOSITION);
	if (hWnd != NULL)
	{
		hWnd->MoveWindow(nButtonPosX1+15, int(7*Space), BUTTON_WIDTH-15, BUTTON_HEIGHT);
	}
	//Y坐标
	hWnd = GetDlgItem(IDE_CurveWnd_YPOSITION);
	if (hWnd != NULL)
	{
		hWnd->MoveWindow(nButtonPosX1+15, int(8*Space), BUTTON_WIDTH-15, BUTTON_HEIGHT);
	}
	//X标签
	hWnd = GetDlgItem(IDS_CurveWnd_XLABEL);
	if (hWnd != NULL)
	{
		hWnd->MoveWindow(nButtonPosX1, int(7*Space), BUTTON_WIDTH-25, BUTTON_HEIGHT);
	}
	//Y标签
	hWnd = GetDlgItem(IDS_CurveWnd_YLABEL);
	if (hWnd != NULL)
	{
		hWnd->MoveWindow(nButtonPosX1, int(8*Space), BUTTON_WIDTH-25, BUTTON_HEIGHT);
	}

	//OK
	hWnd = GetDlgItem(IDB_CURVE_OK);
	if (hWnd != NULL)
	{
		hWnd->MoveWindow(40, m_pWndSize->bottom-40, BUTTON_WIDTH, BUTTON_HEIGHT);
	}
	//CANCEL
	hWnd = GetDlgItem(IDB_CURVE_CANCEL);
	if (hWnd != NULL)
	{
		hWnd->MoveWindow(m_pWndSize->right-70-BUTTON_WIDTH, m_pWndSize->bottom-40, BUTTON_WIDTH, BUTTON_HEIGHT);
	}

	ReshapePegs(true);

	ReshapeTransform(true);

	// 下述语句必须存在
	if (!CHECK_IMAGE_NULL(m_pImage))
	{
		ShowGrayTransform();
	}
}

//窗口变化时，调整按钮位置
void CCurveWnd::ReShapeButtons(int nItemID, int nButtonPosX1, int nButtonPosX2, d_type Space, int Number)
{
	CWnd *hWnd = GetDlgItem(nItemID);
	if (hWnd != NULL)
	{//MoveWindow:int x, int y, int nWidth, int nHeight
		hWnd->MoveWindow(nButtonPosX1, int(Number*Space-10), BUTTON_WIDTH, BUTTON_HEIGHT);
	}
}

// 窗口变化时，Pegs坐标同时变化
// 2014年9月6日修改此函数, 解决一个错误
void CCurveWnd::ReshapePegs(bool bAllRefresh)
{
	if (bAllRefresh)
	{
		for (int i = 0; i < 4; ++i)
		{
			POSITION pos = m_pPegsList[i].GetHeadPosition();
			if (m_bInverseImage[i])
			{
				while (pos != NULL)
				{
					peg& Temp = m_pPegsList[i].GetNext(pos);
					Temp.m_pt->x = LONG(m_pOnPaintRect->left+Temp.m_dRatioX*m_nRectWidth);
					Temp.m_pt->y = LONG(m_pOnPaintRect->top+Temp.m_dRatioY*m_nRectHeight);
					int gray = m_pOnPaintRect->bottom - Temp.m_pt->y;
					Temp.m_pt->y = m_pOnPaintRect->top + gray;
					Temp.m_dRatioY = 1.0*gray/m_nRectHeight;
				}
				m_bInverseImage[i] = false;
			}
			else
			{
				while (pos != NULL)
				{
					peg& Temp = m_pPegsList[i].GetNext(pos);
					//相对于Rect左上角位置
					Temp.m_pt->x = LONG(m_pOnPaintRect->left+Temp.m_dRatioX*m_nRectWidth);
					Temp.m_pt->y = LONG(m_pOnPaintRect->top+Temp.m_dRatioY*m_nRectHeight);
				}
			}
		}
	}
	else
	{
		POSITION pos = m_pPegsList[m_nChannelSelected].GetHeadPosition();
		if (m_bInverseImage[m_nChannelSelected])
		{
			while (pos != NULL)
			{
				peg& Temp = m_pPegsList[m_nChannelSelected].GetNext(pos);
				Temp.m_pt->x = LONG(m_pOnPaintRect->left+Temp.m_dRatioX*m_nRectWidth);
				Temp.m_pt->y = LONG(m_pOnPaintRect->top+Temp.m_dRatioY*m_nRectHeight);
				int gray = m_pOnPaintRect->bottom - Temp.m_pt->y;
				Temp.m_pt->y = m_pOnPaintRect->top + gray;
				Temp.m_dRatioY = 1.0*gray/m_nRectHeight;
			}
			m_bInverseImage[m_nChannelSelected] = false;
		} 
		else
		{
			while (pos != NULL)
			{
				peg& Temp = m_pPegsList[m_nChannelSelected].GetNext(pos);
				//相对于Rect左上角位置
				Temp.m_pt->x = LONG(m_pOnPaintRect->left+Temp.m_dRatioX*m_nRectWidth);
				Temp.m_pt->y = LONG(m_pOnPaintRect->top+Temp.m_dRatioY*m_nRectHeight);
			}
		}
	}
}

//窗口变化时，存放灰度变换的vector也变化
void CCurveWnd::ReshapeTransform(bool bAllRefresh)
{
	if (bAllRefresh)
	{
		for (int i = 0; i < 4; i++)
		{
			m_V4Transform[i].clear();
			for (int j = 0; j < m_nRectWidth+1; j++)
			{
				m_V4Transform[i].push_back(0);
			}
		}
	}
	else
	{
		m_V4Transform[m_nChannelSelected].clear();
		for (int j = 0; j < m_nRectWidth+1; j++)
		{
			m_V4Transform[m_nChannelSelected].push_back(0);
		}
	}
}

BOOL CCurveWnd::OnEraseBkgnd(CDC* pDC)
{
	//使其不要重绘背景
	return TRUE;
}

//分配数据
BYTE* CCurveWnd::MallocData(BYTE* pSrc)
{
	BYTE* pDst = NULL;
	if (m_nChannel > 1)
	{
		pDst = new BYTE[m_nlenData];
		memcpy(pDst, pSrc, m_nlenData);
		return pDst;
	}
	pDst = new BYTE[m_nNewlenData];
//#pragma omp parallel for
	for (int i = 0; i < m_nImageHeight; ++i)
	{
		for (int j = 0; j < m_nImageWidth; ++j)
		{
			pDst  [    j * m_nNewChannel + i * m_nNewRowlen] 
			= pDst[1 + j * m_nNewChannel + i * m_nNewRowlen] 
			= pDst[2 + j * m_nNewChannel + i * m_nNewRowlen] 
			= pSrc[j * m_nChannel + i * m_nbytesWidth];
		}
	}
	return pDst;
}

//将图像源数据储存起来
void CCurveWnd::MallocData()
{
	SAFE_DELETE(m_dataSrc);
	SAFE_DELETE(m_curData);
	// 3、4通道图像可以直接调用memcpy
	if (m_nChannel > 1)
	{
		m_dataSrc = new BYTE[m_nlenData];
		m_curData = new BYTE[m_nlenData];
		memcpy(m_dataSrc, m_pBits, m_nlenData);
		memcpy(m_curData, m_pBits, m_nlenData);
		return;
	}
	m_nNewChannel = 3;
	m_nNewRowlen = m_nImageWidth * m_nNewChannel;
	m_nNewlenData = m_nImageHeight * m_nNewRowlen;
	m_dataSrc = new BYTE[m_nNewlenData];
	m_curData = new BYTE[m_nNewlenData];
//#pragma omp parallel for
	for (int i = 0; i < m_nImageHeight; ++i)
	{
		for (int j = 0; j < m_nImageWidth; ++j)
		{
			m_dataSrc  [    j * m_nNewChannel + i * m_nNewRowlen] 
			= m_dataSrc[1 + j * m_nNewChannel + i * m_nNewRowlen] 
			= m_dataSrc[2 + j * m_nNewChannel + i * m_nNewRowlen] 
			= m_pBits[j * m_nChannel + i * m_nbytesWidth];
		}
	}
	memcpy(m_curData, m_dataSrc, m_nNewlenData);
}


// 将数据pSrc应用到图像
void CCurveWnd::ApplyToImage(BYTE* pSrc)
{
	// 未检测到修改则返回
	if (!DetectModified())
		return;
	// 检查图像宽度、高度、通道是否没变
	if (m_nImageWidth != m_pImage->GetWidth() || m_nImageHeight != m_pImage->GetHeight() || m_nChannel != m_pImage->GetBPP() / 8)
		return;
	// 3、4通道图像可以直接调用memcpy
	if (m_nChannel > 1)
	{
		memcpy(m_pBits, pSrc, m_nlenData);
		return;
	}
	// 转换公式参考：http://www.cnblogs.com/carekee/articles/3629964.html
	//#pragma omp parallel for
	for (int i = 0; i < m_nImageHeight; ++i)
	{
		for (int j = 0; j < m_nImageWidth; ++j)
		{
			m_pBits[j * m_nChannel + i * m_nbytesWidth] = 
				(299 * pSrc[ j * m_nNewChannel + i * m_nNewRowlen]
			+ 587 * pSrc[1 + j * m_nNewChannel + i * m_nNewRowlen] 
			+ 114 * pSrc[2 + j * m_nNewChannel + i * m_nNewRowlen]+ 500 ) / 1000;
		}
	}
}


// 检查图像是否需要被更新,根据peg列表
bool CCurveWnd::DetectModified()
{
	// 查看通道直方图变换是否被修改
	for (int i = 0; i < 4; i++)
	{
		int pegNum = m_pPegsList[i].GetSize();
		if (pegNum > 2)
			return true;
		// peg个数等于2，检查端点
		POSITION pos = m_pPegsList[i].GetHeadPosition();
		CPoint *Left = m_pPegsList[i].GetNext(pos).m_pt;
		CPoint *Right = m_pPegsList[i].GetNext(pos).m_pt;
		if (Left->x != m_pOnPaintRect->left || Left->y != m_pOnPaintRect->bottom
			|| Right->x != m_pOnPaintRect->right || Right->y != m_pOnPaintRect->top)
			return true;
	}
	return false;
}

// 检查图像是否变化了
bool CCurveWnd::UpdateImageInfo()
{
	if (CHECK_IMAGE_NULL(m_pImage))
		return false;
	//如果图像的位深度不满足8、24、32
	int nChannel = m_pImage->GetBPP() / 8;
	if (nChannel != 1 && nChannel != 3 && nChannel != 4)
	{
		m_pImage = NULL;
		return false;
	}

	int nImageWidth = m_pImage->GetWidth();
	int nImageHeight = m_pImage->GetHeight();
	BYTE* pBits = (BYTE *)m_pImage->GetBits() + (m_pImage->GetPitch() * (nImageHeight - 1));
	// 宽度、高度、通道、指针，其中一项不同就表示图像被修改了
	if (m_nImageWidth != nImageWidth || m_nImageHeight != nImageHeight || m_nChannel != nChannel || m_pBits != pBits)
	{
		m_nImageWidth = nImageWidth;
		m_nImageHeight = nImageHeight;
		m_nChannel = nChannel;
		m_pBits = pBits;
		UpdateExtraInfo();
		MallocData();
		return true;
	}
	// 从外观看图像没有被修改,未检测图像的数据
	return false;
}

// UpdateImageInfo会检查图像宽度、高度、通道等信息是否修改，但不检测数据项，需要检测请调用这个函数。
bool CCurveWnd::DetectDataChanged()
{
	if (CHECK_IMAGE_NULL(m_pImage))
		return false;
	// 以上各项都相同，则判断数据是否相同
	BYTE* dataSrc = MallocData(m_pBits);
	if (strcmp((char*)dataSrc, (char*)m_dataSrc) != 0)
	{// 缓存数据被修改，需要更新缓存数据
		SAFE_DELETE(m_dataSrc);
		m_dataSrc = dataSrc;
		dataSrc = NULL;
		SAFE_DELETE(m_curData);
		m_curData = new BYTE[m_nNewlenData];
		memcpy(m_curData, m_dataSrc, m_nNewlenData);
		return true;
	}
	SAFE_DELETE(dataSrc);
	return false;
}