#pragma once

#include "atlimage.h"
#include "Functions.h"

//
// ********** CyImage���̳���CImage *********** //
// ***** �׶�ʦ����ѧ��Ԭ���飬2015��5��. ***** //
// 

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) if((p) != NULL){ delete [] (p); (p) = NULL; }	//��ȫɾ��ָ��p
#endif

#ifndef CHECK_IMAGE_NULL
#define CHECK_IMAGE_NULL(p) ( ((p) == NULL) || ((p)->IsNull()) ) //���ͼ���ǿ�
#endif

#ifndef RAD
#define RAD(i) (PI * (i) / 180.0f)	//�Ƕ�ת����
#endif // !RAD

class CyImage : public CImage
{
public:
	// �����������ĳ�Ա����
	float*						m_pfFloat;				//��Ӧͼ��ĸ�����
	float						m_fMaximum;				//�������ݵ����ֵ
	float						m_fMinimum;				//�������ݵ���Сֵ

	// �����ͼ����Ϣ�޷�ֱ�ӷ��ʣ��ʳ����²�
	BYTE*						m_pyBits;				//ͼ���׵�ַ
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
	void UpdateInfomation();												//����ͼ����Ϣ
	float* GetFloatData();													//��ȡ��������
	bool SetFloatData(float* pSrc, int nRowlen, int nHeight);				//���ø�������
	float GetMaximum();														//��ȡ���ֵ
	float GetMinimum();														//��ȡ��Сֵ
	float GetAt(float x, float y, int nCurChannel = 0);						//���ʸ�������
	float GetAt(int x, int y, int nCurChannel = 0);							//���ʸ�������

	// ͼ�񼸺α任
	void FlipH();															//ˮƽ��תͼ��
	void FlipV();															//��ֱ��תͼ��
	void Rotate(float degree);												//ͼ�񰴽Ƕ���ת
	void Zoom(float rate);													//ͼ�񰴱�������
	void Zoom(int nNewWidth, int nNewHeight);								//ͼ����������
	/* ��ͼ����ת��������ͼ������ݣ���ȡ��߶Ⱥ�ÿ�и����� */
	float* Rotate(float angle, int &NewWidth, int &NewHeight, int &NewRowlen);
	float* Rotate(float angle, float x0, float y0, int &NewWidth, int &NewHeight, int &NewRowlen);
	float* Rotate(float angle, float x0, float y0, int &Xmin, int &Ymin, int &Xmax, int &Ymax, int &NewWidth, int &NewHeight, int &NewRowlen);
	/* ���ݲ����Ŵ�ͼ�񣬷��ظ������� */
	float* Zoom(int NewWidth, int NewHeight, int Reserved);
	/* ͼ����ֱ�����߻��� */
	float Integrate(float &k, float &c, int nCurChannel = 0);
	/* ͼ����ĳ�����������߻��� */
	float* DirIntegrate(float angle, int nCurChannel = 0);
	/* ͼ��radon�任 */
	float* Radon(float angles_separation, int nAnglesNum, float pixels_separation, int nRaysNum, int nCurChannel = 0);

	// �ַ������ݺ͸�������֮�俽����ͬ������ʱ�������
	void MemcpyByteToFloat();												//�ַ����ݿ�������������
	void MemcpyFloatToByte();												//�������ݿ������ַ�����
	void MemcpyFloatToByteBounded(float lower, float upper);				//���������޿�������

	// ��ȡͼ����Ϣ
	bool BitMapModified();													//������λͼ�Ƿ����
	BYTE* GetHeadAddress();													//��ȡͼ���׵�ַ
	BYTE* GetLineAddress(int LineID);										//��ȡĳ�еĵ�ַ
	int GetChannel();														//��ȡͨ������
	int GetRowlen();														//��ȡÿ���ֽ���
	int GetDiagLength();													//��ȡͼ��Խ��߳�
	bool InitFloatData();													//��ʼ����������
	/* ��ȡͼ�����Ϣ */
	void GetInfomation(int &nWidth, int &nHeight);
	void GetInfomation(int &nWidth, int &nHeight, int &nRowlen);
	void GetInfomation(int &nWidth, int &nHeight, int &nRowlen, int &nBPP);
	void GetInfomation(int &nWidth, int &nHeight, int &nRowlen, int &nBPP, int &nChannel);

	/* �޸�ͼ��λ���*/
	BOOL	ChangeBPP(UINT bpp);						// �ı�ͼ��ͨ����
	void	Bpp8To24();									// 8λת24λ
	void	Bpp8To32();									// 8λת32λ
	void	Bpp24To8();									// 24λת8λ
	void	Bpp24To32();								// 24λת32λ
	void	Bpp32To8();									// 32λת8λ
	void	Bpp32To24();								// 32λת24λ
	void	Bpp1To8();									// ����ת8λ
	void	Bpp1To24();									// ����ת24λ
	void	Bpp1To32();									// ����ת32λ

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