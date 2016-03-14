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

#include "stdafx.h"
#include "SVImageProcessingClass.h"

#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"

#include "SVDataBuffer.h"
#include "SVTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVDataBufferInfoClass::SVDataBufferInfoClass()
{
	POwnerTool			= NULL;
	Length				= 0L;
	Type				= 0L;
}


SVDataBufferInfoClass::SVDataBufferInfoClass( SVDataBufferInfoClass& S2 )
{
	POwnerTool		= S2.POwnerTool;
	Length			= S2.Length;
	Type			= S2.Type;
	HBuffer			= S2.HBuffer;
}

SVDataBufferInfoClass SVDataBufferInfoClass::operator=( SVDataBufferInfoClass& S2 )
{
	POwnerTool		= S2.POwnerTool;
	Length			= S2.Length;
	Type			= S2.Type;
	HBuffer			= S2.HBuffer;

	return ( *this );
}


SV_IMPLEMENT_CLASS( SVDataBufferClass, SVDataBufferClassGuid );

SVDataBufferClass::SVDataBufferClass( SVObjectClass* POwner, int StringResourceID )
			      :SVObjectAppClass( POwner, StringResourceID )
{
	init();
	
}

void SVDataBufferClass::init()
{
	outObjectInfo.ObjectTypeInfo.ObjectType = SVDataBufferClassObjectType;
}

SVDataBufferClass::~SVDataBufferClass()
{
	CloseObject();
}

BOOL SVDataBufferClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if( SVObjectAppClass::CreateObject( PCreateStructure ) )
	{
		if( dataInfo.Type != 0 && dataInfo.Length > 0 )
		{
			dataInfo.POwnerTool = GetTool();

			// Create data buffer...

			bOk = SVImageProcessingClass::Instance().CreateDataBuffer(&dataInfo ) == S_OK;
		}
	}

	isCreated = bOk;

	return bOk;
}


BOOL SVDataBufferClass::CloseObject()
{
	BOOL rc = SVObjectAppClass::CloseObject();

	rc = (SVImageProcessingClass::Instance().DestroyDataBuffer( &dataInfo ) == S_OK ) && rc;

	return rc;
}

BOOL SVDataBufferClass::Resize( int NewLength )
{
	return SVImageProcessingClass::Instance().ResizeDataBuffer( &dataInfo, NewLength ) == S_OK;
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
	DWORD_PTR DwResult = NULL;

	return SVObjectAppClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext );
}

