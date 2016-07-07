//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageBufferStruct
//* .File Name       : $Workfile:   SVImageBufferStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:49:38  $
//******************************************************************************

#include "stdafx.h"
#include "SVImageBufferStruct.h"

#include "SVImageProcessingClass.h"
#include "SVOMFCLibrary/SVDeviceParams.h"

HRESULT SVImageBufferStruct::CreateBuffer()
{
	DestroyBuffer();
	return SVImageBufferStructResourceManager::Create( this );
}

HRESULT SVImageBufferStruct::DestroyBuffer()
{
	return SVImageBufferStructResourceManager::Destroy( this );
}
HRESULT SVImageBufferStructResourceManager::Create( SVImageBufferStruct* pStruct )
{
	return SVImageProcessingClass::Instance().CreateImageBuffer( pStruct->info, pStruct->handle );
}

HRESULT SVImageBufferStructResourceManager::Destroy( SVImageBufferStruct* pStruct )
{
	pStruct->handle.clear();

	return S_OK;
}

void SVImageBufferStructResourceManager::Clear( SVImageBufferStruct& rStruct )
{
	rStruct.handle = SVSmartHandlePointer();
}

SVImageBufferStruct::operator SVScalarValue() const
{
	return SVScalarValue(strName, CString(), object);
}

SVImageBufferStruct::SVImageBufferStruct( const SVScalarValue& rhs )
{
	strName = rhs.strName;
	object = rhs.object;
}

const SVImageBufferStruct& SVImageBufferStruct::operator = ( const SVImageBufferStruct& rhs )
{
	handle = rhs.handle;
	info = rhs.info;
	strName = rhs.strName;
	object = rhs.object;
	overlay = rhs.overlay;
	return *this;
}

