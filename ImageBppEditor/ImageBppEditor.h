// ImageBppEditor.h : ImageBppEditor DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CImageBppEditorApp
// �йش���ʵ�ֵ���Ϣ������� ImageBppEditor.cpp
//

class CImageBppEditorApp : public CWinApp
{
public:
	CImageBppEditorApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
