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

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

///For this class it is not necessary to call SV_IMPLEMENT_CLASS as it is a base class and only derived classes are instantiated.
//SV_IMPLEMENT_CLASS( SVTransform, SVTransformClassGuid )

SVTransform::SVTransform( SVObjectClass* POwner, int StringResourceID )
				 :SVTaskObjectClass( POwner, StringResourceID ) 
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVTransformObjectType;

	// Identify our input type needs...
	// Translation X
	m_inputTranslationXResult.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SvPb::OutputEvaluateTranslationXResultEId);
	m_inputTranslationXResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputTranslationXResult, _T( "TransformTranslationXResult" ) );

	// Translation Y
	m_inputTranslationYResult.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SvPb::OutputEvaluateTranslationYResultEId);
	m_inputTranslationYResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputTranslationYResult, _T( "TransformTranslationYResult" ) );

	// Rotation X
	m_inputRotationXResult.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SvPb::OutputEvaluateRotationXResultEId);
	m_inputRotationXResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputRotationXResult, _T( "TransformRotationXResult" ) );

	// Rotation Y
	m_inputRotationYResult.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SvPb::OutputEvaluateRotationYResultEId);
	m_inputRotationYResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputRotationYResult, _T( "TransformRotationYResult" ) );

	// Rotation Angle
	m_inputRotationAngleResult.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SvPb::OutputEvaluateRotationAngleResultEId);
	m_inputRotationAngleResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputRotationAngleResult, _T( "TransformRotationAngleResult" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_performTranslation, SvPb::PerformTranslationEId, IDS_OBJECTNAME_PERFORM_TRANSLATION, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_performRotation, SvPb::PerformRotationEId, IDS_OBJECTNAME_PERFORM_ROTATION, false, SvOi::SVResetItemTool );

	RegisterEmbeddedObject( &m_learnedTranslationX, SvPb::LearnedTranslationXEId, IDS_OBJECTNAME_LEARNED_TRANSLATION_X, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_learnedTranslationY, SvPb::LearnedTranslationYEId, IDS_OBJECTNAME_LEARNED_TRANSLATION_Y, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_learnedRotationX, SvPb::LearnedRotationXEId, IDS_OBJECTNAME_LEARNED_ROTATION_X, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_learnedRotationY, SvPb::LearnedRotationYEId, IDS_OBJECTNAME_LEARNED_ROTATION_Y, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_learnedRotationAngle, SvPb::LearnedRotationAngleEId, IDS_OBJECTNAME_LEARNED_ROTATION_ANGLE, false, SvOi::SVResetItemTool );

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

SVTransform::~SVTransform()
{
	SVTransform::CloseObject();
}

bool SVTransform::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVTaskObjectClass::CreateObject(rCreateStructure);

	m_performTranslation.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	m_performRotation.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );

	m_learnedTranslationX.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	m_learnedTranslationY.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	m_learnedRotationX.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	m_learnedRotationY.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	m_learnedRotationAngle.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );

	m_isCreated = bOk;

	return bOk;
}

bool SVTransform::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	SvOl::SVInObjectInfoStructPtrVector InputList
	{
		&m_inputTranslationXResult,
		&m_inputTranslationYResult,
		&m_inputRotationXResult,
		&m_inputRotationYResult,
		&m_inputRotationAngleResult
	};

	SvOl::ValidateInputList(InputList);

	for (auto pEntry : InputList)
	{
		// pEntry cannot be nullptr as the InputList are member variable addresses
		if (nullptr == SvOl::getInput<SvVol::SVDoubleValueObjectClass>(*pEntry))
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
				pErrorMessages->push_back(Msg);
			}
			break;
		}
	}

	return Result;
}

} //namespace SvOp
