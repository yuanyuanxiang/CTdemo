
// CTdemo.h : CTdemo Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


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
	bool		m_bProjectionFile;	//��ǰͼ��ΪͶӰ����
	BOOL		m_bHiColorIcons;

	CMultiDocTemplate * m_pDocTemplate;

	bool CheckProjectionFile();		//����Ƿ�ΪͶӰ����
	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnAppAbout();
	afx_msg void OnLoadProject();
	afx_msg void OnFileNew();
};

extern CCTdemoApp theApp;
