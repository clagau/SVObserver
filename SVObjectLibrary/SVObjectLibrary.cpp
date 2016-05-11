//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectLibrary
//* .File Name       : $Workfile:   SVObjectLibrary.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 09:42:28  $
//******************************************************************************

#include "stdafx.h"
#include "ObjectInterfaces/SVObjectTypeInfoStruct.h"
#include "SVObjectLibrary.h"

#include "SVObjectClass.h"
#include "SVObjectInfoStruct.h"
#include "SVObjectManagerClass.h"

const SVObjectInfoStruct		SVInvalidObjectInfo;
const SVObjectTypeInfoStruct	SVInvalidObjectTypeInfo;

// GUID for the SVObjectImageClass
/*const*/ GUID SVObjectImageClassGuid	= { 0xb62f5910, 0x6528, 0x11d1, { 0xF0, 0x11, 0x0, 0x0, 0xf8, 0x1e, 0xd, 0xd } };

//******************************************************************************
//* FUNCTION IMPLEMENTATION(S):
//******************************************************************************

DWORD_PTR SVSendMessage( SVObjectClass* PObject, DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	if( nullptr != PObject )
	{
		return PObject->processMessage( DwMessageID, DwMessageValue, DwMessageContext );
	}
	return SVMR_NOT_PROCESSED;
}


DWORD_PTR SVSendMessage( const GUID& RUniqueObjectID, DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject( RUniqueObjectID );

	// Check if object exists...
	if( nullptr != pObject )
	{
		return pObject->processMessage( DwMessageID, DwMessageValue, DwMessageContext );
	}
	return SVMR_NOT_PROCESSED;
}

