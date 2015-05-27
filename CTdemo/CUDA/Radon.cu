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
int AnglesNum		��������
float *d_pVec		����
int nNewRaysNum		��������
float alpha			���ű���
int column			��ǰ��
float Separation	���ؼ��
ע�⣺������һά��block
*/
__global__ void cudaCopyColumnData(float *d_pDst, int AnglesNum, float *d_pVec, int nNewRaysNum, float alpha, int column, float Separation)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	{
		float x = i * alpha;
		d_pDst[column + i * AnglesNum] = cudaLinearInterp(d_pVec, nNewRaysNum, x) * Separation;
	}
}


// ���Բ�ֵ
__device__ float cudaLinearInterp(float* pPrj, int nWidth, float x)
{
	int x1 = floor(x), x2 = x1 + 1;
	if ( x1 < 0 || x2 >= nWidth)
		return 0;
	return pPrj[x1] * (1 - x + x1) + pPrj[x2] * (x - x1);
}


// ��������,��ͼ�����ۼӵ�ֵ������d_pDst
/*
float *d_pDst			Ŀ��
int nNewRaysNum			Ŀ�곤��
int nDetectorCenter		Ŀ������
float *d_pSrc			Դ
int Width				Դ����
int nHalfWidth			Դ����
float fPixelDistance	���ؼ��
*/
__global__ void cudaCopyTempData(float *d_pDst, int nNewRaysNum, int nDetectorCenter, float *d_pSrc, int Width, int nHalfWidth, float fPixelDistance)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i < nNewRaysNum)
	{
		int s = (i - nDetectorCenter) + nHalfWidth;
		if (s >= 0 && s < Width)
			d_pDst[i] = d_pSrc[s];
	}
}


// radon�任�ĺ˺���
/*
float* d_pSrc, int Width, int Height			Դ�Ϳ�ȡ��߶�
float* d_pDst,int NewWidth, int NewHeight		Ŀ��Ϳ�ȡ��߶�
int Xmin, int Ymin								Ŀ������½Ƕ���
float cos_theta, float sin_theta				��ת�Ƕȵ�������
*/
__global__ void cudaRotateKernel(float* d_pSrc, int Width, int Height, float* d_pDst, int NewWidth, int NewHeight, int Xmin, int Ymin, float cos_theta, float sin_theta)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x; //i��
	int j = blockIdx.y * blockDim.y + threadIdx.y; //j��
	if (i < NewHeight && j < NewWidth)
	{
		float x = float(j + Xmin);
		float y = float(i + Ymin);
		cudaPositionTransform(x, y, cos_theta, -sin_theta);
		*(d_pDst + j + i * NewWidth) = cudaGetPositionValue(x, y, d_pSrc, Width, Height);
	}
}


// �������ã�����ͶӰͼ��
/*
float* h_pDst				ͶӰͼ��
int RaysNum					ÿ�Ƕ�������
int AnglesNum				�����Ƕȸ���
 ***��������������2015.4.17����***
float pixel_separation		���ؼ��
float angle_separation		�Ƕȼ��
BYTE* h_pSrc				ԭʼͼ��
int Width					ԭͼ���
int Height					ԭͼ�߶�
int Rowlen					ԭͼ���ֽ���
*/
__host__ const char* cudaRadon(float* h_pDst, int RaysNum, int AnglesNum, float pixel_separation, float angle_separation, BYTE* h_pSrc, int Width, int Height, int Rowlen)
{
	// cuda ���󼰴�����Ϣ
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
	if (deviceProp.major < 1) return "��������С��1.0.";
	else if (deviceProp.major == 1 && deviceProp.minor < 1) return "��������С��1.1.";

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
	float ZoomRate = 1.f / pixel_separation;
	int zoom_NewWidth = ZoomRate * Width;
	int zoom_NewHeight = ZoomRate * Height;
	d_pZoom = cudaImageZoom(d_pZoom, zoom_NewWidth, zoom_NewHeight, d_pSrc, Width, Height, Rowlen);
	Width = zoom_NewWidth;
	Height = zoom_NewHeight;
	Rowlen = Width;
	// ����ͼ��Ŵ�֮����������
	int nNewRaysNum = ComputeRaysNum(Width, Height);
	float *d_pTemp = NULL,	//�豸���м����ݣ�����nNewRaysNum
		*d_pOnes = NULL,	//�豸�˵�λ����������NewHeight
		*d_pWidth_add = NULL;

	error = cudaMalloc((void **)&d_pTemp, nNewRaysNum * sizeof(float));
	CHECK_ERRORS(error, errstr);
	float density = 1.f * nNewRaysNum / RaysNum;
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
		cudaRotateKernel<<<dimGrid, dimBlock>>>(d_pZoom, Width, Height, d_pRotatedBits, NewWidth, NewHeight, Xmin, Ymin, cos_theta, sin_theta);
		error = cudaGetLastError();
		CHECK_ERRORS(error, errstr);

		//ͼ�����ۼ�
		error = cudaMalloc((void **)&d_pOnes, NewHeight * sizeof(float));
		CHECK_ERRORS(error, errstr);
		error = cudaMalloc((void **)&d_pWidth_add, NewWidth * sizeof(float));
		CHECK_ERRORS(error, errstr);
		//�˺�����<<<1, N>>>�߳����ޣ�ֻ�ܴ���512��С.�������ÿ��blockʹ��ThreadsNum���̣߳�������grid
		int ThreadsNum = 256;
		cudaVectorAssigned<<<(NewHeight + ThreadsNum - 1) / ThreadsNum, ThreadsNum>>>(d_pOnes, NewHeight, 1.f);
		error = cudaGetLastError();
		CHECK_ERRORS(error, errstr);
		cudaVectorAssigned<<<(nNewRaysNum + ThreadsNum - 1) / ThreadsNum, ThreadsNum>>>(d_pTemp, nNewRaysNum, 0.f);
		error = cudaGetLastError();
		CHECK_ERRORS(error, errstr);
		cudaVectorAssigned<<<(NewWidth + ThreadsNum - 1) / ThreadsNum, ThreadsNum>>>(d_pWidth_add, NewWidth, 0.f);
		error = cudaGetLastError();
		CHECK_ERRORS(error, errstr);
		cubStatus = cublasSgemv(cubHandle, cubTrans, NewWidth, NewHeight, &fAlpha, d_pRotatedBits, NewWidth, d_pOnes, 1, &fBeta, d_pWidth_add, 1);
		CHECK_CUBLAS_ERRORS(cubStatus, errstr);
		cudaFree(d_pOnes);
		cudaFree(d_pRotatedBits);
		// ̽�������ĺ�ͼ���ȵ�����
		int nDetectorCenter = (nNewRaysNum + 1) / 2;
		int nHalfWidth = (NewWidth + 1) / 2;
		cudaCopyTempData<<<(nNewRaysNum + ThreadsNum - 1) / ThreadsNum, ThreadsNum>>>(d_pTemp, nNewRaysNum, nDetectorCenter, d_pWidth_add, NewWidth, nHalfWidth, pixel_separation);
		error = cudaGetLastError();
		CHECK_ERRORS(error, errstr);
		cudaCopyColumnData<<<(RaysNum + ThreadsNum - 1) / ThreadsNum, ThreadsNum>>>(d_pDst, AnglesNum, d_pTemp, nNewRaysNum, density, index, pixel_separation);
		error = cudaGetLastError();
		CHECK_ERRORS(error, errstr);
		cudaFree(d_pWidth_add);
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
/*
float* d_pDst, float wRatio, float hRatio, int NewWidth, int NewHeight		Ŀ�꼰�Ŵ��ʡ�������Ϣ
BYTE* d_pSrc, int Width, int Height, int Rowlen								ԭͼ���ȡ��߶ȼ�ÿ�и�����
*/
__global__ void cudaZoomKernel(float* d_pDst, float wRatio, float hRatio, int NewWidth, int NewHeight, BYTE* d_pSrc, int Width, int Height, int Rowlen)
{
	int j = blockIdx.x * blockDim.x + threadIdx.x; //j��
	int i = blockIdx.y * blockDim.y + threadIdx.y; //i��
	if ( i < NewWidth && j < NewHeight)
		d_pDst[i + j * NewWidth] = cudaGetImagePixel(i * wRatio, j * wRatio, d_pSrc, Width, Height, Rowlen);
}


// ��ͼ����зŴ�
/*
float* pDst		Ŀ������
int NewWidth	Ŀ����
int NewHeight	Ŀ��߶�
BYTE* pSrc		ԭͼ����
int Width		ԭͼ���
int Height		ԭͼ�߶�
int Rowlen		ԭͼÿ���ֽ���
 ** ע�� ** ���뽫d_pDst��Ϊ������ء�
*/
__host__ float* cudaImageZoom(float* d_pDst, int NewWidth, int NewHeight, BYTE* d_pSrc, int Width, int Height, int Rowlen)
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
	cudaZoomKernel<<<dimGrid, dimBlock>>>(d_pDst, wRatio, hRatio, NewWidth, NewHeight, d_pSrc, Width, Height, Rowlen);
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
__device__ float cudaGetImagePixel(float x, float y, BYTE* pSrc, int Width, int Height, int Rowlen)
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
__device__ float cudaGetPositionValue(int x, int y, float* pSrc, int Width, int Height)
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
	Ans1 = cudaGetPositionValue(x1, y1, pSrc, Width, Height) * (1 - x + x1) + cudaGetPositionValue(x2, y2, pSrc, Width, Height) * (x - x1);
	Ans2 = cudaGetPositionValue(x4, y4, pSrc, Width, Height) * (1 - x + x4) + cudaGetPositionValue(x3, y3, pSrc, Width, Height) * (x - x4);
	return (Ans1 * (1 - y + y1) + Ans2 * (y - y1));
}


// �豸�ˣ�������(x, y)��תangle�Ƕȡ�
__device__ void cudaPositionTransform(float &x, float &y, float theta)
{
	float cos_theta = cos(theta);
	float sin_theta = sin(theta);
	float x_temp = x * cos_theta - y * sin_theta;
	y = x * sin_theta + y * cos_theta;
	x = x_temp;
}


// �豸�ˣ������޸�Ϊ�Ƕȵ����Һ����ҡ�
__device__ void cudaPositionTransform(float &x, float &y, float cos_theta, float sin_theta)
{
	float x_temp = x * cos_theta - y * sin_theta;
	y = x * sin_theta + y * cos_theta;
	x = x_temp;
}