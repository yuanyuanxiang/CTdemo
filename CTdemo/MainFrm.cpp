
// MainFrm.cpp : CMainFrame ���ʵ��
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


// ״̬��ָʾ��
static UINT indicators[] =
{
	ID_SEPARATOR, 
	ID_COPYRIGHT_STRING, 
	ID_INDICATOR_CLOCK, 
	ID_INDICATOR_CAPS, 
	ID_INDICATOR_NUM, 
	ID_INDICATOR_SCRL,
};


// CMainFrame ����/����

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
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // ����������ʽ...
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // ����Ϊ FALSE �Ὣ�رհ�ť������ѡ�������Ҳ�
	mdiTabParams.m_bTabIcons = FALSE;    // ����Ϊ TRUE ���� MDI ѡ��������ĵ�ͼ��
	mdiTabParams.m_bAutoColor = TRUE;    // ����Ϊ FALSE ������ MDI ѡ����Զ���ɫ
	mdiTabParams.m_bDocumentMenu = TRUE; // ��ѡ�������ұ�Ե�����ĵ��˵�
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("δ�ܴ����˵���\n");
		return -1;      // δ�ܴ���
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// ��ֹ�˵����ڼ���ʱ��ý���
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("δ�ܴ���������\n");
		return -1;      // δ�ܴ���
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// �����û�����Ĺ���������:
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: �������ϣ���������Ͳ˵�����ͣ������ɾ��������
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// ���� Visual Studio 2005 ��ʽͣ��������Ϊ
	CDockingManager::SetDockingMode(DT_SMART);
	// ���� Visual Studio 2005 ��ʽͣ�������Զ�������Ϊ
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// ����ͣ������
	if (!CreateDockingWindows())
	{
		TRACE0("δ�ܴ���ͣ������\n");
		return -1;
	}

	// �������ڻ��������û�����Ԫ�ص��Ӿ�������
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// ������ǿ�Ĵ��ڹ���Ի���
	EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	// ���ù�������ͣ�����ڲ˵��滻
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// ���ÿ���(��ס Alt �϶�)�������Զ���
	CMFCToolBar::EnableQuickCustomization();

	// ���ĵ�����Ӧ�ó��������ڴ��ڱ������ϵ�˳����н�������
	// ���Ľ��������Ŀ����ԣ���Ϊ��ʾ���ĵ�����������ͼ��
	ModifyStyle(0, FWS_PREFIXTITLE);

	// ��װ��ʱ����������ʱ������Ϊ1000����
	SetTimer(indicators_clock, 1000, NULL);
	
	DragAcceptFiles(TRUE);//֧���ļ���ק

	m_salver.LoadMenu(IDR_SALVER_MENU);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// ��������ͼ
	CString strClassView;
	bNameValid = strClassView.LoadString(IDS_CLASS_VIEW);
	ASSERT(bNameValid);

	// �����ļ���ͼ
	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);

	// �����������
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);

	// �������Դ���
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

// CMainFrame ���

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


// CMainFrame ��Ϣ�������

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* ɨ��˵�*/);
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
	// ���ཫִ�������Ĺ���

	if (!CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// Ϊ�����û������������Զ��尴ť
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
		// �õ���ǰʱ��
		time = CTime::GetCurrentTime();
		// ת��ʱ���ʽ
		CString str_time = time.Format(_T("%H:%M:%S"));
		//��ʾʱ��
		m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_INDICATOR_CLOCK), str_time);
		//��ʾ��Ϣ
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


// ֧���Ϸ��ļ�
void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	// ��ȡ�϶����ļ�����
	const int fileCount = DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);
	ASSERT(fileCount >= 1);

	CCTdemoApp* pApp = (CCTdemoApp*)AfxGetApp();
	for (int i = 0; i < fileCount; i++)
	{
		TCHAR fileName[MAX_PATH] = { 0 };
		DragQueryFile(hDropInfo, i, fileName, MAX_PATH);
		CString str;
		str.Format(_T("%s"), fileName);
		// ���ִ����Ҫִ�еĲ���
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
	CString sText = _T("CTdemo\nģ��CTɨ�����ؽ�");
	wcscpy(m_tnid.szTip, CT2CW(sText));				//��ʾ��Ϣ
	Shell_NotifyIcon(NIM_ADD, &m_tnid);				//�����������ͼ��
	ShowWindow(SW_HIDE);							//����������
}


LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_SHOWTASK:
		// ������û��������Ϣ
		if(lParam == WM_LBUTTONDOWN)
		{
			OnShowwindow();
		}
		else if(lParam == WM_RBUTTONDOWN)
		{
			// ����Ҽ���������ѡ��
			LPPOINT lpoint = new tagPOINT;
			::GetCursorPos(lpoint); // �õ����λ��
			CMenu *pMenu = m_salver.GetSubMenu(0);   // ����IDR_SALVER_MENU�˵��ĵ�0���˵���
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

		OnMinimizedApp(); // ����С��ʱ������������
	}
}


void CMainFrame::OnDestroy()
{
	CMDIFrameWndEx::OnDestroy();

	// ��������ɾ��ͼ��
	Shell_NotifyIcon(NIM_DELETE, &m_tnid);
}


// ��ʾ���򴰿ڣ�ʹ��������ǰ��
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