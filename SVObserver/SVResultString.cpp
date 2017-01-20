//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultString
//* .File Name       : $Workfile:   SVResultString.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:57:54  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVResultString.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVStringResultClass, SVStringResultClassGuid );

SVStringResultClass::SVStringResultClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
				  :SVResultClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	// Identify yourself
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType = SVResultStringObjectType;

	// Identify our input type needs
	m_SVInputStringObjectInfo.SetInputObjectType( SVStringValueObjectType );
	m_SVInputStringObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_SVInputStringObjectInfo, _T( "StringResultValue" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_szValue, SVStringValueObjectClassGuid, IDS_OBJECTNAME_VALUE, false, SVResetItemNone );

	// Set Embedded defaults
	m_szValue.SetDefaultValue (_T(""), TRUE);

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVStringResultClass::~SVStringResultClass()
{

}

BOOL SVStringResultClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = false;

	if( SVResultClass::CreateObject( PCreateStructure ) )
	{
		bOk = nullptr != getInputString();
	}

	m_szValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_isCreated = bOk;

	return bOk;
}

BOOL SVStringResultClass::CloseObject()
{
	return SVResultClass::CloseObject();
}

SVStringValueObjectClass* SVStringResultClass::getInputString()
{
	if( m_SVInputStringObjectInfo.IsConnected() && m_SVInputStringObjectInfo.GetInputObjectInfo().PObject )
		return ( SVStringValueObjectClass* ) m_SVInputStringObjectInfo.GetInputObjectInfo().PObject;

	return nullptr;
}

BOOL SVStringResultClass::OnValidate()
{
	BOOL bRetVal = FALSE;
	if( m_SVInputStringObjectInfo.IsConnected() &&
		m_SVInputStringObjectInfo.GetInputObjectInfo().PObject )
	{
		bRetVal = SVResultClass::OnValidate();
	}

	// Note: Make sure this is called when Validate fails !!!
	if( ! bRetVal )
		SetInvalid();

	return bRetVal;
}

BOOL SVStringResultClass::onRun( SVRunStatusClass& RRunStatus )
{
	// All inputs and outputs must be validated first
	if( SVResultClass::onRun( RRunStatus ) )
	{
		SVStringValueObjectClass* pValue = getInputString();
		ASSERT( pValue );

		SVString Value;
		pValue->GetValue( Value );

		// Set Value
		m_szValue.SetValue( RRunStatus.m_lResultDataIndex, Value );
		
		return TRUE;
  }
	RRunStatus.SetInvalid();
	return FALSE;
}

