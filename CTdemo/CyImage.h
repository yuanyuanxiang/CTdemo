#pragma once

#include "atlimage.h"
#include "Functions.h"
//
// ********** CyImage���̳���CImage ********** //
//

class CyImage : public CImage
{
public:
	// �����������ĳ�Ա����
	float*						m_pfFloat;				//��Ӧͼ��ĸ�����
	float						m_fMaximum;				//�������ݵ����ֵ
	float						m_fMinimum;				//�������ݵ���Сֵ

	// �����ͼ����Ϣ�޷�ֱ�ӷ��ʣ��ʳ����²�
	int							m_nyWidth;				//ͼ����
	int							m_nyHeight;				//ͼ��߶�
	int							m_nyRowlen;				//ͼ��ÿ���ֽ���
	int							m_nyRowlen2;			//ÿ�и�����
	int							m_nyBpp;				//ͼ��λ���
	int							m_nyChannel;			//ÿ�����ֽ���

	// ���� / ����
	CyImage();
	~CyImage();

	// ���������Ա�����Ĳ���
	void UpdateImageInfomation();											//����ͼ����Ϣ
	float* GetFloatData();													//��ȡ��������
	bool SetFloatData(float* pSrc, int nRowlen, int nHeight);				//���ø�������
	float GetMaximum();														//��ȡ���ֵ
	float GetMinimum();														//��ȡ��Сֵ
	float GetAt(float x, float y, int nCurChannel = 0);						//���ʸ�������
	float GetAt(int x, int y, int nCurChannel = 0);							//���ʸ�������
	float* Rotate(float angle, int &NewWidth, int &NewHeight, int &NewRowlen);
	float* Rotate(float angle, float x0, float y0, int &NewWidth, int &NewHeight, int &NewRowlen);
	float* Rotate(float angle, float x0, float y0, int &Xmin, int &Ymin, int &Xmax, int &Ymax, int &NewWidth, int &NewHeight, int &NewRowlen);
	float* Zoom(int NewWidth, int NewHeight);
	float Integrate(float &k, float &c, int nCurChannel = 0);
	void DirIntegrate(float* pDst, int nLength, float angle, float sub_pixel, int nCurChannel = 0);
	void Radon(float* pDst, float angles_separation, int nAnglesNum, float rays_separation, int nRaysNum, int nCurChannel = 0);

	// ���ݿ���
	void MemcpyByteToFloat();
	void MemcpyFloatToByte();

	// ��ȡͼ����Ϣ
	BYTE* GetHeadAddress();
	BYTE* GetLineAddress(int LineID);
	int GetChannel();
	int GetRowlen();
	int GetDiagLength();
	bool InitFloatData();
	void GetInfomation(int &nWidth, int &nHeight);
	void GetInfomation(int &nWidth, int &nHeight, int &nRowlen);
	void GetInfomation(int &nWidth, int &nHeight, int &nRowlen, int &nBPP);
	void GetInfomation(int &nWidth, int &nHeight, int &nRowlen, int &nBPP, int &nChannel);

	// ��������
	BOOL Create(int nWidth, int nHeight, int nBPP, DWORD dwFlags = 0) throw();
	HRESULT Load(LPCTSTR pszFileName) throw();
	HRESULT Save(LPCTSTR pszFileName, REFGUID guidFileType = GUID_NULL) const throw();
	bool Save(REFGUID guidFileType) const throw();
	bool Save(CWnd* pParentWnd = NULL, REFGUID guidFileType = GUID_NULL) const throw();
	BOOL Create(float* pSrc, int nWidth, int nHeight, int nRowlen, DWORD dwFlags = 0) throw();
	void Destroy() throw();

private:
	void ComputeMinMax();
};