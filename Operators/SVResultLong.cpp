//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultLong
//* .File Name       : $Workfile:   SVResultLong.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:56:40  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVResultLong.h"
#include "SVRange.h"
#include "Definitions/TextDefineSvDef.h"
#include "SVStatusLibrary/RunStatus.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVLongResult, SvPb::LongResultClassId);

SVLongResult::SVLongResult( SVObjectClass* POwner, int StringResourceID )
				  :SVResult( POwner, StringResourceID )
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVResultObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVResultLongObjectType;

	// Identify our input type needs
	m_inputObjectInfo.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVLongValueObjectType );
	m_inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputObjectInfo, SvDef::cInputTag_LongResultValue );

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_Value, SvPb::ValueEId, IDS_OBJECTNAME_VALUE, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	m_Value.SetDefaultValue( 0, true );
	m_Value.setSaveValueFlag(false);

	// Instantiate Dynamic Objects

	// Construct the SVRange via the ClassInfoStruct
	SvIe::SVClassInfoStruct rangeClassInfo;
	SvDef::SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of the SVRange...
	interfaceInfo.m_ObjectType = SvPb::SVValueObjectType;
	interfaceInfo.m_SubType = SvPb::SVLongValueObjectType;
	interfaceInfo.m_EmbeddedID = SvPb::ValueEId;
	rangeClassInfo.m_DesiredInputVector.push_back( interfaceInfo );

	// Describe the SVRange ...
	rangeClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVRangeObjectType;
	rangeClassInfo.m_ObjectTypeInfo.m_SubType	= SvPb::SVNotSetSubObjectType;
	rangeClassInfo.m_ClassId = SvPb::RangeClassId;
	rangeClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVRANGE );

	// Construct it
	SVRange* pRange = dynamic_cast<SVRange*>(rangeClassInfo.Construct()); 
	if (pRange)
	{
		Add(pRange);
	}

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVLongResult::~SVLongResult()
{

}

bool SVLongResult::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVResult::CreateObject(rCreateStructure) && nullptr != getInput();
	
	m_Value.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

SvPb::EmbeddedIdEnum SVLongResult::GetInputEmbeddedID() const
{
	if (nullptr != getInput() && SvPb::SVLongValueObjectType == getInput()->GetObjectSubType())
	{
		return getInput()->GetEmbeddedID();
	}
	else
	{
		return SvPb::NoEmbeddedId;
	}
}

bool SVLongResult::onRun( RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	// All inputs and outputs must be validated first
	if( __super::onRun( rRunStatus, pErrorMessages ) )
	{
		const SVObjectClass* pObject = getInput();
		assert( pObject );

		if( nullptr != pObject )
		{
			double FetchValue(0.0);
			pObject->getValue( FetchValue );
			long SetValue = static_cast<long> (FetchValue);
			m_Value.SetValue(SetValue);
			return true;
		}
	}
	rRunStatus.SetInvalid();
	return false;
}

} //namespace SvOp
