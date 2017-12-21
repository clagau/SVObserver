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

SV_IMPLEMENT_CLASS( SVPolarTransformClass, SVPolarTransformClassGuid )

SVPolarTransformClass::SVPolarTransformClass( SVObjectClass* POwner, int StringResourceID )
				 :SVTaskObjectClass( POwner, StringResourceID ) 
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVPolarTransformObjectType;

	// Identify our input type needs...

	// Center X
	m_inputCenterXResult.SetInputObjectType(SvDef::SVValueObjectType, SvDef::SVDoubleValueObjectType, SVOutputEvaluateCenterXResultObjectGuid);
	m_inputCenterXResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputCenterXResult, _T( "PolarTransformCenterXResult" ) );

	// Center Y
	m_inputCenterYResult.SetInputObjectType(SvDef::SVValueObjectType, SvDef::SVDoubleValueObjectType, SVOutputEvaluateCenterYResultObjectGuid);
	m_inputCenterYResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputCenterYResult, _T( "PolarTransformCenterYResult" ) );

	// Start Radius
	m_inputStartRadiusResult.SetInputObjectType(SvDef::SVValueObjectType, SvDef::SVDoubleValueObjectType, SVOutputEvaluateStartRadiusResultObjectGuid);
	m_inputStartRadiusResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputStartRadiusResult, _T( "PolarTransformStartRadiusResult" ) );

	// End Radius
	m_inputEndRadiusResult.SetInputObjectType(SvDef::SVValueObjectType, SvDef::SVDoubleValueObjectType, SVOutputEvaluateEndRadiusResultObjectGuid);
	m_inputEndRadiusResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputEndRadiusResult, _T( "PolarTransformEndRadiusResult" ) );
	
	// Start Angle
	m_inputStartAngleResult.SetInputObjectType(SvDef::SVValueObjectType, SvDef::SVDoubleValueObjectType, SVOutputEvaluateStartAngleResultObjectGuid);
	m_inputStartAngleResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputStartAngleResult, _T( "PolarTransformStartAngleResult" ) );

	// End Angle
	m_inputEndAngleResult.SetInputObjectType(SvDef::SVValueObjectType, SvDef::SVDoubleValueObjectType, SVOutputEvaluateEndAngleResultObjectGuid);
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
	bool Result = true;
	if( nullptr == getInputCenterXResult() || nullptr == getInputCenterYResult() ||
		 nullptr == getInputStartRadiusResult() || nullptr == getInputEndRadiusResult() ||
		 nullptr == getInputStartAngleResult() ||  nullptr == getInputEndAngleResult() )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}
	return __super::ResetObject(pErrorMessages) && Result;
}

SVDoubleValueObjectClass* SVPolarTransformClass::getInputCenterXResult()
{
	if( m_inputCenterXResult.IsConnected() && m_inputCenterXResult.GetInputObjectInfo().getObject() )
	{
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when getObject() is not nullptr that it is the correct type
		return static_cast<SVDoubleValueObjectClass*> (m_inputCenterXResult.GetInputObjectInfo().getObject());
	}
	return nullptr;
}

SVDoubleValueObjectClass* SVPolarTransformClass::getInputCenterYResult()
{
	if( m_inputCenterYResult.IsConnected() && m_inputCenterYResult.GetInputObjectInfo().getObject())
	{
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when getObject() is not nullptr that it is the correct type
		return static_cast<SVDoubleValueObjectClass*> (m_inputCenterYResult.GetInputObjectInfo().getObject());
	}
	return nullptr;
}

SVDoubleValueObjectClass* SVPolarTransformClass::getInputStartRadiusResult()
{
	if( m_inputStartRadiusResult.IsConnected() && m_inputStartRadiusResult.GetInputObjectInfo().getObject())
	{
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when getObject() is not nullptr that it is the correct type
		return static_cast<SVDoubleValueObjectClass*> (m_inputStartRadiusResult.GetInputObjectInfo().getObject());
	}

	return nullptr;
}

SVDoubleValueObjectClass* SVPolarTransformClass::getInputEndRadiusResult()
{

	if( m_inputEndRadiusResult.IsConnected() && m_inputEndRadiusResult.GetInputObjectInfo().getObject())
	{
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when getObject() is not nullptr that it is the correct type
		return static_cast<SVDoubleValueObjectClass*> (m_inputEndRadiusResult.GetInputObjectInfo().getObject());
	}
	return nullptr;
}

SVDoubleValueObjectClass* SVPolarTransformClass::getInputStartAngleResult()
{
	if( m_inputStartAngleResult.IsConnected() && m_inputStartAngleResult.GetInputObjectInfo().getObject())
	{
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when getObject() is not nullptr that it is the correct type
		return static_cast<SVDoubleValueObjectClass*> (m_inputStartAngleResult.GetInputObjectInfo().getObject());
	}
	return nullptr;
}

SVDoubleValueObjectClass* SVPolarTransformClass::getInputEndAngleResult()
{
	if( m_inputEndAngleResult.IsConnected() && m_inputEndAngleResult.GetInputObjectInfo().getObject())
	{
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when getObject() is not nullptr that it is the correct type
		return static_cast<SVDoubleValueObjectClass*> (m_inputEndAngleResult.GetInputObjectInfo().getObject());
	}
	return nullptr;
}
