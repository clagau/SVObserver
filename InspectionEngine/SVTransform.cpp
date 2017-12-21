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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVTransformObjectType;

	// Identify our input type needs...
	// Translation X
	m_inputTranslationXResult.SetInputObjectType(SvDef::SVValueObjectType, SvDef::SVDoubleValueObjectType, SVOutputEvaluateTranslationXResultObjectGuid);
	m_inputTranslationXResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputTranslationXResult, _T( "TransformTranslationXResult" ) );

	// Translation Y
	m_inputTranslationYResult.SetInputObjectType(SvDef::SVValueObjectType, SvDef::SVDoubleValueObjectType, SVOutputEvaluateTranslationYResultObjectGuid);
	m_inputTranslationYResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputTranslationYResult, _T( "TransformTranslationYResult" ) );

	// Rotation X
	m_inputRotationXResult.SetInputObjectType(SvDef::SVValueObjectType, SvDef::SVDoubleValueObjectType, SVOutputEvaluateRotationXResultObjectGuid);
	m_inputRotationXResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputRotationXResult, _T( "TransformRotationXResult" ) );

	// Rotation Y
	m_inputRotationYResult.SetInputObjectType(SvDef::SVValueObjectType, SvDef::SVDoubleValueObjectType, SVOutputEvaluateRotationYResultObjectGuid);
	m_inputRotationYResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputRotationYResult, _T( "TransformRotationYResult" ) );

	// Rotation Angle
	m_inputRotationAngleResult.SetInputObjectType(SvDef::SVValueObjectType, SvDef::SVDoubleValueObjectType, SVOutputEvaluateRotationAngleResultObjectGuid);
	m_inputRotationAngleResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputRotationAngleResult, _T( "TransformRotationAngleResult" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_performTranslation, SVPerformTranslationObjectGuid, IDS_OBJECTNAME_PERFORM_TRANSLATION, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_performRotation, SVPerformRotationObjectGuid, IDS_OBJECTNAME_PERFORM_ROTATION, false, SvOi::SVResetItemTool );

	RegisterEmbeddedObject( &m_learnedTranslationX, SVLearnedTranslationXObjectGuid, IDS_OBJECTNAME_LEARNED_TRANSLATION_X, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_learnedTranslationY, SVLearnedTranslationYObjectGuid, IDS_OBJECTNAME_LEARNED_TRANSLATION_Y, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_learnedRotationX, SVLearnedRotationXObjectGuid, IDS_OBJECTNAME_LEARNED_ROTATION_X, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_learnedRotationY, SVLearnedRotationYObjectGuid, IDS_OBJECTNAME_LEARNED_ROTATION_Y, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_learnedRotationAngle, SVLearnedRotationAngleObjectGuid, IDS_OBJECTNAME_LEARNED_ROTATION_ANGLE, false, SvOi::SVResetItemTool );

	// Set Embedded defaults
	m_performTranslation.SetDefaultValue( BOOL(false) );
	m_performRotation.SetDefaultValue( BOOL(false) );

	m_learnedTranslationX.SetDefaultValue( 0.0 );
	m_learnedTranslationY.SetDefaultValue( 0.0 );
	m_learnedRotationX.SetDefaultValue( 0.0 );
	m_learnedRotationY.SetDefaultValue( 0.0 );
	m_learnedRotationAngle.SetDefaultValue( 0.0 );

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

	m_performTranslation.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_performRotation.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );

	m_learnedTranslationX.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_learnedTranslationY.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_learnedRotationX.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_learnedRotationY.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_learnedRotationAngle.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );

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
	if (m_inputTranslationXResult.IsConnected() && m_inputTranslationXResult.GetInputObjectInfo().getObject())
	{
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when getObject() is not nullptr that it is the correct type
		return static_cast<SVDoubleValueObjectClass*> (m_inputTranslationXResult.GetInputObjectInfo().getObject());
	}

	return nullptr;
}

SVDoubleValueObjectClass* SVTransformClass::getInputTranslationYResult()
{
	if (m_inputTranslationYResult.IsConnected() && m_inputTranslationYResult.GetInputObjectInfo().getObject())
	{
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when getObject() is not nullptr that it is the correct type
		return static_cast<SVDoubleValueObjectClass*> (m_inputTranslationYResult.GetInputObjectInfo().getObject());
	}

	return nullptr;
}

SVDoubleValueObjectClass* SVTransformClass::getInputRotationXResult()
{
	if (m_inputRotationXResult.IsConnected() && m_inputRotationXResult.GetInputObjectInfo().getObject())
	{
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when getObject() is not nullptr that it is the correct type
		return static_cast<SVDoubleValueObjectClass*> (m_inputRotationXResult.GetInputObjectInfo().getObject());
	}

	return nullptr;
}

SVDoubleValueObjectClass* SVTransformClass::getInputRotationYResult()
{
	if (m_inputRotationYResult.IsConnected() && m_inputRotationYResult.GetInputObjectInfo().getObject())
	{
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when getObject() is not nullptr that it is the correct type
		return static_cast<SVDoubleValueObjectClass*> (m_inputRotationYResult.GetInputObjectInfo().getObject());
	}

	return nullptr;
}


SVDoubleValueObjectClass* SVTransformClass::getInputRotationAngleResult()
{
	if (m_inputRotationAngleResult.IsConnected() && m_inputRotationAngleResult.GetInputObjectInfo().getObject())
	{
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when getObject() is not nullptr that it is the correct type
		return static_cast<SVDoubleValueObjectClass*> (m_inputRotationAngleResult.GetInputObjectInfo().getObject());
	}

	return nullptr;
}
