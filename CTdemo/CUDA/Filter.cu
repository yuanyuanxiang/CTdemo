#include "Filter.h"
#include "cublas.h"
#include "cublas_v2.h"

/*
// ******************* �˲� ��ʵ�� ***************** //
   ********** �׶�ʦ����ѧ ������ʵ���� ********** //
// *************** Ԭ���飬2015��4�� *************** //
*/


/*
	g(r, fai) = Fourier(radon(r, fai)) * HW(r)
	radon�任�ĸ���Ҷ�任���˲����Ƕ�ά����Ҷ�任.
*/


#define CHECK_ERRORS(err, str) if(err != cudaSuccess) { str = cudaGetErrorString(err); cudaDeviceReset(); return str; }


// �������Ⱦ���ת��Ϊ�����Ⱦ���
/*
float *d_pSrc		�����Ⱦ���
float *d_temp		�м�����
int row				�������
int col				�������
*/
__global__ void cudaRowMajor2ColMajor(float *d_pSrc, float *d_temp, int row, int col)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x; //i�У����temp
	int j = blockIdx.y * blockDim.y + threadIdx.y; //j�У����temp
	if (i < row && j < col)
		d_pSrc[i + j * row] = d_temp[j + i * col];
}


// �������Ⱦ���ת��Ϊ�����Ⱦ���
/*
float *d_pSrc		�����Ⱦ���
float *d_temp		�м�����
int row				�������
int col				�������
*/
__global__ void cudaColMajor2RowMajor(float *d_pSrc, float *d_temp, int row, int col)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x; //i�У����d_pSrc
	int j = blockIdx.y * blockDim.y + threadIdx.y; //j�У����d_pSrc
	if (i < row && j < col)
		d_pSrc[j + i * col] = d_temp[i + j * row];
}


// ��ʼ�������
/*
float* d_Conv		�����(������)
int row				����˵���
int col				����˵���
float w0			��ֹƵ��
*/
__global__ void cudaInitConvolute(float* d_pConv, int row, int col, float w0)
{
	int m = blockIdx.x * blockDim.x + threadIdx.x; //m��
	int n = blockIdx.y * blockDim.y + threadIdx.y; //n��
	if (m < row && n < col)
		d_pConv[m + n * row] = cudaConvKernel(m - n, w0);
}


// ����ˣ�HW(r). w0 - ��Ƶ��ֹ
__device__ float cudaConvKernel(float x, float w0)
{
	if(x == 0) return w0 * w0;
	float theta = 2 * PI * w0 * x;
	return (-1 + cos(theta) + theta * sin(theta)) / (2 * PI * PI * x * x);
}


// RL�˲���: w0 = 1 / (2d)
__device__ float cudaRLFilter(int n, float d)
{
	if (n == 0) return 1.f / (4 * d * d);
	if (n % 2 == 0) return 0.f;
	return -1.f / (PI * PI * n * n * d * d);
}


// �������ͶӰ���������
/*
float* h_pDst				Ŀ��
float* prj					ָ��ͶӰ���ݵ�ָ��
int row						ͶӰ���ݵ��У��˴���ʾr
int col						ͶӰ���ݵ��У��˴���ʾfai
float delta_r				��ɢ��֮���dr
float w0					��Ƶ��ֹ
*/
__host__ const char* cudaConvolute(float* h_pDst, float* h_prj, int row, int col, float delta_r, float w0)
{
	cudaError_t error;
	const char* errstr = 0;
	cudaDeviceProp deviceProp;
	int devID = 0;
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

	float *d_prj = NULL, *d_pDst = NULL, *d_temp = NULL;
	size_t length = row * col * sizeof(float);

	error = cudaMalloc((void **)&d_prj, length);
	CHECK_ERRORS(error, errstr);
	error = cudaMalloc((void **)&d_pDst, length);
	CHECK_ERRORS(error, errstr);
	error = cudaMalloc((void **)&d_temp, length);
	CHECK_ERRORS(error, errstr);
	error = cudaMemset(d_pDst, 0, length);
	CHECK_ERRORS(error, errstr);

	cublasInit();
	float *d_pConv = NULL;
	error = cudaMalloc((void **)&d_pConv, row * row * sizeof(float));
	CHECK_ERRORS(error, errstr);
	dim3 dimBlock(16, 16);
	dim3 dimGrid((row + dimBlock.x - 1) / dimBlock.x, (row + dimBlock.y - 1) / dimBlock.y);
	cudaInitConvolute<<<dimGrid, dimBlock>>>(d_pConv, row, row, w0);
	error = cudaGetLastError();
	CHECK_ERRORS(error, errstr);
	// �������Ⱦ���ת��Ϊ�����Ⱦ���
	error = cudaMemcpy(d_temp, h_prj, length, cudaMemcpyHostToDevice);
	CHECK_ERRORS(error, errstr);
	cudaRowMajor2ColMajor<<<dimGrid, dimBlock>>>(d_prj, d_temp, row, col);
	error = cudaGetLastError();
	CHECK_ERRORS(error, errstr);

	float alpha = 1.f, belta = 0.f;
	cublasHandle_t handle;
	cublasCreate(&handle);
	//P = C * P -> P' = P' * C';
	cublasSgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, row, col, row, &alpha, d_pConv, row, d_prj, row, &belta, d_pDst, row);
	// �������Ⱦ���ת��Ϊ�����Ⱦ���
	cudaColMajor2RowMajor<<<dimGrid, dimBlock>>>(d_temp, d_pDst, row, col);
	error = cudaGetLastError();
	CHECK_ERRORS(error, errstr);
	error = cudaMemcpy(h_pDst, d_temp, length, cudaMemcpyDeviceToHost);
	CHECK_ERRORS(error, errstr);

	cudaFree(d_pDst);
	cudaFree(d_prj);
	cudaFree(d_pConv);
	cudaFree(d_temp);
	cublasDestroy(handle);
	cublasShutdown();
	cudaDeviceReset();
	return errstr;
	/* ������ʽ��P(m, n) = C(m, m) * P(m, n)
	for (int m = 0; m < row; ++m)
	{
		for (int n = 0; n < col; ++n)
		{
			for (int i = 0; i < row; ++i)
				h_pDst[n + m * col] += h_prj[n + i * col] * ConvKernel(m - i, w0);
		}
	}*/
}