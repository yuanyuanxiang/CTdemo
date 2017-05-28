#pragma once


class FileOp
{
public:
	FileOp();
	~FileOp();

	//////////////////////////////////////////////////////////////////////////

	// ��������д���ı��ĵ�
	bool Write2Txt(float* pSrc, int nWidth, int nHeight, CString path);

	bool ReadTxt(float* &pDst, int &nWidth, int &nHeight, CString path);

	// ��������д��ר��RAW�ļ�
	bool Write2Raw(float* pSrc, int nWidth, int nHeight, CString path);

	// ��ȡר��raw��ʽ�ļ�
	bool ReadPropRaw(float* &pDst, int &nWidth, int &nHeight, CString path);

	// ��ȡһ���������
	bool ReadRaw(float* &pDst, int &nWidth, int &nHeight, CString path);

	void *loadRawFile(CString filename, size_t size);
};

