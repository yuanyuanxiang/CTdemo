#pragma once


class FileOp
{
public:
	FileOp();
	~FileOp();

	//////////////////////////////////////////////////////////////////////////

	// 将浮点数写入文本文档
	bool Write2Txt(float* pSrc, int nWidth, int nHeight, CString path);

	bool ReadTxt(float* &pDst, int &nWidth, int &nHeight, CString path);

	// 将浮点数写入专有RAW文件
	bool Write2Raw(float* pSrc, int nWidth, int nHeight, CString path);

	// 读取专有raw格式文件
	bool ReadPropRaw(float* &pDst, int &nWidth, int &nHeight, CString path);

	// 读取一般的裸数据
	bool ReadRaw(float* &pDst, int &nWidth, int &nHeight, CString path);

	void *loadRawFile(CString filename, size_t size);
};

