//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageBufferStruct
//* .File Name       : $Workfile:   SVImageBufferStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:49:48  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVObjectLibrary/SVObjectReference.h"
#include "SVImageBuffer.h"	// SVImageOverlayClass

#include "SVLibrary/SVReferenceCountingTemplate.h"
#include "SVScalarValue.h"
#pragma endregion Includes

struct SVImageBufferStruct
{
	SVSmartHandlePointer       handle;
	SVImageInfoClass           info;
	CString                    strName;
	SVObjectReference          object;
	SVImageOverlayClass        overlay;

	SVImageBufferStruct() {}
	SVImageBufferStruct( const SVImageBufferStruct& rhs ) : handle(rhs.handle), info(rhs.info), strName(rhs.strName), object(rhs.object), overlay(rhs.overlay) {}	// needs to be defined to avoid infinite recursion
	explicit SVImageBufferStruct( const SVScalarValue& rhs );
	operator SVScalarValue() const;
	const SVImageBufferStruct& operator = ( const SVImageBufferStruct& rhs );

	HRESULT CreateBuffer();
	HRESULT DestroyBuffer();
};

// Resource Manager policy class for SVReferenceCountingTemplate
struct SVImageBufferStructResourceManager
{
	static void      DefaultCreate ( SVImageBufferStruct* ) {};
	static HRESULT Create        ( SVImageBufferStruct* );
	static HRESULT Destroy       ( SVImageBufferStruct* );
	static void      Clear         ( SVImageBufferStruct& );
};

typedef SVReferenceCountingTemplate < SVImageBufferStruct, SVImageBufferStructResourceManager > SVRefCountImageBufferStruct;

typedef std::vector <SVImageBufferStruct>  SVImageBufferStructVectorType;
typedef std::vector <SVRefCountImageBufferStruct> SVRefCountImageBufferStructVectorType;

