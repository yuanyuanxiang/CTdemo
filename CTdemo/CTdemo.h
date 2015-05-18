
// CTdemo.h : CTdemo 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号


// CCTdemoApp:
// 有关此类的实现，请参阅 CTdemo.cpp
//

class CCTdemoApp : public CWinAppEx
{
public:
	CCTdemoApp();


// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
	bool		m_bProjectionFile;	//当前图像为投影数据
	BOOL		m_bHiColorIcons;

	CMultiDocTemplate * m_pDocTemplate;

	bool CheckProjectionFile();		//检查是否为投影数据
	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnAppAbout();
	afx_msg void OnLoadProject();
	afx_msg void OnFileNew();
};

extern CCTdemoApp theApp;
