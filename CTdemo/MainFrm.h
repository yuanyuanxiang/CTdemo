
// MainFrm.h : CMainFrame ��Ľӿ�
//

#pragma once

// ��������Ӧ������ID
#define ID_VIEW_ZOOM_IN 10240
#define ID_VIEW_ZOOM_OUT 10250
#define ID_VIEW_DEFAULT 10260
#define ID_FLIP_VERTICAL 10270
#define ID_FLIP_HORIZONTAL 10280
#define ID_ABOUT 10290
#define ID_QUIT 10300
#define indicators_clock 0

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // �ؼ���Ƕ���Ա
	CMFCMenuBar			m_wndMenuBar;		//�˵���
	CMFCToolBar			m_wndToolBar;		//������
	CMFCStatusBar		m_wndStatusBar;		//״̬��

public:
	// ���״̬��ָ��
	CMFCStatusBar *GetStatusBar() { return &m_wndStatusBar; }

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnDropFiles(HDROP hDropInfo);
};