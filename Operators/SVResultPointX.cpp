//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultPointX
//* .File Name       : $Workfile:   SVResultPointX.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:57:04  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVResultPointX.h"
#include "SVRange.h"
#include "SVStatusLibrary/RunStatus.h"
#include "SVValueObjectLibrary\SVPointValueObjectClass.h"
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

SV_IMPLEMENT_CLASS( SVPointXResult, SvPb::PointXResultClassId);

SVPointXResult::SVPointXResult( SVObjectClass* POwner, int StringResourceID )
					:SVResult( POwner, StringResourceID )
{
	// Identify yourself
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVResultObjectType;
	m_ObjectTypeInfo.m_SubType = SvPb::SVResultPointXObjectType;

	// Identify our input type needs
	m_inputObject.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVPointValueObjectType );
	registerInputObject( &m_inputObject, _T( "PointXResultValue" ), SvPb::ResultInputEId);
	m_inputObject.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;


	// Register Embedded Objects
	RegisterEmbeddedObject( &m_X, SvPb::XEId, IDS_OBJECTNAME_X, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	m_X.SetDefaultValue( 0 );
	m_X.setSaveValueFlag(false);

	// Instantiate Dynamic Objects

	// Construct the SVRange via the ClassInfoStruct
	SvIe::SVClassInfoStruct rangeClassInfo;
	SvDef::SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of the SVRange...
	interfaceInfo.m_ObjectType = SvPb::SVValueObjectType;
	interfaceInfo.m_SubType = SvPb::SVLongValueObjectType;
	interfaceInfo.m_EmbeddedID = SvPb::XEId;
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
}

SVPointXResult::~SVPointXResult()
{

}

bool SVPointXResult::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	m_isCreated = SVResult::CreateObject(rCreateStructure) && nullptr != m_inputObject.getInput<SvVol::SVPointValueObjectClass>();
	
	m_X.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );

	return m_isCreated;
}

bool SVPointXResult::onRun( RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	// All inputs and outputs must be validated first
	if( __super::onRun( rRunStatus, pErrorMessages ) )
	{
		const SvVol::SVPointValueObjectClass* pValueObject = m_inputObject.getInput<SvVol::SVPointValueObjectClass>(true);
		assert( pValueObject );

		if( nullptr != pValueObject )
		{
			SVPoint<long> Value;
			pValueObject->GetValue(Value);
			m_X.SetValue(Value.m_x);
			return true;
		}
	}
	rRunStatus.SetInvalid();
	return false;
}

} //namespace SvOp
