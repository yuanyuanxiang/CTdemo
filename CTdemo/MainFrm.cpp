
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "CTdemo.h"

#include "MainFrm.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_WM_SETTINGCHANGE()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_DROPFILES()
	ON_COMMAND(ID_MINIMIZED_APP, &CMainFrame::OnMinimizedApp)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_SHOWWINDOW, &CMainFrame::OnShowwindow)
	ON_UPDATE_COMMAND_UI(ID_SHOWWINDOW, &CMainFrame::OnUpdateShowwindow)
END_MESSAGE_MAP()


// 状态行指示器
static UINT indicators[] =
{
	ID_SEPARATOR, 
	ID_COPYRIGHT_STRING, 
	ID_INDICATOR_CLOCK, 
	ID_INDICATOR_CAPS, 
	ID_INDICATOR_NUM, 
	ID_INDICATOR_SCRL,
};


// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // 其他可用样式...
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // 设置为 FALSE 会将关闭按钮放置在选项卡区域的右侧
	mdiTabParams.m_bTabIcons = FALSE;    // 设置为 TRUE 将在 MDI 选项卡上启用文档图标
	mdiTabParams.m_bAutoColor = TRUE;    // 设置为 FALSE 将禁用 MDI 选项卡的自动着色
	mdiTabParams.m_bDocumentMenu = TRUE; // 在选项卡区域的右边缘启用文档菜单
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("未能创建菜单栏\n");
		return -1;      // 未能创建
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// 防止菜单栏在激活时获得焦点
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// 允许用户定义的工具栏操作:
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 如果您不希望工具栏和菜单栏可停靠，请删除这五行
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 创建停靠窗口
	if (!CreateDockingWindows())
	{
		TRACE0("未能创建停靠窗口\n");
		return -1;
	}

	// 设置用于绘制所有用户界面元素的视觉管理器
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 启用增强的窗口管理对话框
	EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	// 启用工具栏和停靠窗口菜单替换
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// 启用快速(按住 Alt 拖动)工具栏自定义
	CMFCToolBar::EnableQuickCustomization();

	// 将文档名和应用程序名称在窗口标题栏上的顺序进行交换。这
	// 将改进任务栏的可用性，因为显示的文档名带有缩略图。
	ModifyStyle(0, FWS_PREFIXTITLE);

	// 安装定时器，并将其时间间隔设为1000毫秒
	SetTimer(indicators_clock, 1000, NULL);
	
	DragAcceptFiles(TRUE);//支持文件拖拽

	m_salver.LoadMenu(IDR_SALVER_MENU);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// 创建类视图
	CString strClassView;
	bNameValid = strClassView.LoadString(IDS_CLASS_VIEW);
	ASSERT(bNameValid);

	// 创建文件视图
	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);

	// 创建输出窗口
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);

	// 创建属性窗口
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{

	UpdateMDITabbedBarsIcons();
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 扫描菜单*/);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CMDIFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}


BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// 基类将执行真正的工作

	if (!CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// 为所有用户工具栏启用自定义按钮
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}


void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CMDIFrameWndEx::OnSettingChange(uFlags, lpszSection);
}


void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == indicators_clock)
	{
		CTime time;
		// 得到当前时间
		time = CTime::GetCurrentTime();
		// 转换时间格式
		CString str_time = time.Format(_T("%H:%M:%S"));
		//显示时钟
		m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_INDICATOR_CLOCK), str_time);
		//显示信息
		m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_COPYRIGHT_STRING), _T("CTdemo"));
	}
	else if (nIDEvent == SalverMenuPopup)
	{
		this->ActivateTopParent();
		KillTimer(SalverMenuPopup);
	}

	CMDIFrameWndEx::OnTimer(nIDEvent);
}


void CMainFrame::OnClose()
{
	KillTimer(indicators_clock);
	CMDIFrameWndEx::OnClose();
}


// 支持拖放文件
void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	// 获取拖动的文件个数
	const int fileCount = DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);
	ASSERT(fileCount >= 1);

	CCTdemoApp* pApp = (CCTdemoApp*)AfxGetApp();
	for (int i = 0; i < fileCount; i++)
	{
		TCHAR fileName[MAX_PATH] = { 0 };
		DragQueryFile(hDropInfo, i, fileName, MAX_PATH);
		CString str;
		str.Format(_T("%s"), fileName);
		// 最后执行你要执行的操作
		pApp->m_pDocTemplate->OpenDocumentFile(str);
	}
	// CMDIFrameWndEx::OnDropFiles(hDropInfo);
}


void CMainFrame::OnMinimizedApp()
{
	m_tnid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);   
	m_tnid.hWnd = GetSafeHwnd();   
	m_tnid.uID = IDR_MAINFRAME;   
	m_tnid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP ;   
	m_tnid.uCallbackMessage = WM_SHOWTASK;
	m_tnid.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	CString sText = _T("CTdemo\n模拟CT扫描与重建");
	wcscpy(m_tnid.szTip, CT2CW(sText));				//提示信息
	Shell_NotifyIcon(NIM_ADD, &m_tnid);				//在托盘区添加图标
	ShowWindow(SW_HIDE);							//隐藏主窗口
}


LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_SHOWTASK:
		// 如果是用户定义的消息
		if(lParam == WM_LBUTTONDOWN)
		{
			OnShowwindow();
		}
		else if(lParam == WM_RBUTTONDOWN)
		{
			// 鼠标右键单击弹出选单
			LPPOINT lpoint = new tagPOINT;
			::GetCursorPos(lpoint); // 得到鼠标位置
			CMenu *pMenu = m_salver.GetSubMenu(0);   // 弹出IDR_SALVER_MENU菜单的第0个菜单项
			SetTimer(SalverMenuPopup, 3000, NULL);
			pMenu->TrackPopupMenu(TPM_RIGHTALIGN | TPM_RIGHTBUTTON, lpoint->x ,lpoint->y, this);
			delete lpoint;        
		}
		break;
	default:
		break;
	} 
	return CMDIFrameWndEx::WindowProc(message, wParam, lParam);
}


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CMDIFrameWndEx::OnSize(nType, cx, cy);

	if(nType == SIZE_MINIMIZED)
	{

		OnMinimizedApp(); // 当最小化时，隐藏主窗口
	}
}


void CMainFrame::OnDestroy()
{
	CMDIFrameWndEx::OnDestroy();

	// 在托盘区删除图标
	Shell_NotifyIcon(NIM_DELETE, &m_tnid);
}


// 显示程序窗口，使窗口在最前面
void CMainFrame::OnShowwindow()
{
	if (!IsWindowVisible())
	{
		ShowWindow(SW_SHOWNORMAL);
		SetWindowPos(&this->wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		SetWindowPos(&this->wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	}
	else
	{
		ShowWindow(SW_HIDE);
	}
}


void CMainFrame::OnUpdateShowwindow(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!IsWindowVisible());
}