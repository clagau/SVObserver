// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.


#include "stdafx.h"
#include "svmaskeditor.h"

namespace SvMc
{
	IMPLEMENT_DYNCREATE(CSVMaskEditor, CWnd)

	LPUNKNOWN CSVMaskEditor::GetImageDIBHandle()
	{
		LPUNKNOWN result;
		GetProperty(0x1, VT_UNKNOWN, (void*)&result);
		return result;
	}

	void CSVMaskEditor::SetImageDIBHandle(LPUNKNOWN propVal)
	{
		SetProperty(0x1, VT_UNKNOWN, propVal);
	}

	long CSVMaskEditor::GetMaskOperator()
	{
		long result;
		GetProperty(0x2, VT_I4, (void*)&result);
		return result;
	}

	void CSVMaskEditor::SetMaskOperator(long propVal)
	{
		SetProperty(0x2, VT_I4, propVal);
	}

	LPUNKNOWN CSVMaskEditor::GetMaskData()
	{
		LPUNKNOWN result;
		GetProperty(0x3, VT_UNKNOWN, (void*)&result);
		return result;
	}
	void CSVMaskEditor::SetMaskData(LPUNKNOWN propVal)
	{
		SetProperty(0x3, VT_UNKNOWN, propVal);
	}


	void CSVMaskEditor::Refresh()
	{
		InvokeHelper(DISPID_REFRESH, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}

	long CSVMaskEditor::DoEditMaskModal()
	{
		long result;
		InvokeHelper(0x4, DISPATCH_METHOD, VT_I4, (void*)&result, nullptr);
		return result;
	}

	void CSVMaskEditor::AboutBox()
	{
		InvokeHelper(0xfffffdd8, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}
} //namespace SvMc
