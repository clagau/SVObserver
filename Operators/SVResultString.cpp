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
#include "SVStatusLibrary/SVRunStatus.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVStringResultClass, SvPb::StringResultClassId);

SVStringResultClass::SVStringResultClass( SVObjectClass* POwner, int StringResourceID )
				  :SVResultClass( POwner, StringResourceID )
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVResultObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVResultStringObjectType;

	// Identify our input type needs
	m_inputObjectInfo.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVStringValueObjectType );
	m_inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputObjectInfo, _T( "StringResultValue" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_Value, SvPb::StringValueEId, IDS_OBJECTNAME_VALUE, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	m_Value.SetDefaultValue (_T(""));
	m_Value.setSaveValueFlag(false);
	m_Value.SetMaxByteSize();

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVStringResultClass::~SVStringResultClass()
{

}

bool SVStringResultClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVResultClass::CreateObject(rCreateStructure) && nullptr != SvOl::getInput<SvVol::SVStringValueObjectClass>(m_inputObjectInfo);

	m_Value.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );


	m_isCreated = bOk;

	return bOk;
}

bool SVStringResultClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	// All inputs and outputs must be validated first
	if( __super::onRun( rRunStatus, pErrorMessages ) )
	{
		SvVol::SVStringValueObjectClass* pValueObject = SvOl::getInput<SvVol::SVStringValueObjectClass>(m_inputObjectInfo, true);
		assert( pValueObject );

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

} //namespace SvOp
