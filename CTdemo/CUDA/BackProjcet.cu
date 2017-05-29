#include "BackProject.h"


/*
// ******************* �˲� ��ʵ�� ***************** //
   ********** �׶�ʦ����ѧ ������ʵ���� ********** //
// *************** Ԭ���飬2015��4�� *************** //
*/


#define CHECK_ERRORS(err, str) if(err != cudaSuccess) { str = cudaGetErrorString(err); cudaDeviceReset(); return str; }


// �����Ǻ���ֵ���г�ʼ��
__global__ void cudaInitCosSin(float *cos_theta, float *sin_theta, float delta_fai, int angles)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i < angles)
	{
		float theta = i * delta_fai;
		cos_theta[i] = cos(theta);
		sin_theta[i] = sin(theta);
	}
}


// ��ͶӰ�ĺ˺���(���˺�����forѭ�������Ƚϸߵ�ʱ���Կ���������ԭ����)
/*
ǰ8��������BackProject�Ĳ���һ����
int cx		ͼ���������꣬������½�
int cy		ͼ���������꣬������½�
int med		���ߵ��������꣬�̶��ĽǶ�
*/
__global__ void cudaBackProjectKernel(float* d_pDst, float* d_prj, int width, int height, int rays, int angles, float *cos_fai, float *sin_fai, float delta_r, float delta_fai, 
								  int cx, int cy, int med)
{
	int m = blockIdx.x * blockDim.x + threadIdx.x; //i��
	int n = blockIdx.y * blockDim.y + threadIdx.y; //j��

	if(m < height && n < width)
	{
		for (int i = 0; i < angles; ++i)
		{
			float r = (n - cx) * cos_fai[i] + (m - cy) * sin_fai[i];
			d_pDst[n + m * width] += cudaLinearInterp(med + r * delta_r, i, d_prj, angles, rays);
		}
		d_pDst[n + m * width] *= delta_fai;
	}
}


// ��ͶӰ
/*
float* h_pDst		Ŀ��
float* prj			������ͶӰ
int width			�ؽ�ͼ��Ŀ�
int height			�ؽ�ͼ��ĸ�
int rays			ͶӰ������(r)
int angles			ͶӰ������(fai)
float delta_r		�����ܶ�
float delta_fai		ɨ�����
*/
__host__ const char* cudaBackProject(float* h_pDst, float* h_prj, int width, int height, int rays, int angles, float delta_r, float delta_fai)
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
	if (deviceProp.major < 1) return "��������С��1.0.";
	else if (deviceProp.major == 1 && deviceProp.minor < 1) return "��������С��1.1.";

	float *d_pDst = NULL;
	float *d_prj = NULL;
	error = cudaMalloc((void **)&d_pDst, width * height * sizeof(float));
	CHECK_ERRORS(error, errstr);
	error = cudaMemcpy(d_pDst, h_pDst, width * height * sizeof(float), cudaMemcpyHostToDevice);
	CHECK_ERRORS(error, errstr);
	error = cudaMalloc((void **)&d_prj, rays * angles * sizeof(float));
	CHECK_ERRORS(error, errstr);
	error = cudaMemcpy(d_prj, h_prj, rays * angles * sizeof(float), cudaMemcpyHostToDevice);
	CHECK_ERRORS(error, errstr);

	int cx = (width + 1) / 2;
	int cy = (height + 1) / 2;
	int med = (rays + 1) / 2;

	// 2015.5.19 �����Ǻ����ŵ�ѭ��������
	float *cos_fai = NULL;
	float *sin_fai = NULL;
	error = cudaMalloc((void **)&cos_fai, angles * sizeof(float));
	CHECK_ERRORS(error, errstr);
	error = cudaMalloc((void **)&sin_fai, angles * sizeof(float));
	CHECK_ERRORS(error, errstr);

	int ThreadsNum = 256;
	cudaInitCosSin<<<(angles + ThreadsNum - 1) / ThreadsNum, ThreadsNum>>>(cos_fai, sin_fai, delta_fai, angles);
	error = cudaGetLastError();
	CHECK_ERRORS(error, errstr);

	dim3 dimBlock(16, 16);
	dim3 dimGrid((height + dimBlock.x - 1) / dimBlock.x, (width + dimBlock.y - 1) / dimBlock.y);
	cudaBackProjectKernel<<<dimGrid, dimBlock>>>(d_pDst, d_prj, width, height, rays, angles, cos_fai, sin_fai, delta_r, delta_fai, cx, cy, med);
	error = cudaGetLastError();
	CHECK_ERRORS(error, errstr);

	error = cudaMemcpy(h_pDst, d_pDst, width * height * sizeof(float), cudaMemcpyDeviceToHost);
	CHECK_ERRORS(error, errstr);

	cudaFree(cos_fai);
	cudaFree(sin_fai);
	cudaFree(d_pDst);
	cudaFree(d_prj);
	cudaDeviceReset();
	return errstr;
}


// ��(r, fai)�����в�ֵ��ĳ���ֵ�����Բ�ֵ��
__device__ float cudaLinearInterp(float r_id, int fai_id, float* prj, int angles, int rays)
{
	int r1 = floor(r_id), r2 = r1 + 1;
	if ( r1 < 0 || r2 >= rays)
		return 0;
	return prj[fai_id + r1 * angles] * (1 - r_id + r1) + prj[fai_id + r2 * angles] * (r_id - r1);
}