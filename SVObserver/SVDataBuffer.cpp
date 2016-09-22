//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDataBuffer
//* .File Name       : $Workfile:   SVDataBuffer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:07:30  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVImageProcessingClass.h"
#include "SVDataBuffer.h"
#include "SVOMFCLibrary/SVDeviceParams.h"

#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVDataBufferInfoClass::SVDataBufferInfoClass()
: POwnerTool(nullptr)
, Length(0L)
, Type(0L)
{
}

SVDataBufferInfoClass::SVDataBufferInfoClass( const SVDataBufferInfoClass& S2 )
{
	POwnerTool	= S2.POwnerTool;
	Length		= S2.Length;
	Type		= S2.Type;
	HBuffer		= S2.HBuffer;
}

SVDataBufferInfoClass SVDataBufferInfoClass::operator=( SVDataBufferInfoClass& S2 )
{
	POwnerTool	= S2.POwnerTool;
	Length		= S2.Length;
	Type		= S2.Type;
	HBuffer		= S2.HBuffer;

	return ( *this );
}

SV_IMPLEMENT_CLASS( SVDataBufferClass, SVDataBufferClassGuid );

SVDataBufferClass::SVDataBufferClass( SVObjectClass* pOwner, int StringResourceID )
: SVObjectAppClass( pOwner, StringResourceID )
{
	init();
}

void SVDataBufferClass::init()
{
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVDataBufferClassObjectType;
}

SVDataBufferClass::~SVDataBufferClass()
{
	CloseObject();
}

BOOL SVDataBufferClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = false;

	if( SVObjectAppClass::CreateObject( PCreateStructure ) )
	{
		if( dataInfo.Type != 0 && 0 < dataInfo.Length )
		{
			dataInfo.POwnerTool = GetTool();

			// Create data buffer...
			bOk = S_OK == SVImageProcessingClass::Instance().CreateDataBuffer(&dataInfo);
		}
	}
	m_isCreated = bOk;

	return bOk;
}

BOOL SVDataBufferClass::CloseObject()
{
	BOOL rc = SVObjectAppClass::CloseObject();

	rc = (S_OK == SVImageProcessingClass::Instance().DestroyDataBuffer( &dataInfo ) ) && rc;

	return rc;
}

BOOL SVDataBufferClass::Resize( int NewLength )
{
	return S_OK == SVImageProcessingClass::Instance().ResizeDataBuffer( &dataInfo, NewLength );
}

SVDataBufferInfoClass& SVDataBufferClass::GetDataBufferInfo()
{
	return dataInfo;
}

SVDataBufferHandleStruct& SVDataBufferClass::GetDataBufferHandle()
{
	return dataInfo.HBuffer;
}

DWORD_PTR SVDataBufferClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = SVMR_NOT_PROCESSED;

	return SVObjectAppClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext );
}
