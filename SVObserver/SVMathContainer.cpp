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
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"

SV_IMPLEMENT_CLASS( SVMathContainerClass, SVMathContainerClassGuid )

SVMathContainerClass::SVMathContainerClass( SVObjectClass* POwner, int StringResourceID )
					 :SVTaskObjectClass( POwner, StringResourceID ) 
{
	// Identify yourself
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVMathContainerObjectType;

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

BOOL SVMathContainerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = false;

	if( SVTaskObjectClass::CreateObject( PCreateStructure ) &&
		PCreateStructure->CheckCreateStructName( "SVToolLevelCreateStruct" ) 
	  )
	{
		SVDoubleValueObjectClass*  pInputMathResult;

		bOk = nullptr != GetInspection() && nullptr != GetTool() && 
			  nullptr != ( pInputMathResult = getInputMathResult() );
	}

	m_isCreated = bOk;

	return bOk;
}

BOOL SVMathContainerClass::CloseObject()
{
	if( SVTaskObjectClass::CloseObject() )
	{
		BOOL bRetVal = TRUE;
		return bRetVal;
	}
	return FALSE;
}

SVDoubleValueObjectClass* SVMathContainerClass::getInputMathResult()
{
	if( inputMathResult.IsConnected() && inputMathResult.GetInputObjectInfo().PObject )
		return ( SVDoubleValueObjectClass* ) inputMathResult.GetInputObjectInfo().PObject;

	return nullptr;
}

BOOL SVMathContainerClass::OnValidate()
{
	BOOL bRetVal = FALSE;
	if( getInputMathResult() )
	{
		bRetVal = TRUE;
		bRetVal = SVTaskObjectClass::OnValidate() && bRetVal;
	}

	// Note: Make sure this is called when Validate fails !!!
	if( ! bRetVal )
		SetInvalid();

	return bRetVal;
}

