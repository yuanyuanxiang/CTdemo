#pragma once
#include "afxwin.h"


// CDlgRotateAngle �Ի���

class CDlgRotateAngle : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRotateAngle)

public:
	CDlgRotateAngle(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgRotateAngle();

// �Ի�������
	enum { IDD = IDD_ROTATE_ANGLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EditRotateAngle;
	float m_fRotateAngle;
};
