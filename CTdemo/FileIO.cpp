#include "stdafx.h"
#include "FileIO.h"
#include <fstream>
using namespace std;


/** 
* @brief 将BYTE图像数据以二进制形式读入文件，nChannel表示一个像素占用几个字节
* @param[in] *fileName 文件名
* @param[out] &nHeight 图像高度
* @param[out] &nHeight 每行字节数
* @param[out] &nRowlen 每行字节数
* @param[out] &nChannel 每像素字节
* @return 图像数据指针
*/
BYTE* ReadTxt(const char* fileName, int &nWidth, int &nHeight, int &nRowlen, int &nChannel)
{
	ifstream fin;
	fin.open(fileName, ifstream::binary);
	nWidth = 0;

	if (fin.is_open())
	{
		BYTE *pSrc = NULL;
		try
		{
			// 获取文件大小
			streampos pos = fin.tellg();
			fin.seekg(0, ios::end);
			long nFileSize = (long)fin.tellg();
			fin.seekg(pos);
			if (nFileSize <= HEADER_SIZE)
			{
				fin.close();
				return NULL;
			}
			// 读取文件头
			const char* check = "yuanyuanxiang";
			const int nLen = strlen(check);
			char* pHead = new char[nLen];
			fin.read(pHead, nLen);
			for (int i = 0; i < nLen; ++i)
			{
				if (pHead[i] != check[i])
				{
					SAFE_DELETE(pHead);
					fin.close();
					return FALSE;
				}
			}
			SAFE_DELETE(pHead);
			int nInt = sizeof(int);
			fin.read((char*)&nRowlen, nInt);
			fin.read((char*)&nHeight, nInt);
			fin.read((char*)&nChannel, nInt);
			if (nRowlen * nHeight + HEADER_SIZE <= nFileSize)
			{
				pSrc = new BYTE[nRowlen * nHeight];
				fin.seekg(HEADER_SIZE);
				fin.read((char*)pSrc, nRowlen * nHeight);
				fin.close();
				nWidth = nRowlen / nChannel;
				return pSrc;
			}
		}
		catch (...)
		{
			fin.close();
			return NULL;
		}
	}
	return NULL;
}