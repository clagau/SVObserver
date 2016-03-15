// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOLibrary
// * .File Name       : $Workfile:   SVOLibrary.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 11:56:16  $
// ******************************************************************************

#if !defined(AFX_SVOLIBRARY_H__52D66EC9_2595_455A_9093_4D8DF3829D47__INCLUDED_)
#define AFX_SVOLIBRARY_H__52D66EC9_2595_455A_9093_4D8DF3829D47__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <comdef.h>

enum SVOVariantTypeEnum
{
	SVEmptyType = VT_EMPTY,
	SVErrorType = VT_ERROR,
	SVHResultType = VT_HRESULT,
	SVArrayType = VT_ARRAY,
	SVBStrType = VT_BSTR,
	SVBoolType = VT_BOOL,
	SVFloatType = VT_R4,
	SVDoubleType = VT_R8,
	SVCharType = VT_I1,
	SVShortType = VT_I2,
	SVIntType = VT_INT,
	SVLongType = VT_I4,
	SVUnsignedCharType = VT_UI1,
	SVUnsignedShortType = VT_UI2,
	SVUnsignedIntType = VT_UINT,
	SVUnsignedLongType = VT_UI4,
	SVClsIdType = VT_CLSID,
	SVBStrArrayType = VT_BSTR | VT_ARRAY,
	SVBoolArrayType = VT_BOOL | VT_ARRAY,
	SVFloatArrayType = VT_R4 | VT_ARRAY,
	SVDoubleArrayType = VT_R8 | VT_ARRAY,
	SVCharArrayType = VT_I1 | VT_ARRAY,
	SVShortArrayType = VT_I2 | VT_ARRAY,
	SVIntArrayType = VT_INT | VT_ARRAY,
	SVLongArrayType = VT_I4 | VT_ARRAY,
	SVUnsignedCharArrayType = VT_UI1 | VT_ARRAY,
	SVUnsignedShortArrayType = VT_UI2 | VT_ARRAY,
	SVUnsignedIntArrayType = VT_UINT | VT_ARRAY,
	SVUnsignedLongArrayType = VT_UI4 | VT_ARRAY,
	SVClsIdArrayType = VT_CLSID | VT_ARRAY
};

#endif // !defined(AFX_SVOLIBRARY_H__52D66EC9_2595_455A_9093_4D8DF3829D47__INCLUDED_)

