//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTransform
//* .File Name       : $Workfile:   SVTransform.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 14:48:46  $
//******************************************************************************

#include "stdafx.h"
#include "SVTransform.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"

SV_IMPLEMENT_CLASS( SVTransformClass, SVTransformClassGuid )

SVTransformClass::SVTransformClass( SVObjectClass* POwner, int StringResourceID )
				 :SVTaskObjectClass( POwner, StringResourceID ) 
{
	// Identify yourself
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVTransformObjectType;

	// Identify our input type needs...
	// Translation X
	inputTranslationXResult.SetInputObjectType( SVOutputEvaluateTranslationXResultObjectGuid, SVDoubleValueObjectType );
	inputTranslationXResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputTranslationXResult, _T( "TransformTranslationXResult" ) );

	// Translation Y
	inputTranslationYResult.SetInputObjectType( SVOutputEvaluateTranslationYResultObjectGuid, SVDoubleValueObjectType );
	inputTranslationYResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputTranslationYResult, _T( "TransformTranslationYResult" ) );

	// Rotation X
	inputRotationXResult.SetInputObjectType( SVOutputEvaluateRotationXResultObjectGuid, SVDoubleValueObjectType );
	inputRotationXResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputRotationXResult, _T( "TransformRotationXResult" ) );

	// Rotation Y
	inputRotationYResult.SetInputObjectType( SVOutputEvaluateRotationYResultObjectGuid, SVDoubleValueObjectType );
	inputRotationYResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputRotationYResult, _T( "TransformRotationYResult" ) );

	// Rotation Angle
	inputRotationAngleResult.SetInputObjectType( SVOutputEvaluateRotationAngleResultObjectGuid, SVDoubleValueObjectType );
	inputRotationAngleResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputRotationAngleResult, _T( "TransformRotationAngleResult" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject( &performTranslation, SVPerformTranslationObjectGuid, IDS_OBJECTNAME_PERFORM_TRANSLATION, false, SVResetItemTool );
	RegisterEmbeddedObject( &performRotation, SVPerformRotationObjectGuid, IDS_OBJECTNAME_PERFORM_ROTATION, false, SVResetItemTool );

	RegisterEmbeddedObject( &learnedTranslationX, SVLearnedTranslationXObjectGuid, IDS_OBJECTNAME_LEARNED_TRANSLATION_X, false, SVResetItemTool );
	RegisterEmbeddedObject( &learnedTranslationY, SVLearnedTranslationYObjectGuid, IDS_OBJECTNAME_LEARNED_TRANSLATION_Y, false, SVResetItemTool );
	RegisterEmbeddedObject( &learnedRotationX, SVLearnedRotationXObjectGuid, IDS_OBJECTNAME_LEARNED_ROTATION_X, false, SVResetItemTool );
	RegisterEmbeddedObject( &learnedRotationY, SVLearnedRotationYObjectGuid, IDS_OBJECTNAME_LEARNED_ROTATION_Y, false, SVResetItemTool );
	RegisterEmbeddedObject( &learnedRotationAngle, SVLearnedRotationAngleObjectGuid, IDS_OBJECTNAME_LEARNED_ROTATION_ANGLE, false, SVResetItemTool );

	// Set Embedded defaults
	performTranslation.SetDefaultValue( FALSE, TRUE );
	performRotation.SetDefaultValue( FALSE, TRUE );

	learnedTranslationX.SetDefaultValue( 0.0, TRUE );
	learnedTranslationY.SetDefaultValue( 0.0, TRUE );
	learnedRotationX.SetDefaultValue( 0.0, TRUE );
	learnedRotationY.SetDefaultValue( 0.0, TRUE );
	learnedRotationAngle.SetDefaultValue( 0.0, TRUE );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVTransformClass::~SVTransformClass()
{
	CloseObject();
}

BOOL SVTransformClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if( SVTaskObjectClass::CreateObject( PCreateStructure ) )
	{
		bOk = TRUE;
	}

	performTranslation.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	performRotation.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	learnedTranslationX.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	learnedTranslationY.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	learnedRotationX.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	learnedRotationY.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	learnedRotationAngle.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	m_isCreated = bOk;

	return bOk;
}

BOOL SVTransformClass::CloseObject()
{
	if( SVTaskObjectClass::CloseObject() )
	{
		BOOL bRetVal = TRUE;
		return bRetVal;
	}
	return FALSE;
}

SVDoubleValueObjectClass* SVTransformClass::getInputTranslationXResult()
{
	if( inputTranslationXResult.IsConnected() && inputTranslationXResult.GetInputObjectInfo().PObject )
		return ( SVDoubleValueObjectClass* ) inputTranslationXResult.GetInputObjectInfo().PObject;

	return nullptr;
}

SVDoubleValueObjectClass* SVTransformClass::getInputTranslationYResult()
{
	if( inputTranslationYResult.IsConnected() && inputTranslationYResult.GetInputObjectInfo().PObject )
		return ( SVDoubleValueObjectClass* ) inputTranslationYResult.GetInputObjectInfo().PObject;

	return nullptr;
}

SVDoubleValueObjectClass* SVTransformClass::getInputRotationXResult()
{
	if( inputRotationXResult.IsConnected() && inputRotationXResult.GetInputObjectInfo().PObject )
		return ( SVDoubleValueObjectClass* ) inputRotationXResult.GetInputObjectInfo().PObject;

	return nullptr;
}

SVDoubleValueObjectClass* SVTransformClass::getInputRotationYResult()
{
	if( inputRotationYResult.IsConnected() && inputRotationYResult.GetInputObjectInfo().PObject )
		return ( SVDoubleValueObjectClass* ) inputRotationYResult.GetInputObjectInfo().PObject;

	return nullptr;
}


SVDoubleValueObjectClass* SVTransformClass::getInputRotationAngleResult()
{
	if( inputRotationAngleResult.IsConnected() && inputRotationAngleResult.GetInputObjectInfo().PObject )
		return ( SVDoubleValueObjectClass* ) inputRotationAngleResult.GetInputObjectInfo().PObject;

	return nullptr;
}

BOOL SVTransformClass::OnValidate()
{
	BOOL bRetVal = FALSE;
	if( getInputTranslationXResult() && getInputTranslationYResult() &&
		getInputRotationXResult()    && getInputRotationYResult() &&
		getInputRotationAngleResult() )
	{
		bRetVal = TRUE;
		bRetVal = SVTaskObjectClass::OnValidate() && bRetVal;
	}

	// Note: Make sure this is called when Validate fails !!!
	if( ! bRetVal )
		SetInvalid();

	return bRetVal;
}

DWORD_PTR SVTransformClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = SVMR_NOT_PROCESSED;

	return( SVTaskObjectClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

