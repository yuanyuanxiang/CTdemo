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
		// �û�ȡ�����棬����true
		return true;
	}

	std::ofstream fout(path, std::ios::binary);

	if (fout)
	{
		switch (dlg.m_nRawHeader)
		{
		case -1://-1�򱣴�ɱ������Զ���raw��ʽ
		{
					// ���Կ����ļ�ͷ
					char* str = "# CTdemo";
					fout.write(str, sizeof(char)* strlen(str));
					// ����ȡ��߶ȡ������Զ�����д��RAW�ļ�
					// ��ȡ��߶���Ϊ�ļ�ͷ��ռ��8�ֽ�
					fout.write((char*)&nWidth, sizeof(int));
					fout.write((char*)&nHeight, sizeof(int));
					fout.write((char*)pSrc, sizeof(float)* nWidth * nHeight);
		}
			break;
		case 0://�����ȫ�����ݣ�û���ļ�ͷ��
		{
				   fout.write((char*)pSrc, sizeof(float)* nWidth * nHeight);
		}
			break;
		default:
		{
				   // ��0����ļ�ͷ
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


// ��ȡר�е�RAW������
bool FileOp::ReadPropRaw(float* &pDst, int &nWidth, int &nHeight, CString path)
{
	std::ifstream fin(path, std::ios::binary);
	if (fin)
	{
		// ��ȡ�ļ�ͷ
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
		temp1 = fin.tellg();			// ��¼�µ�ǰλ��
		fin.seekg(0, ios_base::end);	// �ƶ����ļ�β
		temp2 = fin.tellg();			// ȡ�õ�ǰλ�õ�ָ�볤��
		fin.seekg(temp1);				// �ƶ���ԭ����λ��
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


// ��ȡһ���������
bool FileOp::ReadRaw(float* &pDst, int &nWidth, int &nHeight, CString path)
{
	// ������ReadPropRaw��ȡ
	if (ReadPropRaw(pDst, nWidth, nHeight, path))
		return true;

	CDlgRawDataSize dlg;
	if (dlg.DoModal() != IDOK)
	{
		// �û����ȡ��
		return false;
	}

	// ħ�۵��ļ�ͷ��СΪ10816�ֽڣ�ԼΪ10.6k
	std::ifstream fin(path, std::ios::binary);
	if (fin)
	{
		nWidth = dlg.m_nRawWidth;
		nHeight = dlg.m_nRawHeight;
		int temp1, temp2;
		temp1 = fin.tellg();			// ��¼�µ�ǰλ��
		fin.seekg(0, ios_base::end);	// �ƶ����ļ�β
		temp2 = fin.tellg();			// ȡ�õ�ǰλ�õ�ָ�볤��
		fin.seekg(temp1);				// �ƶ���ԭ����λ��
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


// ��pSrcָ��ָ�������д���ļ�[path]��width��heightָ���˿����.
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
			// ÿ�����һ������
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
		// �ļ�����
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
