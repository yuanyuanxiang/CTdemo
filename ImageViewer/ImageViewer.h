// ImageViewer.h : ImageViewer DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "DlgImageViewer.h"


// CImageViewerApp
// �йش���ʵ�ֵ���Ϣ������� ImageViewer.cpp
//

class CImageViewerApp : public CWinApp
{
public:
	CImageViewerApp();

public:
	virtual BOOL InitInstance();
	DECLARE_MESSAGE_MAP()
};