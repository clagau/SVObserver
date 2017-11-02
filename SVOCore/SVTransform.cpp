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
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVObjectLibrary/SVClsIds.h"

SV_IMPLEMENT_CLASS( SVTransformClass, SVTransformClassGuid )

SVTransformClass::SVTransformClass( SVObjectClass* POwner, int StringResourceID )
				 :SVTaskObjectClass( POwner, StringResourceID ) 
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVTransformObjectType;

	// Identify our input type needs...
	// Translation X
	inputTranslationXResult.SetInputObjectType( SVOutputEvaluateTranslationXResultObjectGuid, SVValueObjectType, SVDoubleValueObjectType );
	inputTranslationXResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputTranslationXResult, _T( "TransformTranslationXResult" ) );

	// Translation Y
	inputTranslationYResult.SetInputObjectType( SVOutputEvaluateTranslationYResultObjectGuid, SVValueObjectType, SVDoubleValueObjectType );
	inputTranslationYResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputTranslationYResult, _T( "TransformTranslationYResult" ) );

	// Rotation X
	inputRotationXResult.SetInputObjectType( SVOutputEvaluateRotationXResultObjectGuid, SVValueObjectType, SVDoubleValueObjectType );
	inputRotationXResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputRotationXResult, _T( "TransformRotationXResult" ) );

	// Rotation Y
	inputRotationYResult.SetInputObjectType( SVOutputEvaluateRotationYResultObjectGuid, SVValueObjectType, SVDoubleValueObjectType );
	inputRotationYResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputRotationYResult, _T( "TransformRotationYResult" ) );

	// Rotation Angle
	inputRotationAngleResult.SetInputObjectType( SVOutputEvaluateRotationAngleResultObjectGuid, SVValueObjectType, SVDoubleValueObjectType );
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
	performTranslation.SetDefaultValue( BOOL(false) );
	performRotation.SetDefaultValue( BOOL(false) );

	learnedTranslationX.SetDefaultValue( 0.0 );
	learnedTranslationY.SetDefaultValue( 0.0 );
	learnedRotationX.SetDefaultValue( 0.0 );
	learnedRotationY.SetDefaultValue( 0.0 );
	learnedRotationAngle.SetDefaultValue( 0.0 );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVTransformClass::~SVTransformClass()
{
	CloseObject();
}

bool SVTransformClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVTaskObjectClass::CreateObject(rCreateStructure);

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
