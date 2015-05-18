#pragma once
#include <afxdialogex.h>
#include <atlimage.h>
#include "Resource.h"


// CImageQuickViewer

#define QUICK_VIEWER_SIZE 256 // 快速预览窗口大小

class CDlgQuickViewer : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgQuickViewer)

public:
	CDC*	m_ParentCDC;
	CRect	m_ClientRect;
	CRect	m_SourceRect;

	void SetParentCDC(CDC* pDC);
	void SetSourceRect(CRect rect);
	void SetSourceRect(int left, int top, int right, int bottom);

	CDlgQuickViewer(CWnd* pParent = NULL);
	CDlgQuickViewer(CDC* pDC, CWnd* pParent = NULL);
	virtual ~CDlgQuickViewer();

// 对话框数据
	enum { IDD = IDD_DLG_QUICK_VIEWER };

protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
};