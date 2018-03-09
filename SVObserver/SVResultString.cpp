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

#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVStringResultClass, SVStringResultClassGuid );

SVStringResultClass::SVStringResultClass( SVObjectClass* POwner, int StringResourceID )
				  :SVResultClass( POwner, StringResourceID )
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVResultObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvDef::SVResultStringObjectType;

	// Identify our input type needs
	m_inputObjectInfo.SetInputObjectType(SvDef::SVValueObjectType, SvDef::SVStringValueObjectType );
	m_inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputObjectInfo, _T( "StringResultValue" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_Value, SVStringValueObjectClassGuid, IDS_OBJECTNAME_VALUE, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	m_Value.SetDefaultValue (_T(""));
	m_Value.setSaveValueFlag(false);

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVStringResultClass::~SVStringResultClass()
{

}

bool SVStringResultClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVResultClass::CreateObject(rCreateStructure) && nullptr != getInputString();

	m_Value.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

SVStringValueObjectClass* SVStringResultClass::getInputString(bool bRunMode /*= false*/)
{
	if( m_inputObjectInfo.IsConnected() && nullptr != m_inputObjectInfo.GetInputObjectInfo().getObject())
	{
		SVObjectClass* pObject = m_inputObjectInfo.GetInputObjectInfo().getObject();
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when getObject() is not nullptr that it is the correct type
		return bRunMode ? static_cast<SVStringValueObjectClass*> (pObject) : dynamic_cast<SVStringValueObjectClass*> (pObject);
	}

	return nullptr;
}

bool SVStringResultClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	// All inputs and outputs must be validated first
	if( __super::onRun( rRunStatus, pErrorMessages ) )
	{
		SVStringValueObjectClass* pValueObject = getInputString(true);
		ASSERT( pValueObject );

		if( nullptr != pValueObject )
		{
			std::string Value;
			pValueObject->GetValue(Value);
			m_Value.SetValue(Value);
			return true;
		}
	}
	rRunStatus.SetInvalid();
	return false;
}

