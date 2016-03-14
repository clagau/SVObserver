//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAcquisition
//* .File Name       : $Workfile:   SVToolAcquisition.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   14 Jul 2014 12:44:44  $
//******************************************************************************

#include "stdafx.h"
#include "SVToolAcquisition.h"

//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************


//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************



//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVAcquisitionToolClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//******************************************************************************
// Adjustments
//******************************************************************************

SV_IMPLEMENT_CLASS( SVAcquisitionToolClass, SVAcquisitionToolClassGuid );

//******************************************************************************
// Constructor(s):
//******************************************************************************

SVAcquisitionToolClass::SVAcquisitionToolClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
					   :SVToolClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	init();
}


void SVAcquisitionToolClass::init()
{
	// Indentify our type in the Output List
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	outObjectInfo.ObjectTypeInfo.SubType = SVToolAcquisitionObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &mainImageObject, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );

	// Register SourceImageNames Value Object
	RegisterEmbeddedObject( &m_svSourceImageNames, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SVResetItemTool );

	addDefaultInputObjects();
}


SVAcquisitionToolClass::~SVAcquisitionToolClass()
{
}


BOOL SVAcquisitionToolClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if( SVToolClass::CreateObject( PCreateStructure ) )
	{
		bOk = ( SetImageExtent( 1, mainImageObject.GetImageExtents() ) == S_OK );
		mainImageObject.ObjectAttributesAllowedRef() |= SV_PUBLISH_RESULT_IMAGE;
	}

	m_svSourceImageNames.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;
	m_svSourceImageNames.SetValue( 0, mainImageObject.GetCompleteObjectName() );

	isCreated = bOk;

	return bOk;
}

HRESULT SVAcquisitionToolClass::ResetObject()
{
	HRESULT l_hrOk = SVToolClass::ResetObject();
	HRESULT l_Temp = SetImageExtent( 1, mainImageObject.GetImageExtents() );

	if( l_hrOk == S_OK )
	{
		l_hrOk = l_Temp;
	}

	UpdateImageWithExtent( 1 );

	return l_hrOk;
}

BOOL SVAcquisitionToolClass::OnValidate()
{
	BOOL bRetVal = FALSE;

	bRetVal = SVToolClass::OnValidate();

	bRetVal &= mainImageObject.GetCamera() != NULL;

	if( !bRetVal )
	{
		SetInvalid();
	}

	return bRetVal;	
}

bool SVAcquisitionToolClass::DoesObjectHaveExtents() const
{
	return false;
}

DWORD_PTR SVAcquisitionToolClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;

	return( SVToolClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}


SVCameraImageTemplate* SVAcquisitionToolClass::GetMainImageClass()
{
	return &mainImageObject;
}

SVStaticStringValueObjectClass* SVAcquisitionToolClass::GetInputImageNames()
{
	return &m_svSourceImageNames;
}


