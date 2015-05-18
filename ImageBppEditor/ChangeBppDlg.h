#pragma once

#include "afxwin.h"
#include <atlimage.h>
#include <afxdialogex.h>
#include "resource.h"

#define IDC_STATIC_BPP                  1
#define IDC_COMBO_BPP                   2

// CChangeBppDlg �Ի���

// ************************************************************* //
// ����DoModal����-1�����⡣��ע��������ĿΪ"�ھ�̬����ʹ�� MFC" //
// ���� FindResource �ʹ� dll ��Ѱ����Դ������� exe ��Ѱ����Դ  //
// ****************** 2015��3��24�� ��ע *********************** //

// �ڼ���ͼ���Сʱ�����ù�ʽ��biSizeImage = biWidth' �� biHeight��
// ��biWidth'��������biWidth�������biWidth'������4������������ʾ
// ���ڻ����biWidth�ģ���4�������������WIDTHBYTES������������
// biWidth'
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)

#define SAFE_DELETE(p)		{ if (p != NULL) { delete [] p; p = NULL; } }

class CChangeBppDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CChangeBppDlg)

public:
	CChangeBppDlg(CWnd* pParent = NULL);   // ��׼���캯��
	CChangeBppDlg(CImage* pImage, CWnd* pParent = NULL);
	virtual ~CChangeBppDlg();

// �Ի�������
	enum { IDD = IDD_DLG_CHANGE_BPP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	CImage*			m_pImage;							// �ⲿ�����ָ��
	CStatic*		m_pStaticBpp;						// ��ʾ��ǰλ���
	CComboBox*		m_pComboBpp;						// λ��ȸ�ѡ��
	CButton*		m_pBnOK;							// Ok ��ť
	CButton*		m_pBnCancel;						// Cancel ��ť
	CFont			m_Font;								// �ؼ�������
	int				m_nTargetBpp;						// Ŀ��ͼ��λ���
	BOOL			ChangeBPP(UINT bpp);				// �ı�ͼ��ͨ����
	afx_msg void	OnSelchangeComboBpp();				// �ı�λ���
	afx_msg void	OnBnClickedOk();					// ��OK����Ӧ����
	afx_msg void	OnBnClickCancel();					// "Cancel"��Ӧ����
	virtual BOOL	OnInitDialog();						// ��ʼ������
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);// ����ʱִ��

private:
	void	Bpp8To24();									// 8λת24λ
	void	Bpp8To32();									// 8λת32λ
	void	Bpp24To8();									// 24λת8λ
	void	Bpp24To32();								// 24λת32λ
	void	Bpp32To8();									// 32λת8λ
	void	Bpp32To24();								// 32λת24λ
	void	Bpp1To8();									// ����ת8λ
	void	Bpp1To24();									// ����ת24λ
	void	Bpp1To32();									// ����ת32λ
};


void SafeDestroy(CImage* pImage);						// ��ȫɾ������λͼ