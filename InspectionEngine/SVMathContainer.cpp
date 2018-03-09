//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMathContainer
//* .File Name       : $Workfile:   SVMathContainer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:29:06  $
//******************************************************************************

#include "stdafx.h"
#include "SVMathContainer.h"
#include "SVMathEquation.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVObjectLibrary/SVClsids.h"


SV_IMPLEMENT_CLASS( SVMathContainerClass, SVMathContainerClassGuid )

SVMathContainerClass::SVMathContainerClass( SVObjectClass* POwner, int StringResourceID )
					 :SVTaskObjectClass( POwner, StringResourceID ) 
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVMathContainerObjectType;

	// Identify our input type needs...

	// Input Math Result...
	m_inputMathResult.SetInputObjectType(SvDef::SVValueObjectType, SvDef::SVDoubleValueObjectType, SVMathEquationResultObjectGuid);
	m_inputMathResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputMathResult, _T( "MathResult" ) );

	// Register Embedded Objects

	// Set Embedded defaults

	// Make a friend
	SVMathEquationClass* pMathEquation = new SVMathEquationClass( this );
	AddFriend( pMathEquation->GetUniqueObjectID() );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVMathContainerClass::~SVMathContainerClass()
{
	CloseObject();
}

bool SVMathContainerClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	m_isCreated = SVTaskObjectClass::CreateObject(rCreateStructure ) && nullptr != GetInspection() && 
		nullptr != GetTool() && nullptr != getInputMathResult();
	return m_isCreated;
}

bool SVMathContainerClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Valid = __super::ResetObject(pErrorMessages);

	// Check if the input object is still valid otherwise the pointer is invalid
	if (m_inputMathResult.IsConnected() && !m_inputMathResult.GetInputObjectInfo().CheckExistence())
	{
		m_inputMathResult.SetInputObject(nullptr);
	}

	if( nullptr == getInputMathResult() )
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

SVDoubleValueObjectClass* SVMathContainerClass::getInputMathResult(bool bRunMode /*= false*/)
{
	if (m_inputMathResult.IsConnected() && m_inputMathResult.GetInputObjectInfo().getObject())
	{
		SVObjectClass* pObject = m_inputMathResult.GetInputObjectInfo().getObject();
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when getObject() is not nullptr that it is the correct type
		return bRunMode ? static_cast<SVDoubleValueObjectClass*> (pObject) : dynamic_cast<SVDoubleValueObjectClass*> (pObject);
	}

	return nullptr;
}
