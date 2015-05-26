
#include "PanRadon.h"
#include "vector_types.h"
#include "vector_functions.h"

#define CHECK_ERRORS(err, str) if(err != cudaSuccess) { str = cudaGetErrorString(err); cudaDeviceReset(); return str; }

#define CHECK_CUBLAS_ERRORS(err, str) if (err != CUBLAS_STATUS_SUCCESS) { str = "CUBLAS 遇到错误。"; cudaDeviceReset(); return str; }


// 核函数参数如果是变量的引用，可能会出现问题
__global__ void Kernel_cudaIntsections(float R, float D, float pan_u0, float pan_delta_u, float center_y, 
									   float2* pDst, float2* temp, float* pPrj, int pan_rays, int pan_angles, int angles_index, 
									   float* pSrc, int Xmin, int Ymin, int Xmax, int Ymax, int Width, int Height, int intsection_length)
{
	int j = blockIdx.x * blockDim.x + threadIdx.x;
	if (j < pan_rays)
	{
		float u = -pan_u0 + j * pan_delta_u;
		float k = -u / D;
		float c = -R * k + center_y;
		cudaIntsections(pDst, temp, pPrj, pan_rays, pan_angles, j, angles_index, 
			pSrc, Xmin, Ymin, Xmax, Ymax, Width, Height, intsection_length, k, c);
	}
}


__global__ void Kernel_cudaValuesCopy(float* pDst, float* pSrc, int Width, int Height, float cos_theta, float sin_theta, 
						float m_Ox, float m_Oy, int Xmin, int Ymin, int NewWidth, int NewHeight)
{
	int j = blockIdx.x * blockDim.x + threadIdx.x; //j行
	int i = blockIdx.y * blockDim.y + threadIdx.y; //i列
	if (i < NewWidth && j < NewHeight)
	{
		float x = float(i + Xmin);
		float y = float(j + Ymin);
		_cudaPositionTransform(x, y, cos_theta, -sin_theta, m_Ox, m_Oy);
		*(pDst + i + j * NewWidth) = cudaBiLinearInterp(pSrc, Height, Width, x, y);
	}
}


/*
float* pSrc				//图像数据
int src_width			//图像宽度
int src_height			//图像高度
float* pDst				//投影数据
int pan_angles			//投影宽度
int pan_rays			//投影高度
float scan_range		//采样范围
float R					//SO
float D					//SO'
*/
__host__ const char* cudaPanRadon(float* h_pSrc, int src_width, int src_height, float* h_pDst, int pan_angles, int pan_rays, float scan_range, float R, float D)
{
	// 错误及错误信息
	cudaError_t error;
	const char* str = 0;

	cudaDeviceProp deviceProp;
	int devID;
	error = cudaGetDeviceCount(&devID);
	CHECK_ERRORS(error, str);
	// pick the device with highest Gflops/s
	devID = gpuGetMaxGflopsDeviceId();
	error = cudaSetDevice(devID);
	CHECK_ERRORS(error, str);
	error = cudaGetDeviceProperties(&deviceProp, devID);
	CHECK_ERRORS(error, str);
	// 如果计算能力 < 1.1
	if (deviceProp.major < 1) return "计算能力小于1.0.";
	else if (deviceProp.major == 1 && deviceProp.minor < 1) return "计算能力小于1.1.";

	// 拷贝内存
	float *d_pSrc = NULL, *d_pDst = NULL;
	error = cudaMalloc((void**)&d_pSrc, src_width * src_height * sizeof(float));
	CHECK_ERRORS(error, str);
	error = cudaMemcpy(d_pSrc, h_pSrc, src_width * src_height * sizeof(float), cudaMemcpyHostToDevice);
	CHECK_ERRORS(error, str);
	error = cudaMalloc((void**)&d_pDst, pan_angles * pan_rays * sizeof(float));
	CHECK_ERRORS(error, str);
	error = cudaMemset(d_pDst, 0, pan_angles * pan_rays * sizeof(float));
	CHECK_ERRORS(error, str);

	float diagonal = sqrt(1.f * src_width * src_width + src_height * src_height);
	float theta_0 = acos(diagonal / 2.f / R);
	float pan_u0 = D / tan(theta_0);
	float pan_delta_u = (2.f * pan_u0) / pan_rays;
	float angles_separation = scan_range / pan_angles;
	float center_y = (src_height + 1) / 2.f;
	for (int i = 0; i < pan_angles; ++i)
	{
		int Xmin, Ymin, Xmax, Ymax, NewWidth, NewHeight;
		float theta = i * angles_separation;
		float cos_theta = cos(theta);
		float sin_theta = sin(theta);
		float *d_pCur = NULL;
		d_pCur = cudaComputeRotatedValues(d_pSrc, src_width, src_height, cos_theta, sin_theta, Xmin, Ymin, Xmax, Ymax, NewWidth, NewHeight);
		int intsection_length = NewWidth + NewHeight;
		float2 *IntSections = NULL, *temp = NULL;
		error = cudaMalloc((void**)&IntSections, pan_rays * intsection_length * sizeof(float2));
		error = cudaMemset(IntSections, 0, pan_rays * intsection_length * sizeof(float2));
		error = cudaMalloc((void**)&temp, pan_rays * intsection_length * sizeof(float2));
		error = cudaMemset(temp, 0, pan_rays * intsection_length * sizeof(float2));
		
		int ThreadsNum = 256;
		Kernel_cudaIntsections<<<(pan_rays + ThreadsNum - 1) / ThreadsNum, ThreadsNum>>>(R, D, pan_u0, pan_delta_u, center_y, 
			IntSections, temp, d_pDst, pan_rays, pan_angles, i, d_pCur, Xmin, Ymin, Xmax, Ymax, NewWidth, NewHeight, intsection_length);
		error = cudaGetLastError();
		CHECK_ERRORS(error, str);

		error = cudaFree(temp);
		error = cudaFree(IntSections);
		error = cudaFree(d_pCur);
	}
	error = cudaMemcpy(h_pSrc, d_pSrc, src_width * src_height * sizeof(float), cudaMemcpyDeviceToHost);
	CHECK_ERRORS(error, str);
	error = cudaMemcpy(h_pDst, d_pDst, pan_angles * pan_rays * sizeof(float), cudaMemcpyDeviceToHost);
	CHECK_ERRORS(error, str);

	error = cudaFree(d_pSrc);
	error = cudaFree(d_pDst);
	return str;
}


float* cudaComputeRotatedValues(float *pSrc, int Width, int Height, float cos_theta, float sin_theta, int &Xmin, int &Ymin, int &Xmax, int &Ymax, int &NewWidth, int &NewHeight) 
{
	// 原始图像四个顶点的坐标
	float x1, x2, x3, x4, y1, y2, y3, y4;
	x1 = 0;							y1 = 0;
	x2 = float(Width - 1);			y2 = 0;
	x3 = x2;						y3 = float(Height - 1);
	x4 = x1;						y4 = y3;
	// 四个顶点顺时针旋转,绕坐标原点
	float center_x = (Width + 1) / 2.f;
	float center_y = (Height + 1) / 2.f;
	PositionTransform(x1, y1, cos_theta, sin_theta, center_x, center_y);
	PositionTransform(x2, y2, cos_theta, sin_theta, center_x, center_y);
	PositionTransform(x3, y3, cos_theta, sin_theta, center_x, center_y);
	PositionTransform(x4, y4, cos_theta, sin_theta, center_x, center_y);
	Xmax = int(FindMaxBetween4Numbers(x1, x2, x3, x4));
	Ymax = int(FindMaxBetween4Numbers(y1, y2, y3, y4));
	Xmin = int(FindMinBetween4Numbers(y1, y2, y3, y4));
	Ymin = int(FindMinBetween4Numbers(y1, y2, y3, y4));
	// 新图像宽度、高度、每行字节数的变化
	NewWidth = Xmax - Xmin + 1;
	NewHeight = Ymax - Ymin + 1;
	float *pDst = NULL;
	cudaMalloc((void**)&pDst, NewWidth * NewHeight * sizeof(float));
	cudaMemset(pDst, 0, NewWidth * NewHeight * sizeof(float));
	
	dim3 dimBlock(16, 16);
	dim3 dimGrid((NewHeight + dimBlock.x - 1) / dimBlock.x, (NewWidth + dimBlock.y - 1) / dimBlock.y);
	Kernel_cudaValuesCopy<<<dimGrid, dimBlock>>>(pDst, pSrc, Width, Height, cos_theta, sin_theta, center_x, center_y, Xmin, Ymin, NewWidth, NewHeight);
	
	return pDst;
}


__device__ float cudaGetPositionValue(float *pSrc, int row, int col, int x, int y)
{
	if (x < 0 || x >= col || y < 0 || y >= row)
		return 0;
	return *(pSrc + x + y * col);
}


__device__ float cudaBiLinearInterp(float *pSrc, int row, int col, float x, float y)
{
	int x1, x2, x3, x4, y1, y2, y3, y4;
	float Ans1, Ans2;
	x1 = floor(x);	y1 = floor(y);
	x2 = x1 + 1;	y2 = y1;
	x3 = x2;		y3 = y1 + 1;
	x4 = x1;		y4 = y3;
	Ans1 = cudaGetPositionValue(pSrc, row, col, x1, y1) * (1 - x + x1) + cudaGetPositionValue(pSrc, row, col, x2, y2) * (x - x1);
	Ans2 = cudaGetPositionValue(pSrc, row, col, x4, y4) * (1 - x + x4) + cudaGetPositionValue(pSrc, row, col, x3, y3) * (x - x4);
	return (Ans1 * (1 - y + y1) + Ans2 * (y - y1));
}


__device__ __host__ float cudaLineGetXValue(float &k, float &c, float y)
{
	return (y - c) / k;
}


__device__ __host__ float cudaLineGetYValue(float &k, float &c, float x)
{
	return (k * x + c);
}


/*
float2* pDst			交线数据
float2* temp			中间数据
int Xmin				左下角x坐标
int Ymin				左下角y坐标
int Xmax				右上角x坐标
int Ymax				右上角y坐标
int intsection_length	相交像素数
int line_index			交线编号
float &k				直线斜率
float &c				直接与y轴截距
*/
__device__ __host__ void cudaIntsections(float2* pDst, float2* temp, float* pPrj, int pan_rays, int pan_angles, int line_index, int angles_index, 
								float* pSrc, int Xmin, int Ymin, int Xmax, int Ymax, int Width, int Height, int intsection_length, float &k, float &c) 
{
	float2* dst_pos = pDst + line_index * intsection_length;
	float2* temp_pos = temp + line_index * intsection_length;
	int n = 0, n1, n2, s = 0;
	for (int i = Xmin; i <= Xmax; ++i)
	{
		float x = i;
		float y = cudaLineGetYValue(k, c, x);
		// 因为直线是单调函数，所以当y超出图像了就break
		if (y < Ymin || y > Ymax)
			break;
		temp_pos[n++] = make_float2(x, y);
	}
	n1 = n;
	if (k >= 0)
	{
		for (int j = int(c); j <= Ymax; ++j)
		{
			float y = j;
			float x = cudaLineGetXValue(k, c, y);
			// 这里不能直接break，画个草图可以看出
			if (x < Xmin || x > Xmax)
				continue;
			temp_pos[n++] = make_float2(x, y);
		}
	}
	else
	{
		for (int j = int(c); j >= Ymin; --j)
		{
			float y = j;
			float x = cudaLineGetXValue(k, c, y);
			// 这里不能直接break，画个草图可以看出
			if (x < Xmin || x > Xmax)
				continue;
			temp_pos[n++] = make_float2(x, y);
		}
	}
	n2 = n - n1;
	// 对两个有序数组进行排序
	if (n1 >= n2)
	{
		for (int i = 0; i < n2; ++i)
		{
			if (temp_pos[i].x < temp_pos[n1 + i].x)
			{
				dst_pos[s++] = temp_pos[i];
			}
			else
			{
				dst_pos[s++] = temp_pos[n1 + i];
			}
		}
		for (int i = n2; i < n1; ++i)
		{
			dst_pos[s++] = temp_pos[i];
		}
	}
	else
	{
		for (int i = 0; i < n1; ++i)
		{
			if (temp_pos[i].x < temp_pos[n1 + i].x)
			{
				dst_pos[s++] = temp_pos[i];
			}
			else
			{
				dst_pos[s++] = temp_pos[n1 + i];
			}
		}
		for (int i = n1; i < n2; ++i)
		{
			dst_pos[s++] = temp_pos[n1 + i];
		}
	}
	
	float sum = 0.f;
	for (int i = 0; i < s - 1; ++i)
	{
		int x = dst_pos[i].x - Xmin;
		int y = dst_pos[i].y - Ymin;
		if (0 <= x && x < Width && 0 <= y && y < Height)
			sum += cudaDistance(dst_pos[i], dst_pos[i + 1]) * pSrc[x + y * Width];
	}
	pPrj[angles_index + line_index * pan_angles] = sum;
}


/*
float2* Intsections			交线数据
float *pDst					投影数据
float *pSrc					原始图像
int pan_angles				采样角度数
int intsection_length		相交像素数
int angles_index			当前角度
int line_index				交线编号
int Xmin, int Ymin			图像左下角坐标
int Width, int Height		图像宽度与高度
*/
__device__ void cudaIntergrate(float2* Intsections, float *pDst, float *pSrc, int pan_angles, int intsection_length, int angles_index, int line_index, int Xmin, int Ymin, int Width, int Height)
{
	float2* Pos = Intsections + line_index * intsection_length;
	float s = 0.f;
	for (int i = 0; i < intsection_length - 1; ++i)
	{
		int x = Pos[i].x - Xmin;
		int y = Pos[i].y - Ymin;
		if (0 <= x && x < Width && 0 <= y && y < Height)
			s += cudaDistance(Pos[i], Pos[i + 1]) * pSrc[x + y * Width];
	}
	pDst[angles_index + line_index * pan_angles] = s;
}


__device__ __host__ float cudaDistance(const float2 &a, const float2 &b)
{
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}


/* 坐标绕原点旋转变换.*/
__device__ void _cudaPositionTransform(float &x, float &y, float cos_theta, float sin_theta)
{
	float x_temp = x * cos_theta - y * sin_theta;
	y = x * sin_theta + y * cos_theta;
	x = x_temp;
}


/* 坐标绕固定点旋转变换.*/
__device__ void _cudaPositionTransform(float &x, float &y, float cos_theta, float sin_theta, float x0, float y0)
{
	float delta_x = x - x0;
	float delta_y = y - y0;
	_cudaPositionTransform(delta_x, delta_y, cos_theta, sin_theta);
	x = x0 + delta_x;
	y = y0 + delta_y;
}