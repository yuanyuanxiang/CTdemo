#include "stdafx.h"
#include "FileOp.h"
#include <fstream>
#include <vector>
using namespace std;
#include "DlgRawDataSize.h"

FileOp::FileOp()
{
}


FileOp::~FileOp()
{
}

//////////////////////////////////////////////////////////////////////////


bool FileOp::Write2Raw(float* pSrc, int nWidth, int nHeight, CString path)
{
	CDlgRawDataSize dlg(FALSE);
	dlg.m_nRawWidth = nWidth;
	dlg.m_nRawHeight = nHeight;
	if (dlg.DoModal() != IDOK)
	{
		// 用户取消保存，返回true
		return true;
	}

	std::ofstream fout(path, std::ios::binary);

	if (fout)
	{
		switch (dlg.m_nRawHeader)
		{
		case -1://-1则保存成本程序自定义raw格式
		{
					// 可以看作文件头
					char* str = "# CTdemo";
					fout.write(str, sizeof(char)* strlen(str));
					// 将宽度、高度、数据以二进制写入RAW文件
					// 宽度、高度作为文件头，占用8字节
					fout.write((char*)&nWidth, sizeof(int));
					fout.write((char*)&nHeight, sizeof(int));
					fout.write((char*)pSrc, sizeof(float)* nWidth * nHeight);
		}
			break;
		case 0://保存成全裸数据（没有文件头）
		{
				   fout.write((char*)pSrc, sizeof(float)* nWidth * nHeight);
		}
			break;
		default:
		{
				   // 用0填充文件头
				   int i = 0;
				   char* c = "0";
				   while (i < dlg.m_nRawHeader)
				   {
					   fout.write(c, sizeof(char));
					   i++;
				   }
				   fout.write((char*)pSrc, sizeof(float)* nWidth * nHeight);
		}
			break;
		}

		fout.close();
		return true;
	}
	return false;
}


// 读取专有的RAW裸数据
bool FileOp::ReadPropRaw(float* &pDst, int &nWidth, int &nHeight, CString path)
{
	std::ifstream fin(path, std::ios::binary);
	if (fin)
	{
		// 读取文件头
		char* str = "# CTdemo";
		int len = strlen(str);
		char* buf = new char[len];
		fin.read(buf, sizeof(char)* len);
		for (int i = 0; i < len; ++i)
		{
			if (str[i] != buf[i])
			{
				SAFE_DELETE(buf);
				fin.close();
				return false;
			}
		}
		SAFE_DELETE(buf);
		fin.read((char*)&nWidth, sizeof(int));
		fin.read((char*)&nHeight, sizeof(int));
		int temp1, temp2;
		temp1 = fin.tellg();			// 记录下当前位置
		fin.seekg(0, ios_base::end);	// 移动到文件尾
		temp2 = fin.tellg();			// 取得当前位置的指针长度
		fin.seekg(temp1);				// 移动到原来的位置
		if (temp2 - temp1 != sizeof(float)* nWidth * nHeight)
		{
			fin.close();
			return false;
		}
		pDst = new float[nWidth * nHeight];
		fin.read((char*)pDst, sizeof(float)* nWidth * nHeight);
		fin.close();
		return true;
	}
	return false;
}


// 读取一般的裸数据
bool FileOp::ReadRaw(float* &pDst, int &nWidth, int &nHeight, CString path)
{
	// 尝试用ReadPropRaw读取
	if (ReadPropRaw(pDst, nWidth, nHeight, path))
		return true;

	CDlgRawDataSize dlg;
	if (dlg.DoModal() != IDOK)
	{
		// 用户点击取消
		return false;
	}

	// 魔眼的文件头大小为10816字节，约为10.6k
	std::ifstream fin(path, std::ios::binary);
	if (fin)
	{
		nWidth = dlg.m_nRawWidth;
		nHeight = dlg.m_nRawHeight;
		int temp1, temp2;
		temp1 = fin.tellg();			// 记录下当前位置
		fin.seekg(0, ios_base::end);	// 移动到文件尾
		temp2 = fin.tellg();			// 取得当前位置的指针长度
		fin.seekg(temp1);				// 移动到原来的位置
		if (temp2 - temp1 - dlg.m_nRawHeader < sizeof(float)* nWidth * nHeight)
		{
			fin.close();
			return false;
		}
		fin.seekg(dlg.m_nRawHeader);
		pDst = new float[nWidth * nHeight];
		fin.read((char*)pDst, sizeof(float)* nWidth * nHeight);
		fin.close();
		return true;
	}
	return false;
}


// 将pSrc指针指向的数据写入文件[path]，width和height指定了宽与高.
bool FileOp::Write2Txt(float* pSrc, int nWidth, int nHeight, CString path)
{
	ofstream InputFile(path);
	if (InputFile)
	{
		for (int i = 0; i < nHeight; i++)
		{
			for (int j = 0; j < nWidth - 1; j++)
			{
				InputFile << pSrc[j + i * nWidth] << ",";
			}
			// 每行最后一个数据
			InputFile << pSrc[nWidth - 1 + i * nWidth] << endl;
		}
		InputFile.close();
		return true;
	}
	return false;
}


bool FileOp::ReadTxt(float* &pDst, int &nWidth, int &nHeight, CString path)
{
	CString strText = _T("");
	CString szLine = _T("");
	CStdioFile file;
	if (file.Open(path, CFile::modeRead))
	{
		// 文件行数
		nHeight = 0;
		vector<float> TxtData;
		while (file.ReadString(szLine))
		{
			//strText += szLine + _T("\n");
			int Length = szLine.GetLength();
			int k, i = 0;
			while (i < Length)
			{
				k = i;
				while (i < Length && szLine[i] != ',')
				{
					i++;
				}
				CString str_temp = szLine.Mid(k, i - k);
				float data_temp = _tstof(str_temp);
				TxtData.push_back(data_temp);
				i++;
			}
			nHeight++;
		}
		int Total = TxtData.size();
		if (nHeight == 0 || Total % nHeight != 0)
		{
			file.Close();
			return false;
		}
		nWidth = Total / nHeight;
		pDst = new float[Total];
		for (int i = 0; i < Total; ++i)
		{
			pDst[i] = TxtData[i];
		}
		file.Close();
		return true;
	}
	return false;
}


// Load raw data from disk
void * FileOp::loadRawFile(CString filename, size_t size)
{
	std::ifstream fin(filename, std::ios::binary);
	if (fin)
	{
		void *data = malloc(size);
		fin.read((char*)data, size);
		fin.close();
		return data;
	}
	return NULL;
}
