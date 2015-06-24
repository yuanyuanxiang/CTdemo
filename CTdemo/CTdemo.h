
// CTdemo.h : CTdemo Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������
#include "CTdemoDoc.h"
#include "CTdemoView.h"


// CCTdemoApp:
// �йش����ʵ�֣������ CTdemo.cpp
//

class CCTdemoApp : public CWinAppEx
{
public:
	CCTdemoApp();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��
	BOOL		m_bHiColorIcons;
	bool		m_bUsingOpenGL;

	CCTdemoDoc* GetMainDoc();
	CCTdemoView* GetMainView();

	CMultiDocTemplate * m_pDocTemplate;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	afx_msg void OnFileOpenUsingOpenGL();
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
};

extern CCTdemoApp theApp;
