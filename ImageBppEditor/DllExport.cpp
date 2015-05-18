#include "stdafx.h"
#include "DllExport.h"
#include "ChangeBppDlg.h"

extern "C" __declspec(dllexport) void ImageBppEditor(CImage* pImage)
{
	if (pImage == NULL || pImage->IsNull())
		return;
	CChangeBppDlg dlg(pImage);
	dlg.DoModal();
}