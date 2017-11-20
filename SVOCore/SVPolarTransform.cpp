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
	inputCenterXResult.SetInputObjectType( SVOutputEvaluateCenterXResultObjectGuid, SvDef::SVValueObjectType, SvDef::SVDoubleValueObjectType );
	inputCenterXResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputCenterXResult, _T( "PolarTransformCenterXResult" ) );

	// Center Y
	inputCenterYResult.SetInputObjectType( SVOutputEvaluateCenterYResultObjectGuid, SvDef::SVValueObjectType, SvDef::SVDoubleValueObjectType );
	inputCenterYResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputCenterYResult, _T( "PolarTransformCenterYResult" ) );

	// Start Radius
	inputStartRadiusResult.SetInputObjectType( SVOutputEvaluateStartRadiusResultObjectGuid, SvDef::SVValueObjectType, SvDef::SVDoubleValueObjectType );
	inputStartRadiusResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputStartRadiusResult, _T( "PolarTransformStartRadiusResult" ) );

	// End Radius
	inputEndRadiusResult.SetInputObjectType( SVOutputEvaluateEndRadiusResultObjectGuid, SvDef::SVValueObjectType, SvDef::SVDoubleValueObjectType );
	inputEndRadiusResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputEndRadiusResult, _T( "PolarTransformEndRadiusResult" ) );
	
	// Start Angle
	inputStartAngleResult.SetInputObjectType( SVOutputEvaluateStartAngleResultObjectGuid, SvDef::SVValueObjectType, SvDef::SVDoubleValueObjectType );
	inputStartAngleResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputStartAngleResult, _T( "PolarTransformStartAngleResult" ) );

	// End Angle
	inputEndAngleResult.SetInputObjectType( SVOutputEvaluateEndAngleResultObjectGuid, SvDef::SVValueObjectType, SvDef::SVDoubleValueObjectType );
	inputEndAngleResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputEndAngleResult, _T( "PolarTransformEndAngleResult" ) );

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
	if( inputCenterXResult.IsConnected() && inputCenterXResult.GetInputObjectInfo().m_pObject )
	{
		return ( SVDoubleValueObjectClass* ) inputCenterXResult.GetInputObjectInfo().m_pObject;
	}
	return nullptr;
}

SVDoubleValueObjectClass* SVPolarTransformClass::getInputCenterYResult()
{
	if( inputCenterYResult.IsConnected() && inputCenterYResult.GetInputObjectInfo().m_pObject )
	{
		return ( SVDoubleValueObjectClass* ) inputCenterYResult.GetInputObjectInfo().m_pObject;
	}
	return nullptr;
}

SVDoubleValueObjectClass* SVPolarTransformClass::getInputStartRadiusResult()
{
	if( inputStartRadiusResult.IsConnected() && inputStartRadiusResult.GetInputObjectInfo().m_pObject )
	{
		return ( SVDoubleValueObjectClass* ) inputStartRadiusResult.GetInputObjectInfo().m_pObject;
	}

	return nullptr;
}

SVDoubleValueObjectClass* SVPolarTransformClass::getInputEndRadiusResult()
{

	if( inputEndRadiusResult.IsConnected() && inputEndRadiusResult.GetInputObjectInfo().m_pObject )
	{
		return ( SVDoubleValueObjectClass* ) inputEndRadiusResult.GetInputObjectInfo().m_pObject;
	}
	return nullptr;
}

SVDoubleValueObjectClass* SVPolarTransformClass::getInputStartAngleResult()
{
	if( inputStartAngleResult.IsConnected() && inputStartAngleResult.GetInputObjectInfo().m_pObject )
	{
		return ( SVDoubleValueObjectClass* ) inputStartAngleResult.GetInputObjectInfo().m_pObject;
	}
	return nullptr;
}

SVDoubleValueObjectClass* SVPolarTransformClass::getInputEndAngleResult()
{
	if( inputEndAngleResult.IsConnected() && inputEndAngleResult.GetInputObjectInfo().m_pObject )
	{
		return ( SVDoubleValueObjectClass* ) inputEndAngleResult.GetInputObjectInfo().m_pObject;
	}
	return nullptr;
}
