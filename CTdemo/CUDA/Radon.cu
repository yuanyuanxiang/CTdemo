#include "radon.h"
#include "cublas.h"
#include "cublas_v2.h"

/* 
// ***************** radon �任��ʵ�� ************** //
   ********** �׶�ʦ����ѧ ������ʵ���� ********** //
// *************** Ԭ���飬2015��4�� *************** //
*/

#define CHECK_ERRORS(err, str) if(err != cudaSuccess) { str = cudaGetErrorString(err); cudaDeviceReset(); return str; }

#define CHECK_CUBLAS_ERRORS(err, str) if (err != CUBLAS_STATUS_SUCCESS) { str = "CUBLAS ��������"; cudaDeviceReset(); return str; }

/* ��ʼ������vec = [val, val, val, ...]
float*	vec:����ָ��
int		len:��������
float	val:������ֵ
ע�⣺������һά��block
*/
__global__ void cudaVectorAssigned(float* vec, int len, float val)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i < len)
		vec[i] = val;
}


// ����һ���������ݵ�����ĳһ��
/*
float *d_pDst		����
int RaysNum			������
int AnglesNum		������
float *d_pVec		����
int ImageDiag		ͼ��Խ���/��������
int column			����������
int HalfRays		������һ��
int HalfDiag		��������һ��
float Separation	���߼��
ע�⣺������һά��block
*/
__global__ void cudaCopyColumnData(float *d_pDst, int RaysNum, int AnglesNum, float *d_pVec, int ImageDiag, int column, int HalfRays, int HalfDiag, float Separation)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	int num = HalfDiag + (i - HalfRays) * Separation;
	if(i < RaysNum && 0 <= num && num < ImageDiag)
	{
		d_pDst[column + i * AnglesNum] = d_pVec[num];
	}
}


// �������ݣ�d_pDst[int ((i - nHalfDiag) / fPixelDistance + nHalfWidth)] = d_pSrc[temp];
__global__ void cudaCopyTempData(float *d_pDst, float *d_pSrc, int Width, int nImageDiag, int nHalfDiag, int nHalfWidth, float fPixelDistance)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	if( i < nImageDiag )
	{
		int temp = int ((i - nHalfDiag) / fPixelDistance + nHalfWidth);
		if (temp >= 0 && temp < Width)
			d_pDst[i] = d_pSrc[temp] * fPixelDistance;
	}
}


// radon�任�ĺ˺���
__global__ void cudaRotateKernel(float* d_pDst, float* d_pSrc, int Width, int Height, int Rowlen, int NewWidth, int NewHeight, int Xmin, int Ymin, float cos_theta, float sin_theta)
{
	/* ����������forѭ����д��CUDA�ں����ԡ�
	for (int i = 0; i < Width; ++i)
	{
		for (int j = 0; j < Height; ++j)
		{
			float x = float(i + Xmin);
			float y = float(j + Ymin);
			PositonTransform(x, y, -angle);
			*(pDst + i + j * NewRowlen) = (BYTE)GetPositionPixel(x, y, pSrc, Width, Height, Rowlen);
		}
	}
	*/
	int i = blockIdx.x * blockDim.x + threadIdx.x; //i��
	int j = blockIdx.y * blockDim.y + threadIdx.y; //j��
	if (i < NewHeight && j < NewWidth)
	{
		float x = float(j + Xmin);
		float y = float(i + Ymin);
		cudaPositonTransform(x, y, cos_theta, -sin_theta);
		*(d_pDst + j + i * NewWidth) = cudaGetPositionValue(x, y, d_pSrc, Width, Height);
	}
}


// �������ã�����ͶӰͼ��
/*
float* h_pDst				ͶӰͼ��
int RaysNum					ÿ�Ƕ�������
int AnglesNum				�����Ƕȸ���
 ***��������������2015.4.17����***
float rays_separation		���߼��
float angle_separation		�Ƕȼ��
BYTE* h_pSrc				ԭʼͼ��
int Width					ԭͼ���
int Height					ԭͼ�߶�
int Rowlen					ԭͼ���ֽ���
*/
__host__ const char* cudaRadon(float* h_pDst, int RaysNum, int AnglesNum, float rays_separation, float angle_separation, BYTE* h_pSrc, int Width, int Height, int Rowlen, float fSubPixel)
{
	// cuda ����
	cudaError_t error;
	const char* errstr = 0;
	cudaDeviceProp deviceProp;
	int devID;
	error = cudaGetDeviceCount(&devID);
	CHECK_ERRORS(error, errstr);
	// pick the device with highest Gflops/s
	devID = gpuGetMaxGflopsDeviceId();
	error = cudaSetDevice(devID);
	CHECK_ERRORS(error, errstr);
	error = cudaGetDeviceProperties(&deviceProp, devID);
	CHECK_ERRORS(error, errstr);
	// ����������� < 1.1
	if (deviceProp.major < 1) return errstr;
	else if (deviceProp.major == 1 && deviceProp.minor < 1) return errstr;

	// �豸������
	BYTE *d_pSrc = NULL;
	float *d_pDst = NULL;
	// ��������
	error = cudaMalloc((void **)&d_pSrc, Height * Rowlen * sizeof(BYTE));
	CHECK_ERRORS(error, errstr);
	error = cudaMemcpy(d_pSrc, h_pSrc, Height * Rowlen * sizeof(BYTE), cudaMemcpyHostToDevice);
	CHECK_ERRORS(error, errstr);
	error = cudaMalloc((void **)&d_pDst, RaysNum * AnglesNum * sizeof(float));
	CHECK_ERRORS(error, errstr);
	error = cudaMemcpy(d_pDst, h_pDst, RaysNum * AnglesNum * sizeof(float), cudaMemcpyHostToDevice);
	CHECK_ERRORS(error, errstr);
	// ԭʼͼ��Խ��߳���
	int ImageDiag = (int)sqrt(1.0f * Width * Width + Height * Height);
	float *d_pTemp = NULL,	//�豸���м����ݣ�����ImageDiag
		*d_pOnes = NULL,	//�豸�˵�λ����������NewHeight
		*d_pTemp2 = NULL;

	error = cudaMalloc((void **)&d_pTemp, ImageDiag * sizeof(float));
	CHECK_ERRORS(error, errstr);
	
	// ������cublas�����Ĳ���
	float fAlpha = 1.0f, fBeta  = 0.0f;
	cublasHandle_t cubHandle;
	cublasStatus_t cubStatus;
	cubStatus = cublasInit();
	CHECK_CUBLAS_ERRORS(cubStatus, errstr);
	cubStatus = cublasCreate(&cubHandle);
	CHECK_CUBLAS_ERRORS(cubStatus, errstr);
	cublasOperation_t cubTrans = CUBLAS_OP_N;
	// �ȷŴ�ͼ��
	float *d_pZoom = NULL;
	// �Ŵ���
	float ZoomRate = 1.f / fSubPixel;
	d_pZoom = cudaImageZoom(d_pZoom, d_pSrc, ZoomRate * Width, ZoomRate * Height, Width, Height, Rowlen);
	Width *= ZoomRate;
	Height *= ZoomRate;
	Rowlen = Width;
	// ��ĳ�������߻��֣�index��ʾ�ڼ����Ƕ�
	for (int index = 0; index < AnglesNum; ++index)
	{
		// ԭʼͼ���ĸ����������
		float x1, x2, x3, x4, y1, y2, y3, y4;
		x1 = 0;							y1 = 0;
		x2 = float(Width - 1);			y2 = 0;
		x3 = x2;						y3 = float(Height - 1);
		x4 = x1;						y4 = y3;
		// �ĸ�����˳ʱ����ת
		float angle = index * angle_separation;
		// 2015.5.19 Ϊ�˼��ټ��㣬�����Ǻ����ŵ�ѭ������
		float cos_theta = cos(angle);
		float sin_theta = sin(angle);
		PositionTransform(x1, y1, cos_theta, sin_theta);
		PositionTransform(x2, y2, cos_theta, sin_theta);
		PositionTransform(x3, y3, cos_theta, sin_theta);
		PositionTransform(x4, y4, cos_theta, sin_theta);
		int Xmin, Xmax, Ymin, Ymax;
		Xmin = (int)floor(FindMinBetween4Numbers(x1, x2, x3, x4));
		Xmax = (int)floor(FindMaxBetween4Numbers(x1, x2, x3, x4));
		Ymin = (int)floor(FindMinBetween4Numbers(y1, y2, y3, y4));
		Ymax = (int)floor(FindMaxBetween4Numbers(y1, y2, y3, y4));
		// ��ͼ���ȡ��߶ȵı仯
		int NewWidth = Xmax - Xmin + 1;
		int NewHeight = Ymax - Ymin + 1;
		// �����ڴ棬������NewWidth * NewHeight
		float *d_pRotatedBits = NULL;
		error = cudaMalloc((void **)&d_pRotatedBits, NewHeight * NewWidth * sizeof(float));
		CHECK_ERRORS(error, errstr);
		error = cudaMemset(d_pRotatedBits, 0, NewHeight * NewWidth * sizeof(float));
		CHECK_ERRORS(error, errstr);

		// ********** Kernel **********
		dim3 dimBlock(16, 16);
		dim3 dimGrid((NewHeight + dimBlock.x - 1) / dimBlock.x, (NewWidth + dimBlock.y - 1) / dimBlock.y);
		cudaRotateKernel<<<dimGrid, dimBlock>>>(d_pRotatedBits, d_pZoom, Width, Height, Rowlen, NewWidth, NewHeight, Xmin, Ymin, cos_theta, sin_theta);
		error = cudaGetLastError();
		CHECK_ERRORS(error, errstr);

		//ͼ�����ۼ�
		error = cudaMalloc((void **)&d_pOnes, NewHeight * sizeof(float));
		CHECK_ERRORS(error, errstr);
		error = cudaMalloc((void **)&d_pTemp2, NewWidth * sizeof(float));
		CHECK_ERRORS(error, errstr);
		//�˺�����<<<1, N>>>�߳����ޣ�ֻ�ܴ���512��С.�������ÿ��blockʹ��ThreadsNum���̣߳�������grid
		int ThreadsNum = 256;
		cudaVectorAssigned<<<(NewHeight + ThreadsNum - 1) / ThreadsNum, ThreadsNum>>>(d_pOnes, NewHeight, 1.f);
		error = cudaGetLastError();
		CHECK_ERRORS(error, errstr);
		cudaVectorAssigned<<<(ImageDiag + ThreadsNum - 1) / ThreadsNum, ThreadsNum>>>(d_pTemp, ImageDiag, 0.f);
		error = cudaGetLastError();
		CHECK_ERRORS(error, errstr);
		cudaVectorAssigned<<<(NewWidth + ThreadsNum - 1) / ThreadsNum, ThreadsNum>>>(d_pTemp2, NewWidth, 0.f);
		error = cudaGetLastError();
		CHECK_ERRORS(error, errstr);
		cubStatus = cublasSgemv(cubHandle, cubTrans, NewWidth, NewHeight, &fAlpha, d_pRotatedBits, NewWidth, d_pOnes, 1, &fBeta, d_pTemp2, 1);
		CHECK_CUBLAS_ERRORS(cubStatus, errstr);
		int HalfRays = (RaysNum + 1) / 2, HalfDiag = (ImageDiag + 1) / 2, HalfWidth = (NewWidth + 1) / 2;
		cudaCopyTempData<<<(NewWidth + ThreadsNum - 1) / ThreadsNum, ThreadsNum>>>(d_pTemp, d_pTemp2, NewWidth, ImageDiag, HalfDiag, HalfWidth, fSubPixel);
		error = cudaGetLastError();
		CHECK_ERRORS(error, errstr);
		cudaFree(d_pOnes);
		cudaFree(d_pTemp2);
		cudaFree(d_pRotatedBits);

		cudaCopyColumnData<<<(RaysNum + ThreadsNum - 1) / ThreadsNum, ThreadsNum>>>(d_pDst, RaysNum, AnglesNum, d_pTemp, ImageDiag, index, HalfRays, HalfDiag, rays_separation);
		error = cudaGetLastError();
		CHECK_ERRORS(error, errstr);
	}
	error = cudaMemcpy(h_pDst, d_pDst, RaysNum * AnglesNum * sizeof(float), cudaMemcpyDeviceToHost);
	CHECK_ERRORS(error, errstr);
	cudaFree(d_pTemp);
	cudaFree(d_pSrc);
	cudaFree(d_pDst);
	cublasDestroy(cubHandle);
	cublasShutdown();
	cudaDeviceReset();
	return errstr;
}


// ͼ��Ŵ�ĺ˺���
__global__ void cudaZoomKernel(float* d_pDst, BYTE* d_pSrc, float wRatio, float hRatio, int NewWidth, int NewHeight, int Width, int Height, int Rowlen)
{
	int j = blockIdx.x * blockDim.x + threadIdx.x; //j��
	int i = blockIdx.y * blockDim.y + threadIdx.y; //i��
	if ( i < NewWidth && j < NewHeight)
		d_pDst[i + j * NewWidth] = cudaGetPositionPixel(i * wRatio, j * wRatio, d_pSrc, Width, Height, Rowlen);
}


// ��ͼ����зŴ�
/*
float* pDst		Ŀ������
BYTE* pSrc		ԭͼ����
int NewWidth	Ŀ����
int NewHeight	Ŀ��߶�
int Width		ԭͼ���
int Height		ԭͼ�߶�
int Rowlen		ԭͼÿ���ֽ���
 ** ע�� ** ���뽫d_pDst��Ϊ������ء�
*/
__host__ float* cudaImageZoom(float* d_pDst, BYTE* d_pSrc, int NewWidth, int NewHeight, int Width, int Height, int Rowlen)
{
	// ���Ŀ��ǿգ����ؿա�
	if (d_pDst != NULL)
		return NULL;
	// ��������ڴ�ʧ�ܣ����ؿ�
	if(cudaMalloc((void **)&d_pDst, NewWidth * NewHeight * sizeof(float)))
		return NULL;
	// ͼ���ȡ��߶����ű���
	float wRatio = 1.f * Width / NewWidth;
	float hRatio = 1.f * Height / NewHeight;
	// ���ú˺���
	dim3 dimBlock(16, 16);
	dim3 dimGrid((NewHeight + dimBlock.x - 1) / dimBlock.x, (NewWidth + dimBlock.y - 1) / dimBlock.y);
	cudaZoomKernel<<<dimGrid, dimBlock>>>(d_pDst, d_pSrc, wRatio, hRatio, NewWidth, NewHeight, Width, Height, Rowlen);
	cudaError_t error = cudaGetLastError();

	return d_pDst;
}


// ��ȡͼ�����x�У�y�е�����ֵ��
__device__ float cudaGetImagePixel(int x, int y, BYTE* pSrc, int Width, int Height, int Rowlen)
{
	if (x < 0 || x >= Width || y < 0 || y >= Height)
		return 0;
	return (float)*(pSrc + x + y * Rowlen);
}


// ��ȡ����(x, y)��������ֵ������˫���Բ�ֵ��
__device__ float cudaGetPositionPixel(float x, float y, BYTE* pSrc, int Width, int Height, int Rowlen)
{
	int x1, x2, x3, x4, y1, y2, y3, y4;
	float Ans1, Ans2;
	x1 = floor(x);	y1 = floor(y);
	x2 = x1 + 1;	y2 = y1;
	x3 = x2;		y3 = y1 + 1;
	x4 = x1;		y4 = y3;
	Ans1 = cudaGetImagePixel(x1, y1, pSrc, Width, Height, Rowlen) * (1 - x + x1) 
		+ cudaGetImagePixel(x2, y2, pSrc, Width, Height, Rowlen) * (x - x1);
	Ans2 = cudaGetImagePixel(x4, y4, pSrc, Width, Height, Rowlen) * (1 - x + x4) 
		+ cudaGetImagePixel(x3, y3, pSrc, Width, Height, Rowlen) * (x - x4);
	return (Ans1 * (1 - y + y1) + Ans2 * (y - y1));
}


// ��ȡ����x�У�y�е�����ֵ��
__device__ float cudaGetIndexValue(int x, int y, float* pSrc, int Width, int Height)
{
	if (x < 0 || x >= Width || y < 0 || y >= Height)
		return 0;
	return (float)*(pSrc + x + y * Width);
}


// ��ȡ����(x, y)��������ֵ������˫���Բ�ֵ��
__device__ float cudaGetPositionValue(float x, float y, float* pSrc, int Width, int Height)
{
	int x1, x2, x3, x4, y1, y2, y3, y4;
	float Ans1, Ans2;
	x1 = floor(x);	y1 = floor(y);
	x2 = x1 + 1;	y2 = y1;
	x3 = x2;		y3 = y1 + 1;
	x4 = x1;		y4 = y3;
	Ans1 = cudaGetIndexValue(x1, y1, pSrc, Width, Height) * (1 - x + x1) + cudaGetIndexValue(x2, y2, pSrc, Width, Height) * (x - x1);
	Ans2 = cudaGetIndexValue(x4, y4, pSrc, Width, Height) * (1 - x + x4) + cudaGetIndexValue(x3, y3, pSrc, Width, Height) * (x - x4);
	return (Ans1 * (1 - y + y1) + Ans2 * (y - y1));
}


// �豸�ˣ�������(x, y)��תangle�Ƕȡ�
__device__ void cudaPositonTransform(float &x, float &y, float theta)
{
	float cos_theta = cos(theta);
	float sin_theta = sin(theta);
	float x_temp = x * cos_theta - y * sin_theta;
	y = x * sin_theta + y * cos_theta;
	x = x_temp;
}


// �豸�ˣ������޸�Ϊ�Ƕȵ����Һ����ҡ�
__device__ void cudaPositonTransform(float &x, float &y, float cos_theta, float sin_theta)
{
	float x_temp = x * cos_theta - y * sin_theta;
	y = x * sin_theta + y * cos_theta;
	x = x_temp;
}