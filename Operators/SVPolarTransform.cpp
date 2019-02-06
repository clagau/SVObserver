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
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVObjectLibrary/SVClsIds.h"

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVPolarTransformClass, SVPolarTransformClassGuid )

SVPolarTransformClass::SVPolarTransformClass( SVObjectClass* POwner, int StringResourceID )
				 :SVTaskObjectClass( POwner, StringResourceID ) 
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVPolarTransformObjectType;

	// Identify our input type needs...

	// Center X
	m_inputCenterXResult.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SVOutputEvaluateCenterXResultObjectGuid);
	m_inputCenterXResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputCenterXResult, _T( "PolarTransformCenterXResult" ) );

	// Center Y
	m_inputCenterYResult.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SVOutputEvaluateCenterYResultObjectGuid);
	m_inputCenterYResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputCenterYResult, _T( "PolarTransformCenterYResult" ) );

	// Start Radius
	m_inputStartRadiusResult.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SVOutputEvaluateStartRadiusResultObjectGuid);
	m_inputStartRadiusResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputStartRadiusResult, _T( "PolarTransformStartRadiusResult" ) );

	// End Radius
	m_inputEndRadiusResult.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SVOutputEvaluateEndRadiusResultObjectGuid);
	m_inputEndRadiusResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputEndRadiusResult, _T( "PolarTransformEndRadiusResult" ) );
	
	// Start Angle
	m_inputStartAngleResult.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SVOutputEvaluateStartAngleResultObjectGuid);
	m_inputStartAngleResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputStartAngleResult, _T( "PolarTransformStartAngleResult" ) );

	// End Angle
	m_inputEndAngleResult.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SVOutputEvaluateEndAngleResultObjectGuid);
	m_inputEndAngleResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputEndAngleResult, _T( "PolarTransformEndAngleResult" ) );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVPolarTransformClass::~SVPolarTransformClass()
{
	CloseObject();
}

bool SVPolarTransformClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	SvOl::SVInObjectInfoStructPtrVector InputList
	{
		&m_inputCenterXResult,
		&m_inputCenterYResult,
		&m_inputEndAngleResult,
		&m_inputEndRadiusResult,
		&m_inputStartAngleResult,
		&m_inputStartRadiusResult
	};

	SvOl::ValidateInputList(InputList);

	for(auto pEntry : InputList)
	{
		// pEntry cannot be nullptr as the InputList are member variable addresses
		if(nullptr == SvOl::getInput<SvVol::SVDoubleValueObjectClass>(*pEntry))
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			break;
		}
	}
	return Result;
}

} //namespace SvOp
