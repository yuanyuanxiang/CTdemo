#ifdef WIN32

#pragma once

#include "atlimage.h"
#include "DataTypes.h"
#include "ImageSrc.h"

/** 
* @file CyImage.h
* @brief ͼ���ȡ/д��/������
* @author Ԭ����
*/

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) if((p) != NULL){ delete [] (p); (p) = NULL; }/**< ��ȫɾ��ָ��p */
#endif

#ifndef RAD
#define RAD(i) (PI * (i) / 180.0f)	/**< �Ƕ�ת���� */
#endif

/**
* @class CyImage
* @brief �̳���CImage 
* @author Ԭ����
* @date 2015��5��
* @warning ������ATL����
* @remark yuan_yuanxiang@163.com
*/
class CyImage : public CImage
{
public:
	// ���캯��
	CyImage();
	// ��������
	~CyImage();
	// �������캯��
	CyImage(const CyImage &pImage);
	// ��ֵ���������
	CyImage operator = (const CyImage &pImage);

	float* GetFloatDataHead() const;										//��ȡ��������ͷָ��
	int GetFloatDataRowlen() const;											//��ȡÿ�и������ݸ���
	BOOL SetFloatData(float* pSrc, int nRowlen, int nHeight);				//���ø�������
	float GetMaximum() const;												//��ȡ���ֵ
	float GetMinimum() const;												//��ȡ��Сֵ

	void FlipH();															//ˮƽ��תͼ��
	void FlipV();															//��ֱ��תͼ��
	void Transpose();														//ת��ͼ��
	void Rotate(float degree = 90.f);										//ͼ�񰴽Ƕ���ת
	void Zoom(float rate = 2.f);											//ͼ�񰴱�������
	void Zoom(int nNewWidth, int nNewHeight);								//ͼ����������
	void Cut(CLogoRect rect = CLogoRect(0, 0, 0, 0));						//ͼ��ü�
	void ToGray();															//ת��Ϊ�Ҷ�ͼ
	BOOL ChangeBPP(UINT bpp);												//�޸�ͼ��λ���

	void MemcpyByteToFloat();												//�ַ����ݿ�������������
	void MemcpyFloatToByte();												//�������ݿ������ַ�����
	void MemcpyFloatToByteBounded(float lower = 0, float upper = 255);		//���������޿�������

	/* ��ȡͼ�������*/

	BYTE* GetHeadAddress() const;											//��ȡͼ���׵�ַ
	BYTE* GetLineAddress(int nLineID) const;								//��ȡĳ�еĵ�ַ
	int GetChannel() const;													//��ȡͨ������
	int GetRowlen() const;													//��ȡÿ���ֽ���
	int GetLength() const;													//��ȡ�ܵ��ֽ���
	int GetDiagLength() const;												//��ȡͼ��Խ��߳�
	void GetInfomation(int &nWidth, int &nHeight) const;					//��ȡͼ�����Ϣ
	void GetInfomation(int &nWidth, int &nHeight, int &nRowlen) const;
	void GetInfomation(int &nWidth, int &nHeight, int &nRowlen, int &nBPP) const;
	void GetInfomation(int &nWidth, int &nHeight, int &nRowlen, int &nBPP, int &nChannel) const;
	BOOL GetHistogram(int hist[4][256]) const;						/* ��ȡͼ��R��G��Bͨ��ֱ��ͼ*/
	BOOL GetGrayHist(int hist[256]) const;									/* ��ȡ�Ҷ�ֱ��ͼ */
	void GetCluster(int cluster[256]) const;								/* ���ذ��������� */
	// 

	/* ��������ͼ����Ҫdelete���� */
	CyImage* MakeCopy();

	/* ��ȡ����ͼ����Ҫdelete���� */
	CyImage* GetBackground(float threahold, float3 background);

	/* ��ȡ����Ȥ������Ҫdelete���� */
	CyImage* ROI(CLogoRect rect = CLogoRect(0, 0, 0, 0));

	/* ��ͼ����ת��������ͼ������ݣ���ȡ��߶Ⱥ�ÿ�и���������Ҫdelete���� */
	float* Rotate(float angle, int &NewWidth, int &NewHeight, int &NewRowlen);
	float* Rotate(float angle, float x0, float y0, int &NewWidth, int &NewHeight, int &NewRowlen);
	float* Rotate(float angle, float x0, float y0, int &Xmin, int &Ymin, int &Xmax, int &Ymax, 
		int &NewWidth, int &NewHeight, int &NewRowlen);

	/* ���ݲ����Ŵ�ͼ�񣬷��ظ������ݣ���Ҫdelete���� */
	float* Zoom(int NewWidth, int NewHeight, int bNeededReturn);
	//

	// ����ͼ��
	BOOL Create(const BYTE* pSrc, int nWidth, int nHeight, int nBPP) throw();
	BOOL Create(const float* pSrc, int nWidth, int nHeight, int nRowlen) throw();
	BOOL Create(const ImageSrc *pSrc) throw();
	// ���غ���
	BOOL Create(int nWidth, int nHeight, int nBPP, DWORD dwFlags = 0) throw();
	HRESULT Load(LPCTSTR pszFileName) throw();
	HRESULT Save(LPCTSTR pszFileName, REFGUID guidFileType = GUID_NULL) const throw();
	void Destroy() throw();

#ifdef _AFX // MFC
	HRESULT Load(CWnd* pParentWnd = NULL) throw();
	BOOL Save(REFGUID guidFileType) const throw();
	BOOL Save(CWnd* pParentWnd = NULL, REFGUID guidFileType = GUID_NULL) const throw();
#endif

private:
	float*	m_pFloatData;													/**< ��Ӧͼ��ĸ����� */
	float	m_fMaximum;														/**< �������ݵ����ֵ */
	float	m_fMinimum;														/**< �������ݵ���Сֵ */
	
	BOOL	InitFloatData();												//��ʼ����������
	void	ComputeMinMax();												//�������������Сֵ
	void	Bpp8To24();														// 8λת24λ
	void	Bpp8To32();														// 8λת32λ
	void	Bpp24To8();														// 24λת8λ
	void	Bpp24To32();													// 24λת32λ
	void	Bpp32To8();														// 32λת8λ
	void	Bpp32To24();													// 32λת24λ
	void	Bpp1To8();														// ����ת8λ
	void	Bpp1To24();														// ����ת24λ
	void	Bpp1To32();														// ����ת32λ
};

// swap ����int��Ԫ�صĴ���
void swap(int &nElem1, int &nElem2);

// Ϊ8λɫͼ��������ɫ��
void SetColorTabFor8BitImage(CImage *pImage);

// ��file path��ȡfile name
CString GetFileName(CString strFilePath, BOOL bHasPostfix = TRUE);

// ��file path��ȡfile ext
CString GetFileExt(CString strFilePath);

// ����תΪCString
CString Num2String(int nNum);

// ��Ŀ��ͼ���ڳ�һ�������Դͼ���滻
void ReplacedLogo(CyImage* pDstImage, CyImage* pSrcImage, CLogoRect LogoRect);

// ��Ŀ��ͼ���ڳ�һ�������Դͼ����
void MixedLogo(CyImage* pDstImage, CyImage* pSrcImage, CLogoRect LogoRect, float dst_rate);

#endif // WIN32