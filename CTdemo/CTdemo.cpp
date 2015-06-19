
// CTdemo.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "CTdemo.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "CTdemoDoc.h"
#include "CTdemoView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCTdemoApp

BEGIN_MESSAGE_MAP(CCTdemoApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CCTdemoApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CCTdemoApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
	ON_COMMAND(ID_FILE_OPEN_USING_OPENGL, &CCTdemoApp::OnFileOpenUsingOpenGL)
	ON_COMMAND(ID_EDIT_COPY, &CCTdemoApp::OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, &CCTdemoApp::OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CCTdemoApp::OnUpdateEditCopy)
END_MESSAGE_MAP()


// CCTdemoApp 构造

CCTdemoApp::CCTdemoApp()
{
	m_bHiColorIcons = TRUE;

	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则:
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("CTdemo.AppID.NoVersion"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CCTdemoApp 对象

CCTdemoApp theApp;


// CCTdemoApp 初始化

BOOL CCTdemoApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// 使用 RichEdit 控件需要  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("Capital Normal University"));
	LoadStdProfileSettings(8);  // 加载标准 INI 文件选项(包括 MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	m_pDocTemplate = new CMultiDocTemplate(IDR_CTdemoTYPE,
		RUNTIME_CLASS(CCTdemoDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CCTdemoView));
	if (!m_pDocTemplate)
		return FALSE;
	AddDocTemplate(m_pDocTemplate);

	// 参看：http://blog.csdn.net/vlily/article/details/7407966
	// http://www.cppblog.com/cs-xiaolee/archive/2011/12/01/161292.html

	// 创建主 MDI 框架窗口
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;


	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	ParseCommandLine(cmdInfo);



	// 调度在命令行中指定的命令。如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// 主窗口已初始化，因此显示它并对其进行更新
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	m_bUsingOpenGL = false;

	return TRUE;
}

int CCTdemoApp::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	CMFCVisualManager::DestroyInstance(TRUE);
	AfxOleTerm(FALSE);
	return CWinAppEx::ExitInstance();
}

// CCTdemoApp 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CCTdemoApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CCTdemoApp 自定义加载/保存方法

void CCTdemoApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
}

void CCTdemoApp::LoadCustomState()
{
}

void CCTdemoApp::SaveCustomState()
{
}

void CCTdemoApp::OnFileNew()
{
	m_pDocTemplate->OpenDocumentFile(NULL);
}


void CCTdemoApp::OnFileOpenUsingOpenGL()
{
	m_bUsingOpenGL = true;
	CWinAppEx::OnFileOpen();
	m_bUsingOpenGL = false;
}


CCTdemoDoc* CCTdemoApp::GetMainDoc()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CChildFrame* pChildFrame = (CChildFrame*)pMainFrame->GetActiveFrame();
	CCTdemoDoc* pDoc = (CCTdemoDoc*)pChildFrame->GetActiveDocument();
	return pDoc;
}


CCTdemoView* CCTdemoApp::GetActiveView()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CChildFrame* pChildFrame = (CChildFrame*)pMainFrame->GetActiveFrame();
	CCTdemoView* pView = (CCTdemoView*)pChildFrame->GetActiveView();
	return pView;
}


void CCTdemoApp::OnEditCopy()
{
	CCTdemoView* pView = GetActiveView();
	ASSERT(pView != NULL);
	pView->CopyImage(pView->m_pCurrent);
}


void CCTdemoApp::OnEditPaste()
{
	CCTdemoView* pView = GetActiveView();
	ASSERT(pView != NULL);
	pView->PasteImage();
}


void CCTdemoApp::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	CCTdemoView* pView = GetActiveView();
	if (!pView)
		return;
	pCmdUI->Enable(!CHECK_IMAGE_NULL(pView->m_pCurrent));
}