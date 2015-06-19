
// CTdemo.cpp : ����Ӧ�ó��������Ϊ��
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
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, &CCTdemoApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// ��׼��ӡ��������
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
	ON_COMMAND(ID_FILE_OPEN_USING_OPENGL, &CCTdemoApp::OnFileOpenUsingOpenGL)
	ON_COMMAND(ID_EDIT_COPY, &CCTdemoApp::OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, &CCTdemoApp::OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CCTdemoApp::OnUpdateEditCopy)
END_MESSAGE_MAP()


// CCTdemoApp ����

CCTdemoApp::CCTdemoApp()
{
	m_bHiColorIcons = TRUE;

	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// ���Ӧ�ó��������ù�����������ʱ֧��(/clr)�����ģ���:
	//     1) �����д˸������ã�������������������֧�ֲ�������������
	//     2) ��������Ŀ�У������밴������˳���� System.Windows.Forms ������á�
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: ������Ӧ�ó��� ID �ַ����滻ΪΨһ�� ID �ַ�����������ַ�����ʽ
	//Ϊ CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("CTdemo.AppID.NoVersion"));

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

// Ψһ��һ�� CCTdemoApp ����

CCTdemoApp theApp;


// CCTdemoApp ��ʼ��

BOOL CCTdemoApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// ʹ�� RichEdit �ؼ���Ҫ  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Capital Normal University"));
	LoadStdProfileSettings(8);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// ע��Ӧ�ó�����ĵ�ģ�塣�ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	m_pDocTemplate = new CMultiDocTemplate(IDR_CTdemoTYPE,
		RUNTIME_CLASS(CCTdemoDoc),
		RUNTIME_CLASS(CChildFrame), // �Զ��� MDI �ӿ��
		RUNTIME_CLASS(CCTdemoView));
	if (!m_pDocTemplate)
		return FALSE;
	AddDocTemplate(m_pDocTemplate);

	// �ο���http://blog.csdn.net/vlily/article/details/7407966
	// http://www.cppblog.com/cs-xiaolee/archive/2011/12/01/161292.html

	// ������ MDI ��ܴ���
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;


	// ������׼ shell ���DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	ParseCommandLine(cmdInfo);



	// ��������������ָ����������
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// �������ѳ�ʼ���������ʾ����������и���
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	m_bUsingOpenGL = false;

	return TRUE;
}

int CCTdemoApp::ExitInstance()
{
	//TODO: �����������ӵĸ�����Դ
	CMFCVisualManager::DestroyInstance(TRUE);
	AfxOleTerm(FALSE);
	return CWinAppEx::ExitInstance();
}

// CCTdemoApp ��Ϣ�������


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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

// �������жԻ����Ӧ�ó�������
void CCTdemoApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CCTdemoApp �Զ������/���淽��

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