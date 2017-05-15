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
#include "SVObjectLibrary/SVClsIds.h"

SV_IMPLEMENT_CLASS( SVTransformClass, SVTransformClassGuid )

SVTransformClass::SVTransformClass( SVObjectClass* POwner, int StringResourceID )
				 :SVTaskObjectClass( POwner, StringResourceID ) 
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVTransformObjectType;

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
	RegisterEmbeddedObject( &performTranslation, SVPerformTranslationObjectGuid, IDS_OBJECTNAME_PERFORM_TRANSLATION, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &performRotation, SVPerformRotationObjectGuid, IDS_OBJECTNAME_PERFORM_ROTATION, false, SvOi::SVResetItemTool );

	RegisterEmbeddedObject( &learnedTranslationX, SVLearnedTranslationXObjectGuid, IDS_OBJECTNAME_LEARNED_TRANSLATION_X, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &learnedTranslationY, SVLearnedTranslationYObjectGuid, IDS_OBJECTNAME_LEARNED_TRANSLATION_Y, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &learnedRotationX, SVLearnedRotationXObjectGuid, IDS_OBJECTNAME_LEARNED_ROTATION_X, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &learnedRotationY, SVLearnedRotationYObjectGuid, IDS_OBJECTNAME_LEARNED_ROTATION_Y, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &learnedRotationAngle, SVLearnedRotationAngleObjectGuid, IDS_OBJECTNAME_LEARNED_ROTATION_ANGLE, false, SvOi::SVResetItemTool );

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

	performTranslation.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	performRotation.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );

	learnedTranslationX.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	learnedTranslationY.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	learnedRotationX.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	learnedRotationY.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	learnedRotationAngle.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );

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

bool SVTransformClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Valid = __super::ResetObject(pErrorMessages);

	if( nullptr == getInputTranslationXResult() || nullptr == getInputTranslationYResult() ||
		nullptr == getInputRotationXResult()    || nullptr == getInputRotationYResult() ||
		nullptr == getInputRotationAngleResult() )
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		Valid = false;
	}

	return Valid;
}

SVDoubleValueObjectClass* SVTransformClass::getInputTranslationXResult()
{
	if( inputTranslationXResult.IsConnected() && inputTranslationXResult.GetInputObjectInfo().m_pObject )
		return ( SVDoubleValueObjectClass* ) inputTranslationXResult.GetInputObjectInfo().m_pObject;

	return nullptr;
}

SVDoubleValueObjectClass* SVTransformClass::getInputTranslationYResult()
{
	if( inputTranslationYResult.IsConnected() && inputTranslationYResult.GetInputObjectInfo().m_pObject )
		return ( SVDoubleValueObjectClass* ) inputTranslationYResult.GetInputObjectInfo().m_pObject;

	return nullptr;
}

SVDoubleValueObjectClass* SVTransformClass::getInputRotationXResult()
{
	if( inputRotationXResult.IsConnected() && inputRotationXResult.GetInputObjectInfo().m_pObject )
		return ( SVDoubleValueObjectClass* ) inputRotationXResult.GetInputObjectInfo().m_pObject;

	return nullptr;
}

SVDoubleValueObjectClass* SVTransformClass::getInputRotationYResult()
{
	if( inputRotationYResult.IsConnected() && inputRotationYResult.GetInputObjectInfo().m_pObject )
		return ( SVDoubleValueObjectClass* ) inputRotationYResult.GetInputObjectInfo().m_pObject;

	return nullptr;
}


SVDoubleValueObjectClass* SVTransformClass::getInputRotationAngleResult()
{
	if( inputRotationAngleResult.IsConnected() && inputRotationAngleResult.GetInputObjectInfo().m_pObject )
		return ( SVDoubleValueObjectClass* ) inputRotationAngleResult.GetInputObjectInfo().m_pObject;

	return nullptr;
}
