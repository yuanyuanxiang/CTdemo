
// ******************************** //
// *********** 导出DLL ************ //
// ******************************** //
// ********  2015年3月26日  ******* //
// *****  首都师范大学 袁沅祥 ***** //

#include <afxwin.h>
#include <atlimage.h>

// 导出函数1，参数说明：pImage - 需要显示的图象。
extern "C" __declspec(dllexport) void ImageViewer(CImage* pImage);

// 导出函数2，参数说明：ptr、width、height、rowlen - 需要可视化的数据及宽度、高度、每行字节数。
extern "C" __declspec(dllexport) void BYTEViewer(BYTE* ptr, int width, int height, int rowlen);

// 导出函数3，参数说明：ptr、width、height、rowlen - 需要可视化的数据及宽度、高度、每行字节数。
extern "C" __declspec(dllexport) void IntViewer(int* ptr, int width, int height, int rowlen);

// 导出函数4，参数说明：ptr、width、height、rowlen - 需要可视化的数据及宽度、高度、每行字节数。
extern "C" __declspec(dllexport) void FloatViewer(float* ptr, int width, int height, int rowlen);

// 导出函数5，参数说明：ptr、width、height、rowlen - 需要可视化的数据及宽度、高度、每行字节数。
extern "C" __declspec(dllexport) void DoubleViewer(double* ptr, int width, int height, int rowlen);

// 导出函数6，参数说明：pImage - 需要显示的图象， 为了使形式和上述1-5形式兼容。
extern "C" __declspec(dllexport) void CImageViewer(CImage* pImage, int, int, int);