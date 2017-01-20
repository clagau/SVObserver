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

#pragma region Includes
#include "stdafx.h"
#include "SVToolAcquisition.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVAcquisitionToolClass, SVAcquisitionToolClassGuid );

SVAcquisitionToolClass::SVAcquisitionToolClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
					   :SVToolClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	init();
}

void SVAcquisitionToolClass::init()
{
	// Indentify our type in the Output List
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType = SVToolAcquisitionObjectType;

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
		bOk = ( S_OK == SetImageExtent( 1, mainImageObject.GetImageExtents() ) );
		mainImageObject.ObjectAttributesAllowedRef() |= SV_PUBLISH_RESULT_IMAGE;
	}

	m_svSourceImageNames.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;
	m_svSourceImageNames.SetValue( 0, SVString(mainImageObject.GetCompleteName()) );

	m_isCreated = bOk;

	return bOk;
}

HRESULT SVAcquisitionToolClass::ResetObject()
{
	HRESULT l_hrOk = SVToolClass::ResetObject();
	HRESULT l_Temp = SetImageExtent( 1, mainImageObject.GetImageExtents() );

	if( S_OK == l_hrOk )
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

	bRetVal &= nullptr != mainImageObject.GetCamera();

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

SVStaticStringValueObjectClass* SVAcquisitionToolClass::GetInputImageNames()
{
	return &m_svSourceImageNames;
}


