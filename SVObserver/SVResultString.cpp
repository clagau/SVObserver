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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVResultStringObjectType;

	// Identify our input type needs
	m_inputObjectInfo.SetInputObjectType( SVStringValueObjectType );
	m_inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputObjectInfo, _T( "StringResultValue" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_Value, SVStringValueObjectClassGuid, IDS_OBJECTNAME_VALUE, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	m_Value.SetDefaultValue (_T(""), TRUE);
	m_Value.setSaveValueFlag(false);

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

	m_Value.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

BOOL SVStringResultClass::CloseObject()
{
	return SVResultClass::CloseObject();
}

SVStringValueObjectClass* SVStringResultClass::getInputString()
{
	if( m_inputObjectInfo.IsConnected() )
	{
		return dynamic_cast<SVStringValueObjectClass*> (m_inputObjectInfo.GetInputObjectInfo().m_pObject);
	}

	return nullptr;
}

bool SVStringResultClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	// All inputs and outputs must be validated first
	if( __super::onRun( rRunStatus, pErrorMessages ) )
	{
		SVStringValueObjectClass* pValueObject = getInputString();
		ASSERT( pValueObject );

		if( nullptr != pValueObject )
		{
			SVString Value;
			pValueObject->GetValue(Value);
			m_Value.SetValue(Value);
			return true;
		}
	}
	rRunStatus.SetInvalid();
	return false;
}

