#pragma once
#include "afxwin.h"


// CDlgArtSettings �Ի���

class CDlgArtSettings : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgArtSettings)

public:
	CDlgArtSettings(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgArtSettings();

// �Ի�������
	enum { IDD = IDD_ART_IT_NUM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EditArtItNum;
	int m_nArtItNum;
};
