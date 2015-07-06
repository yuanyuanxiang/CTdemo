
// MainFrm.h : CMainFrame 类的接口
//

#pragma once

// 工具栏响应函数的ID
#define ID_VIEW_ZOOM_IN 10240
#define ID_VIEW_ZOOM_OUT 10250
#define ID_VIEW_DEFAULT 10260
#define ID_FLIP_VERTICAL 10270
#define ID_FLIP_HORIZONTAL 10280
#define ID_ABOUT 10290
#define ID_QUIT 10300
#define indicators_clock 0
#define SalverMenuPopup 1

#define WM_SHOWTASK (WM_USER + 1)	// 自定义的消息名称

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// 特性
public:

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CMFCMenuBar			m_wndMenuBar;		//菜单栏
	CMFCToolBar			m_wndToolBar;		//工具栏
	CMFCStatusBar		m_wndStatusBar;		//状态栏

public:
	NOTIFYICONDATA		m_tnid;				//托盘相关
	CMenu				m_salver;			//托盘菜单

public:
	// 获得状态栏指针
	CMFCStatusBar *GetStatusBar() { return &m_wndStatusBar; }

// 生成的消息映射函数
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
	afx_msg void OnMinimizedApp();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnShowwindow();
	afx_msg void OnUpdateShowwindow(CCmdUI *pCmdUI);
};