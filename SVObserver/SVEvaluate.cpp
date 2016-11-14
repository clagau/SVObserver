//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEvaluate
//* .File Name       : $Workfile:   SVEvaluate.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:21:38  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVEvaluate.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVEvaluateClass, SVEvaluateClassGuid )

SVEvaluateClass::SVEvaluateClass( SVObjectClass* POwner, int StringResourceID )
: SVMathContainerClass( POwner, StringResourceID ) 
{
	// Identify yourself
	m_outObjectInfo.ObjectTypeInfo.SubType = SVEvaluateObjectType;

	// Identify our input type needs...

	// Register Embedded Objects

	// Set Embedded defaults

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVEvaluateClass::~SVEvaluateClass()
{
	CloseObject();
}

BOOL SVEvaluateClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	m_isCreated = SVMathContainerClass::CreateObject( PCreateStructure );

	// Set / Reset Printable flag
	m_outputMathResult.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	return m_isCreated;
}

BOOL SVEvaluateClass::CloseObject()
{
	BOOL bRetVal = false;
	if( SVMathContainerClass::CloseObject() )
	{
		bRetVal = true;
	}
	return bRetVal;
}

SVDoubleValueObjectClass* SVEvaluateClass::getOutputMathResult()
{
	return &m_outputMathResult;
}

BOOL SVEvaluateClass::onRun( SVRunStatusClass& RRunStatus )
{
	SVDoubleValueObjectClass* pResult = getOutputMathResult();
	ASSERT( pResult );
	
	// All inputs and outputs must be validated first
	if( SVMathContainerClass::onRun( RRunStatus ) )
	{
		SVDoubleValueObjectClass* pInputResult = getInputMathResult();
		ASSERT( pInputResult );

		double result;
		if( S_OK != pInputResult->GetValue( result ) )
		{
			SetInvalid();
			RRunStatus.SetInvalid();
			return false;
		}

		if( S_OK != pResult->SetValue( RRunStatus.m_lResultDataIndex, result ) )
		{
			SetInvalid();
			RRunStatus.SetInvalid();
			return false;
		}

		return true;
	}
	return false;
}