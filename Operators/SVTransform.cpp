//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVTransformObjectType;

	// Identify our input type needs...
	// Translation X
	m_inputTranslationXResult.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SvPb::OutputEvaluateTranslationXResultEId);
	registerInputObject( &m_inputTranslationXResult, _T( "TransformTranslationXResult" ), SvPb::TranslationXResultInputEId);
	m_inputTranslationXResult.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;

	// Translation Y
	m_inputTranslationYResult.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SvPb::OutputEvaluateTranslationYResultEId);
	registerInputObject( &m_inputTranslationYResult, _T( "TransformTranslationYResult" ), SvPb::TranslationYResultInputEId);
	m_inputTranslationYResult.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;

	// Rotation X
	m_inputRotationXResult.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SvPb::OutputEvaluateRotationXResultEId);
	registerInputObject( &m_inputRotationXResult, _T( "TransformRotationXResult" ), SvPb::RotationXResultInputEId);
	m_inputRotationXResult.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;

	// Rotation Y
	m_inputRotationYResult.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SvPb::OutputEvaluateRotationYResultEId);
	registerInputObject( &m_inputRotationYResult, _T( "TransformRotationYResult" ), SvPb::RotationYResultInputEId);
	m_inputRotationYResult.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;

	// Rotation Angle
	m_inputRotationAngleResult.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SvPb::OutputEvaluateRotationAngleResultEId);
	registerInputObject( &m_inputRotationAngleResult, _T( "TransformRotationAngleResult" ), SvPb::RotationAngleResultInputEId);
	m_inputRotationAngleResult.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;

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
}

SVTransform::~SVTransform()
{
	SVTransform::CloseObject();
}

bool SVTransform::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVTaskObjectClass::CreateObject(rCreateStructure);

	m_performTranslation.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	m_performRotation.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );

	m_learnedTranslationX.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	m_learnedTranslationY.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	m_learnedRotationX.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	m_learnedRotationY.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	m_learnedRotationAngle.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );

	m_isCreated = bOk;

	return bOk;
}

bool SVTransform::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	std::initializer_list<SvOl::InputObject*> inputList
	{
		&m_inputTranslationXResult,
		&m_inputTranslationYResult,
		&m_inputRotationXResult,
		&m_inputRotationYResult,
		&m_inputRotationAngleResult
	};

	for (auto pEntry : inputList)
	{
		pEntry->validateInput();
		// pEntry cannot be nullptr as the InputList are member variable addresses
		if (nullptr == pEntry->getInput<SvVol::SVDoubleValueObjectClass>())
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
				pErrorMessages->push_back(Msg);
			}
			break;
		}
	}

	return Result;
}

} //namespace SvOp
