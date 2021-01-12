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
#include "SVStatusLibrary/RunStatus.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVStringResult, SvPb::StringResultClassId);

SVStringResult::SVStringResult( SVObjectClass* POwner, int StringResourceID )
				  :SVResult( POwner, StringResourceID )
{
	// Identify yourself
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVResultObjectType;
	m_ObjectTypeInfo.m_SubType = SvPb::SVResultStringObjectType;

	// Identify our input type needs
	m_inputObject.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVStringValueObjectType );
	registerInputObject( &m_inputObject, _T( "StringResultValue" ), SvPb::ResultInputEId);
	m_inputObject.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_Value, SvPb::StringValueEId, IDS_OBJECTNAME_VALUE, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	m_Value.SetDefaultValue (_T(""));
	m_Value.setSaveValueFlag(false);
	m_Value.SetMaxByteSize();
}

SVStringResult::~SVStringResult()
{

}

bool SVStringResult::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVResult::CreateObject(rCreateStructure) && nullptr != m_inputObject.getInput<SvVol::SVStringValueObjectClass>();

	m_Value.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );


	m_isCreated = bOk;

	return bOk;
}

bool SVStringResult::onRun( RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	// All inputs and outputs must be validated first
	if( __super::onRun( rRunStatus, pErrorMessages ) )
	{
		SvVol::SVStringValueObjectClass* pValueObject = m_inputObject.getInput<SvVol::SVStringValueObjectClass>(true);
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
