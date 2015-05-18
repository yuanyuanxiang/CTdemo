#include "BackProject.h"


/*
// ******************* 滤波 的实现 ***************** //
   ********** 首都师范大学 检测成像实验室 ********** //
// *************** 袁沅祥，2015年4月 *************** //
*/


#define CHECK_ERRORS(err, str) if(err != cudaSuccess) { str = cudaGetErrorString(err); cudaDeviceReset(); return str; }

// 反投影的核函数(当核函数中for循环次数比较高的时候，显卡会死掉，原因不明)
/*
前8个参数和BackProject的参数一样。
int cx		图像中心坐标，相对左下角
int cy		图像中心坐标，相对左下角
int med		射线的中心坐标，固定的角度
*/
__global__ void cudaBackProjectKernel(float* d_pDst, float* d_prj, int width, int height, int rays, int angles, float delta_r, float delta_fai, 
								  int cx, int cy, int med)
{
	int m = blockIdx.x * blockDim.x + threadIdx.x; //i行
	int n = blockIdx.y * blockDim.y + threadIdx.y; //j列
	if(m < height && n < width)
	{
		for (int i = 0; i < angles; ++i)
		{
			float fai = i * delta_fai;
			float r = (n - cx) * cos(fai) + (m - cy) * sin(fai);
			d_pDst[n + m * width] += cudaLinearInterp(med + r / delta_r, i, d_prj, angles, rays);
		}
		d_pDst[n + m * width] *= delta_fai;
	}
}


// 反投影
/*
float* h_pDst		目标
float* prj			卷积后的投影
int width			重建图像的宽
int height			重建图像的高
int rays			投影的行数(r)
int angles			投影的列数(fai)
float delta_r		扫描参数
float delta_fai		扫描参数
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
	// 如果计算能力 < 1.1
	if (deviceProp.major < 1) return errstr;
	else if (deviceProp.major == 1 && deviceProp.minor < 1) return errstr;

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

	dim3 dimBlock(16, 16);
	dim3 dimGrid((height + dimBlock.x - 1) / dimBlock.x, (width + dimBlock.y - 1) / dimBlock.y);
	cudaBackProjectKernel<<<dimGrid, dimBlock>>>(d_pDst, d_prj, width, height, rays, angles, delta_r, delta_fai, cx, cy, med);
	error = cudaGetLastError();
	CHECK_ERRORS(error, errstr);
	error = cudaMemcpy(h_pDst, d_pDst, width * height * sizeof(float), cudaMemcpyDeviceToHost);
	CHECK_ERRORS(error, errstr);

	cudaFree(d_pDst);
	cudaFree(d_prj);
	cudaDeviceReset();
	return errstr;
	/*
	for (int m = 0; m < height; ++m)
	{
		for (int n = 0; n < width; ++n)
		{
			x1 = n - cx;
			y1 = m - cy;
			for (int i = 0; i < angles; ++i)
			{
				fai = i * delta_fai;
				r = x1 * cos(fai) + y1 * sin(fai);
				r_id = r / delta_r;
				h_pDst[n + m * width] += LinearInterp(med + r_id, i, h_prj, angles, rays);
			}
			h_pDst[n + m * width] *= delta_fai;
		}
	}
	*/
}


// 在(r, fai)网格中插值出某点的值。线性插值。
__device__ float cudaLinearInterp(float r_id, int fai_id, float* prj, int angles, int rays)
{
	int r1 = floor(r_id), r2 = r1 + 1;
	if ( r1 < 0 || r2 >= rays)
		return 0.f;
	return prj[fai_id + r1 * angles] * (1 - r_id + r1) + prj[fai_id + r2 * angles] * (r_id - r1);
}