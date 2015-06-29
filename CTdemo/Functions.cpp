#include "stdafx.h"
#include "Functions.h"
#include "IntSection.h"
#include <algorithm>
#include <limits>
#include "DlgRawDataSize.h"

// 不显示数据截断警告
#pragma warning(disable: 4244)

#define SUPPORTED_FORMATS_NUM 20

float GetPositionValue(float *pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, int nCurChannel, int x, int y)
{
	if (x < 0 || x >= nWidth || y < 0 || y >= nHeight)
		return 0;
	return (float)*(pSrc + nCurChannel + x * nChannel + y * nRowlen);
}


float biLinearInterp(float *pSrc, int &nWidth, int &nHegiht, int &nRowlen, int &nChannel, int nCurChannel, float x, float y)
{
	int x1, x2, x3, x4, y1, y2, y3, y4;
	float Ans1, Ans2;
	x1 = floor(x);	y1 = floor(y);
	x2 = x1 + 1;	y2 = y1;
	x3 = x2;		y3 = y1 + 1;
	x4 = x1;		y4 = y3;
	Ans1 = GetPositionValue(pSrc, nWidth, nHegiht, nRowlen, nChannel, nCurChannel, x1, y1) * (1 - x + x1) 
		+ GetPositionValue(pSrc, nWidth, nHegiht, nRowlen, nChannel, nCurChannel, x2, y2) * (x - x1);
	Ans2 = GetPositionValue(pSrc, nWidth, nHegiht, nRowlen, nChannel, nCurChannel, x4, y4) * (1 - x + x4) 
		+ GetPositionValue(pSrc, nWidth, nHegiht, nRowlen, nChannel, nCurChannel, x3, y3) * (x - x4);
	return (Ans1 * (1 - y + y1) + Ans2 * (y - y1));
}


void PositionTransform(float &x, float &y, float theta, float x0, float y0)
{
	float delta_x = x - x0;
	float delta_y = y - y0;
	PositionTransform(delta_x, delta_y, theta);
	x = x0 + delta_x;
	y = y0 + delta_y;
}


void PositionTransform(float &x, float &y, float theta)
{
	float cos_theta = cos(theta);
	float sin_theta = sin(theta);
	float x_temp = x * cos_theta - y * sin_theta;
	y = x * sin_theta + y * cos_theta;
	x = x_temp;
}


// 坐标旋转，输入参数为旋转角度的余弦、正弦值
void PositionTransform(float &x, float &y, float cos_sin[2], float x0, float y0)
{
	float delta_x = x - x0;
	float delta_y = y - y0;
	PositionTransform(delta_x, delta_y, cos_sin);
	x = x0 + delta_x;
	y = y0 + delta_y;
}


// 坐标旋转，输入参数为旋转角度的余弦、正弦值
void PositionTransform(float &x, float &y, float cos_sin[2])
{
	float x_temp = x * cos_sin[0] - y * cos_sin[1];
	y = x * cos_sin[1] + y * cos_sin[0];
	x = x_temp;
}


// 坐标旋转，输入参数为旋转角度的余弦、正弦值
void PositionTransform(float &x, float &y, float cos_theta, float sin_theta, float x0, float y0)
{
	float delta_x = x - x0;
	float delta_y = y - y0;
	PositionTransform(delta_x, delta_y, cos_theta, sin_theta);
	x = x0 + delta_x;
	y = y0 + delta_y;
}


// 坐标旋转，输入参数为旋转角度的余弦、正弦值
void PositionTransform(float &x, float &y, float cos_theta, float sin_theta)
{
	float x_temp = x * cos_theta - y * sin_theta;
	y = x * sin_theta + y * cos_theta;
	x = x_temp;
}


float FindMaxBetween4Numbers(float x, float y, float z, float w)
{
	float Max1, Max2;
	Max1 = x > y ? x : y;
	Max2 = z > w ? z : w;
	return (Max1 > Max2 ? Max1 : Max2);
}


float FindMinBetween4Numbers(float x, float y, float z, float w)
{
	float Min1, Min2;
	Min1 = x < y ? x : y;
	Min2 = z < w ? z : w;
	return (Min1 < Min2 ? Min1 : Min2);
}


/*
float* pDst					目标
float* pSrc					指向投影数据的指针
int nWidth					宽度，此处表示fai
int nHeight					高度，此处表示r
float delta_r				离散化之后的dr
float w0					高频截止
*/
void Convolute(float* pDst, float* pSrc, int nWidth, int nHeight, float delta_r, float w0, int nConvKernel)
{
	int m, n, i;
	float sum = 0.f;

	switch (nConvKernel)
	{
	case CONVOLUTE_KERNEL_COSINE:
#pragma omp parallel for private(m, n, i) reduction(+ : sum)
		for (m = 0; m < nHeight; ++m)
		{
			for (n = 0; n < nWidth; ++n)
			{
				sum = 0.f;
				for (i = 0; i < nHeight; ++i)
					sum += pSrc[n + i * nWidth] * CosineKernel(m - i, w0);
				pDst[n + m * nWidth] = sum * delta_r;
			}
		}
		break;
	case CONVOLUTE_KERNEL_RL:
#pragma omp parallel for private(m, n, i) reduction(+ : sum)
		for (m = 0; m < nHeight; ++m)
		{
			for (n = 0; n < nWidth; ++n)
			{
				sum = 0.f;
				for (i = 0; i < nHeight; ++i)
					sum += pSrc[n + i * nWidth] * R_LKernel(m - i, delta_r);
				pDst[n + m * nWidth] = sum * delta_r;
			}
		}
		break;
	case CONVOLUTE_KERNEL_SL:
#pragma omp parallel for private(m, n, i) reduction(+ : sum)
		for (m = 0; m < nHeight; ++m)
		{
			for (n = 0; n < nWidth; ++n)
			{
				sum = 0.f;
				for (i = 0; i < nHeight; ++i)
					sum += pSrc[n + i * nWidth] * S_LKernel(m - i, delta_r);
				pDst[n + m * nWidth] = sum * delta_r;
			}
		}
		break;
	case CONVOLUTE_KERNEL_HAMMING:
#pragma omp parallel for private(m, n, i) reduction(+ : sum)
		for (m = 0; m < nHeight; ++m)
		{
			for (n = 0; n < nWidth; ++n)
			{
				sum = 0.f;
				for (i = 0; i < nHeight; ++i)
					sum += pSrc[n + i * nWidth] * HammingKernel(m - i, delta_r, w0);
				pDst[n + m * nWidth] = sum * delta_r;
			}
		}
		break;
	default:
		break;
	}
}


// 卷积核：HW(r). w0 - 高频截止
float CosineKernel(float x, float w0)
{
	if(x == 0) return w0 * w0;
	float theta = 2 * PI * w0 * x;
	return (-1 + cos(theta) + theta * sin(theta)) / (2 * PI * PI * x * x);
}


// R_L窗函数:n-探测器编号；d-探测器间距
float R_LKernel(int n, float d)
{
	if (n == 0)
		return 1.f / (4 * d * d);
	if (n % 2 == 0)
		return 0.f;
	else
		return -1.f / (PI * PI * n * n * d * d);
	return 0.f;
}


// S_L窗函数:n-探测器编号；d-探测器间距
float S_LKernel(int n, float d)
{
	return 2.f / ((PI * PI * d * d) * (1 - 4 * n * n));
}


// Hamming窗函数:n-探测器编号；d-探测器间距
float HammingKernel(int n, float d, float a)
{
	if (n == 0)
		return -(4 + (-4 + PI * PI) * a) / (4 * d * d * PI * PI);
	if (n == 1 || n == -1)
		return (PI * PI * (-1 + a) - 8 * a) / (8 * d * d * PI * PI);
	if (n % 2 == 0)
		return (-a + n * n * (-(-3 * a + n * n + 1)) + (a + (2 * a - 1) * n * n * n * n - (a + 1) * n * n)) / (2 * PI * PI * d * d * n * n * (n * n - 1) * (n * n - 1));
	else
		return (-a + n * n * (-(-3 * a + n * n + 1)) - (a + (2 * a - 1) * n * n * n * n - (a + 1) * n * n)) / (2 * PI * PI * d * d * n * n * (n * n - 1) * (n * n - 1));
	return 0.f;
}


/*
float* pDst			目标
float* pPrj			卷积后的投影
int nWidth			重建图像的宽
int nHeight			重建图像的高
int nRays			投影的行数(r)
int nAngles			投影的列数(fai)
float delta_r		射线密度
float delta_fai		扫描参数
*/
void BackProject(float* pDst, float* pPrj, int nWidth, int nHeight, int nRays, int nAngles, float delta_r, float delta_fai)
{
	int m, n, i;
	int cx = (nWidth + 1) / 2;
	int cy = (nHeight + 1) / 2;
	int med = (nRays + 1) / 2;
	float sum = 0.f;

	// 2015.5.19 将三角函数放到循环体外面
	float *cos_fai = new float[nAngles];
	float *sin_fai = new float[nAngles];

#pragma omp parallel for
	for (i = 0; i < nAngles; ++i)
	{
		float fai = i * delta_fai;
		cos_fai[i] = cos(fai);
		sin_fai[i] = sin(fai);
	}

#pragma omp parallel for private(m, n, i) reduction(+ : sum)
	for (m = 0; m < nHeight; ++m)
	{
		for (n = 0; n < nWidth; ++n)
		{
			sum = 0.f;
			for (i = 0; i < nAngles; ++i)
			{
				float fai = i * delta_fai;
				float r = (n - cx) * cos_fai[i] + (m - cy) * sin_fai[i];
				sum += LinearInterp(pPrj, nAngles, nRays, i, med + r * delta_r);
			}
			pDst[n + m * nWidth] = sum * delta_fai;
		}
	}

	SAFE_DELETE(cos_fai);
	SAFE_DELETE(sin_fai);
}


// 线性一维插值:插值出x的值.
float LinearInterp(float* pPrj, int nWidth, float x)
{
	int x1 = floor(x), x2 = x1 + 1;
	if ( x1 < 0 || x2 >= nWidth)
		return 0;
	return pPrj[x1] * (1 - x + x1) + pPrj[x2] * (x - x1);
}


// 线性插值:在width*height网格中插值出(x, y)的值.
float LinearInterp(float* pPrj, int nWidth, int nHeight, int x, float y)
{
	int h1 = floor(y), h2 = h1 + 1;
	if ( h1 < 0 || h2 >= nHeight)
		return 0.f;
	return pPrj[x + h1 * nWidth] * (1 - y + h1) + pPrj[x + h2 * nWidth] * (y - h1);
}


void Convolute(float* pDst, float* pSrc, int nWidth, int nHeight, float delta_r, float w0, float R, float D, int nConvKernel)
{
	int m, n, i;
	float sum = 0.f;
	float theta_0 = acos(nHeight * delta_r / 2.f / R);
	float u0 = D / tan(theta_0);
	float delta_u = (2.f * u0) / nHeight;

	switch (nConvKernel)
	{
	case CONVOLUTE_KERNEL_COSINE:
#pragma omp parallel for private(m, n, i) reduction(+ : sum)
		for (m = 0; m < nHeight; ++m)
		{
			for (n = 0; n < nWidth; ++n)
			{
				sum = 0.f;
				for (i = 0; i < nHeight; ++i)
				{
					// 加权做卷积
					float u = -u0 + i * delta_u;
					float alpha = D / sqrt(D * D + u * u);
					sum += alpha * pSrc[n + i * nWidth] * CosineKernel(m - i, w0);
				}
				pDst[n + m * nWidth] = sum * delta_r;
			}
		}
		break;
	case CONVOLUTE_KERNEL_RL:
#pragma omp parallel for private(m, n, i) reduction(+ : sum)
		for (m = 0; m < nHeight; ++m)
		{
			for (n = 0; n < nWidth; ++n)
			{
				sum = 0.f;
				for (i = 0; i < nHeight; ++i)
				{
					// 加权做卷积
					float u = -u0 + i * delta_u;
					float alpha = D / sqrt(D * D + u * u);
					sum += alpha * pSrc[n + i * nWidth] * R_LKernel(m - i, delta_r);
				}
				pDst[n + m * nWidth] = sum * delta_r;
			}
		}
		break;
	case CONVOLUTE_KERNEL_SL:
#pragma omp parallel for private(m, n, i) reduction(+ : sum)
		for (m = 0; m < nHeight; ++m)
		{
			for (n = 0; n < nWidth; ++n)
			{
				sum = 0.f;
				for (i = 0; i < nHeight; ++i)
				{
					// 加权做卷积
					float u = -u0 + i * delta_u;
					float alpha = D / sqrt(D * D + u * u);
					sum += alpha * pSrc[n + i * nWidth] * S_LKernel(m - i, delta_r);
				}
				pDst[n + m * nWidth] = sum * delta_r;
			}
		}
		break;
	case CONVOLUTE_KERNEL_HAMMING:
#pragma omp parallel for private(m, n, i) reduction(+ : sum)
		for (m = 0; m < nHeight; ++m)
		{
			for (n = 0; n < nWidth; ++n)
			{
				sum = 0.f;
				for (i = 0; i < nHeight; ++i)
				{
					// 加权做卷积
					float u = -u0 + i * delta_u;
					float alpha = D / sqrt(D * D + u * u);
					sum += alpha * pSrc[n + i * nWidth] * HammingKernel(m - i, delta_r, w0);
				}
				pDst[n + m * nWidth] = sum * delta_r;
			}
		}
		break;
	default:
		break;
	}
}


void BackProject(float* pDst, float* pPrj, int nWidth, int nHeight, int nRays, int nAngles, float delta_r, float delta_fai, float R, float D)
{
	int m, n, i;
	int cx = (nWidth + 1) / 2;
	int cy = (nHeight + 1) / 2;
	int med = (nRays + 1) / 2;
	float sum = 0.f;

	// 2015.5.19 将三角函数放到循环体外面
	float *cos_fai = new float[nAngles];
	float *sin_fai = new float[nAngles];

#pragma omp parallel for
	for (i = 0; i < nAngles; ++i)
	{
		float fai = i * delta_fai;
		cos_fai[i] = cos(fai);
		sin_fai[i] = sin(fai);
	}

#pragma omp parallel for private(m, n, i) reduction(+ : sum)
	for (m = 0; m < nHeight; ++m)
	{
		for (n = 0; n < nWidth; ++n)
		{
			sum = 0.f;
			for (i = 0; i < nAngles; ++i)
			{
				float x1 = n - cx;
				float x2 = m - cy;
				float fai = i * delta_fai;
				float r = x1 * cos_fai[i] + x2 * sin_fai[i];
				float alpha = R * D / ((R - x2 * cos_fai[i] + x1 * sin_fai[i]) * (R - x2 * cos_fai[i] + x1 * sin_fai[i]));
				sum += alpha * LinearInterp(pPrj, nAngles, nRays, i, med + r * delta_r);
			}
			pDst[n + m * nWidth] = sum * delta_fai;
		}
	}

	SAFE_DELETE(cos_fai);
	SAFE_DELETE(sin_fai);
}


void SetColorTabFor8BitImage(CImage *pImage)
{
	RGBQUAD ColorTab[256];
	for(int i = 0; i < 256; i++)
	{
		ColorTab[i].rgbBlue = ColorTab[i].rgbGreen = ColorTab[i].rgbRed = i;
	}
	pImage->SetColorTable(0, 256, ColorTab);
}


bool Write2Raw(float* pSrc, int nWidth, int nHeight, CString path)
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

	if(fout)
	{
		switch (dlg.m_nRawHeader)
		{
		case -1://-1则保存成本程序自定义raw格式
			{
				// 可以看作文件头
				char* str = "# CTdemo";
				fout.write(str, sizeof(char) * strlen(str));
				// 将宽度、高度、数据以二进制写入RAW文件
				// 宽度、高度作为文件头，占用8字节
				fout.write((char*)&nWidth, sizeof(int));
				fout.write((char*)&nHeight, sizeof(int));
				fout.write((char*)pSrc, sizeof(float) * nWidth * nHeight);
			}
			break;
		case 0://保存成全裸数据（没有文件头）
			{
				fout.write((char*)pSrc, sizeof(float) * nWidth * nHeight);
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
				fout.write((char*)pSrc, sizeof(float) * nWidth * nHeight);
			}
			break;
		}

		fout.close();
		return true;
	}
	return false;
}


// 读取专有的RAW裸数据
bool ReadPropRaw(float* &pDst, int &nWidth, int &nHeight, CString path)
{
	std::ifstream fin(path, std::ios::binary);
	if (fin)
	{
		// 读取文件头
		char* str = "# CTdemo";
		int len = strlen(str);
		char* buf = new char[len];
		fin.read(buf, sizeof(char) * len);
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
		if (temp2 - temp1 != sizeof(float) * nWidth * nHeight)
		{
			fin.close();
			return false;
		}
		pDst = new float[nWidth * nHeight];
		fin.read((char*)pDst, sizeof(float) * nWidth * nHeight);
		fin.close();
		return true;
	}
	return false;
}


// 读取一般的裸数据
bool ReadRaw(float* &pDst, int &nWidth, int &nHeight, CString path)
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
		if (temp2 - temp1 - dlg.m_nRawHeader < sizeof(float) * nWidth * nHeight)
		{
			fin.close();
			return false;
		}
		fin.seekg(dlg.m_nRawHeader);
		pDst = new float[nWidth * nHeight];
		fin.read((char*)pDst, sizeof(float) * nWidth * nHeight);
		fin.close();
		return true;
	}
	return false;
}


// 将pSrc指针指向的数据写入文件[path]，width和height指定了宽与高.
bool Write2Txt(float* pSrc, int nWidth, int nHeight, CString path)
{
	ofstream InputFile(path);
	if(InputFile)
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


bool ReadTxt(float* &pDst, int &nWidth, int &nHeight, CString path)
{
	CString strText = _T("");
	CString szLine = _T("");
	CStdioFile file;
	if (file.Open(path, CFile::modeRead))
	{
		// 文件行数
		nHeight = 0;
		vector<float> TxtData;
		while(file.ReadString(szLine))
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


/*
float &k		直线斜率
float &c		直线截距
float y			已知y值
*/
float LineGetXValue(float &k, float &c, float y)
{
	ASSERT(k != 0);
	return (y - c) / k;
}


/*
float &k		直线斜率
float &c		直线截距
float x			已知x值
*/
float LineGetYValue(float &k, float &c, float x)
{
	return (k * x + c);
}


/*
float* pSrc		图像数据
int Xmin		左下角x坐标
int Ymin		左下角y坐标
int Xmax		右上角x坐标
int Ymax		右上角y坐标
int &Width		图像宽度
int &Height		图像高度
int &Rowlen		每行浮点数
int CurChannel	当前通道
int &Channel	通道总数
float &k		直线斜率
float &c		直接与y轴截距
*/
float LineIntegrate(float* pSrc, int &Width, int &Height, int &Rowlen, int &Channel, int CurChannel, int Xmin, int Ymin, int Xmax, int Ymax, float &k, float &c)
{
	vector <CIntSection> Sections;

	for (int i = Xmin; i <= Xmax; ++i)
	{
		float x = i;
		float y = LineGetYValue(k, c, x);
		if (y < Ymin || y > Ymax)
			continue;
		Sections.push_back(CIntSection(x, y));
	}

	for (int j = Ymin; j <= Ymax; ++j)
	{
		float y = j;
		float x = LineGetXValue(k, c, y);
		if (x < Xmin || x > Xmax)
			continue;
		Sections.push_back(CIntSection(x, y));
	}

	// 排序
	sort(Sections.begin(), Sections.end());

	float s = 0.f;
	int Num = Sections.size() - 1;
	for (int i = 0; i < Num; ++i)
	{
		int x = Sections[i].m_xPos - Xmin;
		int y = Sections[i].m_yPos - Ymin;
		if (0 <= x && x < Width && 0 <= y && y < Height)
			s += Distance(Sections[i], Sections[i + 1]) * pSrc[CurChannel + x * Channel + y * Rowlen];
	}
	return s;
}


/*
radon结果：float* pDst
输入图像信息：loat* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel
当前通道：int nCurChannel
角度间距及个数：float angles_separation, int nAnglesNum
射线间距及个数：float pixels_separation, int nRaysNum
*/
void ImageRadon(float* pDst, float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, int nCurChannel, 
				float angles_separation, int nAnglesNum, float pixels_separation, int nRaysNum)
{
	float zoom_rate = 1 / pixels_separation;
	int nNewWidth = zoom_rate * nWidth;
	int nNewHeight = zoom_rate * nHeight;
	int nNewRowlen = nNewWidth * nChannel;
	float *pSrc2 = ImageZoom(pSrc, nWidth, nHeight, nRowlen, nChannel, nNewWidth, nNewHeight);

	int nNewRaysNum = ComputeRaysNum(nNewWidth, nNewHeight);
	int nDetectorCenter = (nNewRaysNum + 1) / 2;
	float density = 1.f * nNewRaysNum / nRaysNum;

#pragma omp parallel for
	for (int i = 0; i < nAnglesNum; ++i)
	{
		float *temp = new float[nNewRaysNum];
		memset(temp, 0, nNewRaysNum * sizeof(float));
		float angle = i * angles_separation;
		ImageIntegrate(temp, pSrc2, nNewWidth, nNewHeight, nNewRowlen, nChannel, nCurChannel, angle, nNewRaysNum);
		
#pragma omp parallel for
		for (int j = 0; j < nRaysNum; ++j)
		{
			float x = j * density;
			pDst[i + j * nAnglesNum] = LinearInterp(temp, nNewRaysNum, x) * pixels_separation;
		}
		SAFE_DELETE(temp);
	}

	SAFE_DELETE(pSrc2);
}


// 沿单个方向线积分，单个方向的radon变换
/*
float* pDst		积分值
float* pSrc		图像数据
int &nWidth		图像宽度
int &nHeight	图像高度
int &nRowlen	每行浮点数
int &nChannel	通道个数
int nCurChannel	当前通道
float angle		方向
int &nLength	积分个数
*/
void ImageIntegrate(float* pDst, float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, int nCurChannel, float angle, int &nLength)
{
	// 图像旋转之后的信息及顶点坐标
	int NewWidth = 0, NewHeight = 0, NewRowlen = 0;
	int Xmin = 0, Ymin = 0, Xmax = 0, Ymax = 0;
	float *pBits = ImageRotate(pSrc, nWidth, nHeight, nRowlen, nChannel, angle, 0, 0, Xmin, Ymin, Xmax, Ymax, NewWidth, NewHeight, NewRowlen);
	// 图像按行累加
	float* add_width = new float[NewWidth];
	memset(add_width, 0, NewWidth * sizeof(float));
	// sum为了使OpenMP能正常编译
	float sum = 0.f;
	int nNewRowlen = NewWidth * nChannel;

#pragma omp parallel for reduction(+ : sum)
	for (int i = 0; i < NewWidth; ++i)
	{
		sum = 0.f;
		for (int j = 0; j < NewHeight; ++j)
		{
			sum += pBits[nCurChannel + i * nChannel + j * nNewRowlen];
		}
		add_width[i] = sum;
	}

	// 探测器中心和图像宽度的中心
	int nDetectorCenter = (nLength + 1) / 2;
	int nHalfWidth = (NewWidth + 1) / 2;

#pragma omp parallel for
	for (int i = 0; i < nLength; ++i)
	{
		int s = (i - nDetectorCenter) + nHalfWidth;
		if (s >= 0 && s < NewWidth)
			pDst[i] = add_width[s];
		//TRACE("序号 = %d, 值 = %f\n", i, pDst[i]);
	}

	SAFE_DELETE(add_width);
	SAFE_DELETE(pBits);
}


/*
输入图像信息：float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel
旋转角度：float angle
输出图像信息：int &NewWidth, int &NewHeight, int &NewRowlen
*/
float* ImageRotate(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, float angle, int &NewWidth, int &NewHeight, int &NewRowlen)
{
	// 原始图像四个顶点的坐标
	float x1, x2, x3, x4, y1, y2, y3, y4;
	x1 = 0;							y1 = 0;
	x2 = float(nWidth - 1);			y2 = 0;
	x3 = x2;						y3 = float(nHeight - 1);
	x4 = x1;						y4 = y3;
	// 2015.5.19 为了减少计算，把三角函数放到循环外面
	float cos_theta = cos(angle);
	float sin_theta = sin(angle);
	// 四个顶点顺时针旋转,绕图像左下角
	PositionTransform(x1, y1, cos_theta, sin_theta);
	PositionTransform(x2, y2, cos_theta, sin_theta);
	PositionTransform(x3, y3, cos_theta, sin_theta);
	PositionTransform(x4, y4, cos_theta, sin_theta);
	int Xmin, Xmax, Ymin, Ymax;
	Xmax = int(FindMaxBetween4Numbers(x1, x2, x3, x4));
	Ymax = int(FindMaxBetween4Numbers(y1, y2, y3, y4));
	Xmin = int(FindMinBetween4Numbers(x1, x2, x3, x4));
	Ymin = int(FindMinBetween4Numbers(y1, y2, y3, y4));
	// 新图像宽度、高度、每行字节数的变化
	NewWidth = Xmax - Xmin + 1;
	NewHeight = Ymax - Ymin + 1;
	NewRowlen = nChannel * NewWidth;
	float *pDst = new float[NewRowlen * NewHeight];
	memset(pDst, 0, NewRowlen * NewHeight * sizeof(float));

#pragma omp parallel for
	for (int i = 0; i < NewWidth; ++i)
	{
		for (int j = 0; j < NewHeight; ++j)
		{
			float x = float(i + Xmin);
			float y = float(j + Ymin);
			PositionTransform(x, y, cos_theta, -sin_theta);
			for (int nCurChannel = 0; nCurChannel < nChannel; ++nCurChannel)
				pDst[nCurChannel + i * nChannel + j * NewRowlen] = GetPositionValue(pSrc, nWidth, nHeight, nRowlen, nChannel, nCurChannel, x, y);
		}
	}

	return pDst;
}


float* ImageRotate(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, float angle, float x0, float y0, int &NewWidth, int &NewHeight, int &NewRowlen)
{
	// 原始图像四个顶点的坐标
	float x1, x2, x3, x4, y1, y2, y3, y4;
	x1 = 0;							y1 = 0;
	x2 = float(nWidth - 1);			y2 = 0;
	x3 = x2;						y3 = float(nHeight - 1);
	x4 = x1;						y4 = y3;
	// 2015.5.19 为了减少计算，把三角函数放到循环外面
	float cos_theta = cos(angle);
	float sin_theta = sin(angle);
	// 四个顶点顺时针旋转,绕图像中心
	PositionTransform(x1, y1, cos_theta, sin_theta, x0, y0);
	PositionTransform(x2, y2, cos_theta, sin_theta, x0, y0);
	PositionTransform(x3, y3, cos_theta, sin_theta, x0, y0);
	PositionTransform(x4, y4, cos_theta, sin_theta, x0, y0);
	int Xmin, Xmax, Ymin, Ymax;
	Xmax = int(FindMaxBetween4Numbers(x1, x2, x3, x4));
	Ymax = int(FindMaxBetween4Numbers(y1, y2, y3, y4));
	Xmin = int(FindMinBetween4Numbers(x1, x2, x3, x4));
	Ymin = int(FindMinBetween4Numbers(y1, y2, y3, y4));
	// 新图像宽度、高度、每行字节数的变化
	NewWidth = Xmax - Xmin + 1;
	NewHeight = Ymax - Ymin + 1;
	NewRowlen = nChannel * NewWidth;
	float *pDst = new float[NewRowlen * NewHeight];
	memset(pDst, 0, NewRowlen * NewHeight * sizeof(float));

#pragma omp parallel for
	for (int i = 0; i < NewWidth; ++i)
	{
		for (int j = 0; j < NewHeight; ++j)
		{
			float x = float(i + Xmin);
			float y = float(j + Ymin);
			PositionTransform(x, y, cos_theta, -sin_theta, x0, y0);
			for (int nCurChannel = 0; nCurChannel < nChannel; ++nCurChannel)
				pDst[nCurChannel + i * nChannel + j * NewRowlen] = GetPositionValue(pSrc, nWidth, nHeight, nRowlen, nChannel, nCurChannel, x, y);
		}
	}

	return pDst;
}


float* ImageRotate(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, float angle, float x0, float y0, int &Xmin, int &Ymin, int &Xmax, int &Ymax, int &NewWidth, int &NewHeight, int &NewRowlen)
{
	// 原始图像四个顶点的坐标
	float x1, x2, x3, x4, y1, y2, y3, y4;
	x1 = 0;							y1 = 0;
	x2 = float(nWidth - 1);			y2 = 0;
	x3 = x2;						y3 = float(nHeight - 1);
	x4 = x1;						y4 = y3;
	// 2015.5.19 为了减少计算，把三角函数放到循环外面
	float cos_theta = cos(angle);
	float sin_theta = sin(angle);
	// 四个顶点顺时针旋转,绕图像中心
	PositionTransform(x1, y1, cos_theta, sin_theta, x0, y0);
	PositionTransform(x2, y2, cos_theta, sin_theta, x0, y0);
	PositionTransform(x3, y3, cos_theta, sin_theta, x0, y0);
	PositionTransform(x4, y4, cos_theta, sin_theta, x0, y0);
	Xmax = int(FindMaxBetween4Numbers(x1, x2, x3, x4));
	Ymax = int(FindMaxBetween4Numbers(y1, y2, y3, y4));
	Xmin = int(FindMinBetween4Numbers(x1, x2, x3, x4));
	Ymin = int(FindMinBetween4Numbers(y1, y2, y3, y4));
	// 新图像宽度、高度、每行字节数的变化
	NewWidth = Xmax - Xmin + 1;
	NewHeight = Ymax - Ymin + 1;
	NewRowlen = nChannel * NewWidth;
	float *pDst = new float[NewRowlen * NewHeight];
	memset(pDst, 0, NewRowlen * NewHeight * sizeof(float));

#pragma omp parallel for
	for (int i = 0; i < NewWidth; ++i)
	{
		for (int j = 0; j < NewHeight; ++j)
		{
			float x = float(i + Xmin);
			float y = float(j + Ymin);
			PositionTransform(x, y, cos_theta, -sin_theta, x0, y0);
			for (int nCurChannel = 0; nCurChannel < nChannel; ++nCurChannel)
			{
				pDst[nCurChannel + i * nChannel + j * NewRowlen] = GetPositionValue(pSrc, nWidth, nHeight, nRowlen, nChannel, nCurChannel, x, y);
				//TRACE("列 = %d, 行 = %d, 值 = %f\n", i, j, pDst[nCurChannel + i * nChannel + j * NewRowlen]);
			}
		}
	}

	return pDst;
}


float* ImageZoom(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int &nChannel, int NewWidth, int NewHeight)
{
	int NewRowlen = nChannel * NewWidth;
	float* pDst = new float[NewRowlen * NewHeight];
	float wRatio = 1.f * nWidth / NewWidth;
	float hRatio = 1.f * nHeight / NewHeight;

#pragma omp parallel for
	for (int i = 0; i < NewWidth; ++i)
	{
		for (int j = 0; j < NewHeight; ++j)
		{
			for (int nCurChannel = 0; nCurChannel < nChannel; ++nCurChannel)
				pDst[nCurChannel + i * nChannel + j * NewRowlen] = GetPositionValue(pSrc, nWidth, nHeight, nRowlen, nChannel, nCurChannel, i * wRatio, j * hRatio);
		}
	}

	return pDst;
}


int ComputeRaysNum(int nWidth, int nHeight)
{
	// 参考MATLAB
	int temp1 = nWidth - (int)floor((nWidth - 1) / 2.f) - 1;
	int temp2 = nHeight - (int)floor((nHeight - 1) / 2.f) - 1;
	return 2 * (int)ceil(sqrt(1.f * temp1 * temp1 + temp2 * temp2)) + 3;
}


/*
函数返回			导数图像
float* pPrj			投影图像
int nRays			图像高度
int nAngles			图像宽度
float delta_r		间距
*/
float* DiffImage(float* pPrj, int nRays, int nAngles, float delta_r)
{
	float* pDst = new float[nRays * nAngles * sizeof(float)];
	for (int n = 0; n < nAngles; ++n)
	{
		pDst[n] =  pPrj[n] / delta_r;
	}

#pragma omp parallel for
	for (int m = 1; m < nRays; ++m)
	{
		for (int n = 0; n < nAngles; ++n)
		{
			pDst[n + m * nAngles] =  (pPrj[n + m * nAngles] - pPrj[n + (m - 1) * nAngles]) / delta_r;
		}
	}
	return pDst;
}


/*
float* pDst			目标
float* pPrj			投影图像
int nWidth			原始图像的宽
int nHeight			原始图像的高
int nRays			投影的行数(r)
int nAngles			投影的列数(fai)
float delta_r		扫描参数
float delta_fai		扫描参数
float theta			Hilbert角度
*/
void DBPImage(float* pDst, float* pPrj, int nWidth, int nHeight, int nRays, int nAngles, float delta_r, float delta_fai, float theta)
{
	int m, n, i;
	int cx = (nWidth + 1) / 2;
	int cy = (nHeight + 1) / 2;
	int med = (nRays + 1) / 2;
	float sum = 0.f;

	// 2015.5.19 将三角函数放到循环体外面
	float *cos_fai = new float[nAngles];
	float *sin_fai = new float[nAngles];
	float *sgn = new float[nAngles];

	// sin(fai - theta) = sin*cos - cos*sin
	float sin_theta = sin(theta);
	float cos_theta = cos(theta);

#pragma omp parallel for
	for (i = 0; i < nAngles; ++i)
	{
		float fai = i * delta_fai;
		cos_fai[i] = cos(fai);
		sin_fai[i] = sin(fai);
		float sin_fai_theta = sin_fai[i] * cos_theta - cos_fai[i] * sin_theta;
		if (sin_fai_theta > 0)
			sgn[i] = 1.f;
		else if (sin_fai_theta < 0)
			sgn[i] = -1.f;
		else
			sgn[i] = 0.f;
	}

	float* pDiff = DiffImage(pPrj, nRays, nAngles, delta_r);

#pragma omp parallel for private(m, n, i) reduction(+ : sum)
	for (m = 0; m < nHeight; ++m)
	{
		for (n = 0; n < nWidth; ++n)
		{
			sum = 0.f;
			for (i = 0; i < nAngles; ++i)
			{
				float fai = i * delta_fai;
				float r = (n - cx) * cos_fai[i] + (m - cy) * sin_fai[i];
				sum += sgn[i] * LinearInterp(pDiff, nAngles, nRays, i, med + r * delta_r);
			}
			pDst[n + m * nWidth] = sum * delta_fai;
		}
	}

	SAFE_DELETE(pDiff);
	SAFE_DELETE(cos_fai);
	SAFE_DELETE(sin_fai);
	SAFE_DELETE(sgn);
}


// 希尔伯特卷积核函数
float HilbertKernel(float x)
{
	if (x == 0)
		return 0;
	return 1 / (PI * x);
}


/*
float* pDst					目标
float* pSrc					指向DBP数据的指针
int nWidth					宽度，此处表示fai
int nHeight					高度，此处表示r
float delta_r				离散化之后的dr
*/
void InverseHilbert(float* pDst, float* pSrc, int nWidth, int nHeight, float delta_r)
{
	int s, t, s1;
	float sum = 0.f;

	float Lt = 0;
	float Ut = nHeight;
	float s0 = 0.f, Ct = 0.f;

#pragma omp parallel for private(t, s, s1) reduction(+ : sum, Ct)
	for (t = 0; t < nWidth; ++t)
	{
		for (s = 0; s < nHeight; ++s)
		{
			sum = 0.f;
			Ct = 0.f;
			for (s1 = Lt; s1 < Ut; ++s1)
			{
				sum += sqrt((s1 - Lt) * (Ut - s1)) * pSrc[t + s1 * nWidth] * HilbertKernel(s - s1);
				Ct += sqrt((s1 - Lt) * (Ut - s1)) * pSrc[t + s1 * nWidth] * HilbertKernel(s0 - s1);
			}
			float sqrt_result = sqrt((s - Lt) * (Ut - s));
			if (sqrt_result != 0)
				pDst[t + s * nWidth] = -1.f / sqrt_result * (sum - Ct) * delta_r;
		}
	}
}


// Load raw data from disk
void *loadRawFile(CString filename, size_t size)
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


vector<CString> ScanDiskFile(const CString strPath)
{
	// 支持文件格式列表
	CString Formats[SUPPORTED_FORMATS_NUM] = {_T("*.BMP"), _T("*.DIB"), _T("*.RLE"), 
		_T("*.JPG"), _T("*.JPEG"), _T("*.JPE"), _T("*.JFIF"), 
		_T("*.GIF"), _T("*.EMF"), _T("*.WMF"), _T("*.TIF"), _T("*.TIFF"), _T("*.PNG"), _T("*.ICO"),
		_T("*.TXT"), _T("*.RAW")};
	CFileFind find;
	vector<CString> vStrAllFiles;
	for (int i = 0; i < SUPPORTED_FORMATS_NUM; ++i)
	{
		ScanFormatFile(find, vStrAllFiles, strPath, Formats[i]);
	}
	find.Close();
	return vStrAllFiles;
}


// 从文件夹扫描某一类型的文件，存放到容器中
void ScanFormatFile(CFileFind &find, vector<CString> &vStrAllFiles, CString strPath, CString format)
{
	if (format == _T(""))
		return;
	BOOL IsFind = find.FindFile(strPath + _T("\\") + format);
	while (IsFind)
	{
		IsFind = find.FindNextFile();
		// 如果是"." 则不扫描
		if(find.IsDots())
			continue;
		// 文件
		else
			vStrAllFiles.push_back(find.GetFileName());
	}
}