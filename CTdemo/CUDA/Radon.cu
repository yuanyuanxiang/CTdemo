#include "radon.h"
#include "cublas.h"
#include "cublas_v2.h"

/* 
// ***************** radon 变换的实现 ************** //
   ********** 首都师范大学 检测成像实验室 ********** //
// *************** 袁沅祥，2015年4月 *************** //
*/

#define CHECK_ERRORS(err, str) if(err != cudaSuccess) { str = cudaGetErrorString(err); cudaDeviceReset(); return str; }

#define CHECK_CUBLAS_ERRORS(err, str) if (err != CUBLAS_STATUS_SUCCESS) { str = "CUBLAS 遇到错误。"; cudaDeviceReset(); return str; }

/* 初始化向量vec = [val, val, val, ...]
float*	vec:向量指针
int		len:向量长度
float	val:被赋的值
注意：请声明一维的block
*/
__global__ void cudaVectorAssigned(float* vec, int len, float val)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i < len)
		vec[i] = val;
}


// 拷贝一列向量数据到矩阵某一列
/*
float *d_pDst		矩阵
int RaysNum			矩阵行
int AnglesNum		矩阵列
float *d_pVec		向量
int ImageDiag		图像对角线/向量长度
int column			拷贝的列数
int HalfRays		矩阵行一半
int HalfDiag		向量长度一半
float Separation	射线间距
注意：请声明一维的block
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


// 拷贝数据：d_pDst[int ((i - nHalfDiag) / fPixelDistance + nHalfWidth)] = d_pSrc[temp];
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


// radon变换的核函数
__global__ void cudaRotateKernel(float* d_pDst, float* d_pSrc, int Width, int Height, int Rowlen, int NewWidth, int NewHeight, int Xmin, int Ymin, float cos_theta, float sin_theta)
{
	/* 把下述两层for循环改写成CUDA内核语言。
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
	int i = blockIdx.x * blockDim.x + threadIdx.x; //i行
	int j = blockIdx.y * blockDim.y + threadIdx.y; //j列
	if (i < NewHeight && j < NewWidth)
	{
		float x = float(j + Xmin);
		float y = float(i + Ymin);
		cudaPositonTransform(x, y, cos_theta, -sin_theta);
		*(d_pDst + j + i * NewWidth) = cudaGetPositionValue(x, y, d_pSrc, Width, Height);
	}
}


// 函数作用：计算投影图像
/*
float* h_pDst				投影图像
int RaysNum					每角度射线数
int AnglesNum				采样角度个数
 ***下述两个参数于2015.4.17增加***
float rays_separation		射线间距
float angle_separation		角度间距
BYTE* h_pSrc				原始图像
int Width					原图宽度
int Height					原图高度
int Rowlen					原图行字节数
*/
__host__ const char* cudaRadon(float* h_pDst, int RaysNum, int AnglesNum, float rays_separation, float angle_separation, BYTE* h_pSrc, int Width, int Height, int Rowlen, float fSubPixel)
{
	// cuda 错误
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
	// 如果计算能力 < 1.1
	if (deviceProp.major < 1) return errstr;
	else if (deviceProp.major == 1 && deviceProp.minor < 1) return errstr;

	// 设备端数据
	BYTE *d_pSrc = NULL;
	float *d_pDst = NULL;
	// 复制数据
	error = cudaMalloc((void **)&d_pSrc, Height * Rowlen * sizeof(BYTE));
	CHECK_ERRORS(error, errstr);
	error = cudaMemcpy(d_pSrc, h_pSrc, Height * Rowlen * sizeof(BYTE), cudaMemcpyHostToDevice);
	CHECK_ERRORS(error, errstr);
	error = cudaMalloc((void **)&d_pDst, RaysNum * AnglesNum * sizeof(float));
	CHECK_ERRORS(error, errstr);
	error = cudaMemcpy(d_pDst, h_pDst, RaysNum * AnglesNum * sizeof(float), cudaMemcpyHostToDevice);
	CHECK_ERRORS(error, errstr);
	// 原始图像对角线长度
	int ImageDiag = (int)sqrt(1.0f * Width * Width + Height * Height);
	float *d_pTemp = NULL,	//设备端中间数据，长度ImageDiag
		*d_pOnes = NULL,	//设备端单位向量，长度NewHeight
		*d_pTemp2 = NULL;

	error = cudaMalloc((void **)&d_pTemp, ImageDiag * sizeof(float));
	CHECK_ERRORS(error, errstr);
	
	// 被调用cublas函数的参数
	float fAlpha = 1.0f, fBeta  = 0.0f;
	cublasHandle_t cubHandle;
	cublasStatus_t cubStatus;
	cubStatus = cublasInit();
	CHECK_CUBLAS_ERRORS(cubStatus, errstr);
	cubStatus = cublasCreate(&cubHandle);
	CHECK_CUBLAS_ERRORS(cubStatus, errstr);
	cublasOperation_t cubTrans = CUBLAS_OP_N;
	// 先放大图像
	float *d_pZoom = NULL;
	// 放大倍率
	float ZoomRate = 1.f / fSubPixel;
	d_pZoom = cudaImageZoom(d_pZoom, d_pSrc, ZoomRate * Width, ZoomRate * Height, Width, Height, Rowlen);
	Width *= ZoomRate;
	Height *= ZoomRate;
	Rowlen = Width;
	// 沿某个方向线积分，index表示第几个角度
	for (int index = 0; index < AnglesNum; ++index)
	{
		// 原始图像四个顶点的坐标
		float x1, x2, x3, x4, y1, y2, y3, y4;
		x1 = 0;							y1 = 0;
		x2 = float(Width - 1);			y2 = 0;
		x3 = x2;						y3 = float(Height - 1);
		x4 = x1;						y4 = y3;
		// 四个顶点顺时针旋转
		float angle = index * angle_separation;
		// 2015.5.19 为了减少计算，把三角函数放到循环外面
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
		// 新图像宽度、高度的变化
		int NewWidth = Xmax - Xmin + 1;
		int NewHeight = Ymax - Ymin + 1;
		// 分配内存，个数：NewWidth * NewHeight
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

		//图像按列累加
		error = cudaMalloc((void **)&d_pOnes, NewHeight * sizeof(float));
		CHECK_ERRORS(error, errstr);
		error = cudaMalloc((void **)&d_pTemp2, NewWidth * sizeof(float));
		CHECK_ERRORS(error, errstr);
		//核函数中<<<1, N>>>线程受限，只能处理512大小.现在设计每个block使用ThreadsNum个线程，并计算grid
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


// 图相放大的核函数
__global__ void cudaZoomKernel(float* d_pDst, BYTE* d_pSrc, float wRatio, float hRatio, int NewWidth, int NewHeight, int Width, int Height, int Rowlen)
{
	int j = blockIdx.x * blockDim.x + threadIdx.x; //j行
	int i = blockIdx.y * blockDim.y + threadIdx.y; //i列
	if ( i < NewWidth && j < NewHeight)
		d_pDst[i + j * NewWidth] = cudaGetPositionPixel(i * wRatio, j * wRatio, d_pSrc, Width, Height, Rowlen);
}


// 对图像进行放大
/*
float* pDst		目标数据
BYTE* pSrc		原图数据
int NewWidth	目标宽度
int NewHeight	目标高度
int Width		原图宽度
int Height		原图高度
int Rowlen		原图每行字节数
 ** 注意 ** 必须将d_pDst作为结果返回。
*/
__host__ float* cudaImageZoom(float* d_pDst, BYTE* d_pSrc, int NewWidth, int NewHeight, int Width, int Height, int Rowlen)
{
	// 如果目标非空，返回空。
	if (d_pDst != NULL)
		return NULL;
	// 如果分配内存失败，返回空
	if(cudaMalloc((void **)&d_pDst, NewWidth * NewHeight * sizeof(float)))
		return NULL;
	// 图像宽度、高度缩放比例
	float wRatio = 1.f * Width / NewWidth;
	float hRatio = 1.f * Height / NewHeight;
	// 调用核函数
	dim3 dimBlock(16, 16);
	dim3 dimGrid((NewHeight + dimBlock.x - 1) / dimBlock.x, (NewWidth + dimBlock.y - 1) / dimBlock.y);
	cudaZoomKernel<<<dimGrid, dimBlock>>>(d_pDst, d_pSrc, wRatio, hRatio, NewWidth, NewHeight, Width, Height, Rowlen);
	cudaError_t error = cudaGetLastError();

	return d_pDst;
}


// 获取图像矩阵x列，y行的像素值。
__device__ float cudaGetImagePixel(int x, int y, BYTE* pSrc, int Width, int Height, int Rowlen)
{
	if (x < 0 || x >= Width || y < 0 || y >= Height)
		return 0;
	return (float)*(pSrc + x + y * Rowlen);
}


// 获取坐标(x, y)处的像素值，采用双线性插值。
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


// 获取矩阵x列，y行的像素值。
__device__ float cudaGetIndexValue(int x, int y, float* pSrc, int Width, int Height)
{
	if (x < 0 || x >= Width || y < 0 || y >= Height)
		return 0;
	return (float)*(pSrc + x + y * Width);
}


// 获取坐标(x, y)处的像素值，采用双线性插值。
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


// 设备端：对坐标(x, y)旋转angle角度。
__device__ void cudaPositonTransform(float &x, float &y, float theta)
{
	float cos_theta = cos(theta);
	float sin_theta = sin(theta);
	float x_temp = x * cos_theta - y * sin_theta;
	y = x * sin_theta + y * cos_theta;
	x = x_temp;
}


// 设备端：参数修改为角度的余弦和正弦。
__device__ void cudaPositonTransform(float &x, float &y, float cos_theta, float sin_theta)
{
	float x_temp = x * cos_theta - y * sin_theta;
	y = x * sin_theta + y * cos_theta;
	x = x_temp;
}