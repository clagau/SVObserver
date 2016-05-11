//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPolarTransform
//* .File Name       : $Workfile:   SVPolarTransform.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:56:12  $
//******************************************************************************

#include "stdafx.h"
#include "SVPolarTransform.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"

SV_IMPLEMENT_CLASS( SVPolarTransformClass, SVPolarTransformClassGuid )

SVPolarTransformClass::SVPolarTransformClass( SVObjectClass* POwner, int StringResourceID )
				 :SVTaskObjectClass( POwner, StringResourceID ) 
{
	// Identify yourself
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVPolarTransformObjectType;

	// Identify our input type needs...

	// Center X
	inputCenterXResult.SetInputObjectType( SVOutputEvaluateCenterXResultObjectGuid, SVDoubleValueObjectType );
	inputCenterXResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputCenterXResult, _T( "PolarTransformCenterXResult" ) );

	// Center Y
	inputCenterYResult.SetInputObjectType( SVOutputEvaluateCenterYResultObjectGuid, SVDoubleValueObjectType );
	inputCenterYResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputCenterYResult, _T( "PolarTransformCenterYResult" ) );

	// Start Radius
	inputStartRadiusResult.SetInputObjectType( SVOutputEvaluateStartRadiusResultObjectGuid, SVDoubleValueObjectType );
	inputStartRadiusResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputStartRadiusResult, _T( "PolarTransformStartRadiusResult" ) );

	// End Radius
	inputEndRadiusResult.SetInputObjectType( SVOutputEvaluateEndRadiusResultObjectGuid, SVDoubleValueObjectType );
	inputEndRadiusResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputEndRadiusResult, _T( "PolarTransformEndRadiusResult" ) );
	
	// Start Angle
	inputStartAngleResult.SetInputObjectType( SVOutputEvaluateStartAngleResultObjectGuid, SVDoubleValueObjectType );
	inputStartAngleResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputStartAngleResult, _T( "PolarTransformStartAngleResult" ) );

	// End Angle
	inputEndAngleResult.SetInputObjectType( SVOutputEvaluateEndAngleResultObjectGuid, SVDoubleValueObjectType );
	inputEndAngleResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputEndAngleResult, _T( "PolarTransformEndAngleResult" ) );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVPolarTransformClass::~SVPolarTransformClass()
{
	CloseObject();
}

BOOL SVPolarTransformClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = false;

	if( SVTaskObjectClass::CreateObject( PCreateStructure ) )
	{
		bOk = true;
	}

	m_isCreated = bOk;

	return bOk;
}

BOOL SVPolarTransformClass::CloseObject()
{
	BOOL bRetVal = false;
	if( SVTaskObjectClass::CloseObject() )
	{
		bRetVal = true;
	}
	return bRetVal;
}

SVDoubleValueObjectClass* SVPolarTransformClass::getInputCenterXResult()
{
	if( inputCenterXResult.IsConnected() && inputCenterXResult.GetInputObjectInfo().PObject )
	{
		return ( SVDoubleValueObjectClass* ) inputCenterXResult.GetInputObjectInfo().PObject;
	}
	return nullptr;
}

SVDoubleValueObjectClass* SVPolarTransformClass::getInputCenterYResult()
{
	if( inputCenterYResult.IsConnected() && inputCenterYResult.GetInputObjectInfo().PObject )
	{
		return ( SVDoubleValueObjectClass* ) inputCenterYResult.GetInputObjectInfo().PObject;
	}
	return nullptr;
}

SVDoubleValueObjectClass* SVPolarTransformClass::getInputStartRadiusResult()
{
	if( inputStartRadiusResult.IsConnected() && inputStartRadiusResult.GetInputObjectInfo().PObject )
	{
		return ( SVDoubleValueObjectClass* ) inputStartRadiusResult.GetInputObjectInfo().PObject;
	}

	return nullptr;
}

SVDoubleValueObjectClass* SVPolarTransformClass::getInputEndRadiusResult()
{

	if( inputEndRadiusResult.IsConnected() && inputEndRadiusResult.GetInputObjectInfo().PObject )
	{
		return ( SVDoubleValueObjectClass* ) inputEndRadiusResult.GetInputObjectInfo().PObject;
	}
	return nullptr;
}

SVDoubleValueObjectClass* SVPolarTransformClass::getInputStartAngleResult()
{
	if( inputStartAngleResult.IsConnected() && inputStartAngleResult.GetInputObjectInfo().PObject )
	{
		return ( SVDoubleValueObjectClass* ) inputStartAngleResult.GetInputObjectInfo().PObject;
	}
	return nullptr;
}

SVDoubleValueObjectClass* SVPolarTransformClass::getInputEndAngleResult()
{
	if( inputEndAngleResult.IsConnected() && inputEndAngleResult.GetInputObjectInfo().PObject )
	{
		return ( SVDoubleValueObjectClass* ) inputEndAngleResult.GetInputObjectInfo().PObject;
	}
	return nullptr;
}

BOOL SVPolarTransformClass::OnValidate()
{
	BOOL bRetVal = false;
	if( getInputCenterXResult() &&
		getInputCenterYResult() &&
		getInputStartRadiusResult() &&
		getInputEndRadiusResult() &&
		getInputStartAngleResult() &&
		getInputEndAngleResult() )
	{
		bRetVal = true;
		bRetVal = SVTaskObjectClass::OnValidate() && bRetVal;
	}

	// Note: Make sure this is called when Validate fails !!!
	if( ! bRetVal )
	{
		SetInvalid();
	}
	return bRetVal;
}

DWORD_PTR SVPolarTransformClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = SVMR_NOT_PROCESSED;

	return( SVTaskObjectClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

