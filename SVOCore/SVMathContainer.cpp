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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVMathContainerObjectType;

	// Identify our input type needs...

	// Input Math Result...
	inputMathResult.SetInputObjectType( SVMathEquationResultObjectGuid, SVDoubleValueObjectType );
	inputMathResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputMathResult, _T( "MathResult" ) );

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

SVDoubleValueObjectClass* SVMathContainerClass::getInputMathResult()
{
	if( inputMathResult.IsConnected() && inputMathResult.GetInputObjectInfo().m_pObject )
		return ( SVDoubleValueObjectClass* ) inputMathResult.GetInputObjectInfo().m_pObject;

	return nullptr;
}
