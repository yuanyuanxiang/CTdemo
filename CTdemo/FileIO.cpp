#include "stdafx.h"
#include "FileIO.h"
#include <fstream>
using namespace std;


/** 
* @brief ��BYTEͼ�������Զ�������ʽ�����ļ���nChannel��ʾһ������ռ�ü����ֽ�
* @param[in] *fileName �ļ���
* @param[out] &nHeight ͼ��߶�
* @param[out] &nHeight ÿ���ֽ���
* @param[out] &nRowlen ÿ���ֽ���
* @param[out] &nChannel ÿ�����ֽ�
* @return ͼ������ָ��
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
			// ��ȡ�ļ���С
			streampos pos = fin.tellg();
			fin.seekg(0, ios::end);
			long nFileSize = (long)fin.tellg();
			fin.seekg(pos);
			if (nFileSize <= HEADER_SIZE)
			{
				fin.close();
				return NULL;
			}
			// ��ȡ�ļ�ͷ
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