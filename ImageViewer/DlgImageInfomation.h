#pragma once


// CDlgImageInfomation �Ի���

class CDlgImageInfomation : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgImageInfomation)

public:
	CImage* m_pImage;
	void SetImage(CImage* pImage);
	void ShowInfomation();
	CDlgImageInfomation(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgImageInfomation();

// �Ի�������
	enum { IDD = IDD_DLG_IMAGE_INFOMATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
