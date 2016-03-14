//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVROI
//* .File Name       : $Workfile:   SVROI.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:40:08  $
//******************************************************************************

#include "stdafx.h"
#include "SVROI.h"

#include "SVImageLibrary/SVDrawContext.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"

#include "SVImageClass.h"
#include "SVTool.h"

SV_IMPLEMENT_CLASS( SVROIClass, SVROIClassGuid )

SVROIClass::SVROIClass( SVObjectClass* POwner, int StringResourceID )
		   :SVTaskObjectClass( POwner, StringResourceID ) 
{
	// Identify yourself
	outObjectInfo.ObjectTypeInfo.ObjectType = SVROIObjectType;

	// Identify our input type needs
	inputImageObjectInfo.SetInputObjectType( SVImageObjectType );
	inputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputImageObjectInfo, _T( "ROIImage" ) );


	// Register Embedded Objects

	// Set Embedded defaults

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVROIClass::~SVROIClass()
{

}

BOOL SVROIClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if( SVTaskObjectClass::CreateObject( PCreateStructure ) )
	{
		if( getInputImage() != NULL )
		{
			// Image input must already exist, and must be created!!!
			
			bOk = TRUE;
		}
	}

	isCreated = bOk;

	return bOk;
}

BOOL SVROIClass::CloseObject()
{
	return SVTaskObjectClass::CloseObject();
}

DWORD_PTR SVROIClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;

	return( SVTaskObjectClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

SVImageClass *SVROIClass::getInputImage()
{
	SVImageClass *l_psvImage = NULL;

	if( inputImageObjectInfo.IsConnected() )
	{
		l_psvImage = dynamic_cast<SVImageClass *>(inputImageObjectInfo.GetInputObjectInfo().PObject);
	}

	return l_psvImage;
}

BOOL SVROIClass::OnValidate()
{
	BOOL bRetVal = getInputImage() != NULL;

	bRetVal = bRetVal && SVTaskObjectClass::OnValidate();

	// Note: Make sure this is called when Validate fails !!!
	if( ! bRetVal )
	{
		SetInvalid();
	}

	return bRetVal;
}

