
// ******************************** //
// *********** 导出DLL ************ //
// ******************************** //
// ********  2015年3月25日  ******* //
// *****  首都师范大学 袁沅祥 ***** //

#include <afxwin.h>
#include <atlimage.h>

// 该函数是外部调用 dll 的接口函数。参数说明：pImage - 需要修改的图象。
extern "C" __declspec(dllexport) void ImageBppEditor(CImage* pImage);