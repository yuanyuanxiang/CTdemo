
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

	// ��ȡ���ĵ�
	CCTdemoDoc* GetMainDoc();

	// ��ȡ����ͼ
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
	afx_msg void OnAppExit();
};


extern CCTdemoApp theApp;


// ��ȡ��ǰ��ʾ��ͼ��ָ��
CyImage* AfxGetImage();