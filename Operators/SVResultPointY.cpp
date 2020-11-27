//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultPointY
//* .File Name       : $Workfile:   SVResultPointY.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:57:28  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVResultPointY.h"
#include "SVRange.h"
#include "SVStatusLibrary/RunStatus.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVValueObjectLibrary\SVPointValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVPointYResult, SvPb::PointYResultClassId);

SVPointYResult::SVPointYResult( SVObjectClass* POwner, int StringResourceID )
					:SVResult( POwner, StringResourceID )
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVResultObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVResultPointYObjectType;

	// Identify our input type needs
	
	m_inputObjectInfo.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVPointValueObjectType );
	m_inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputObjectInfo, _T( "PointYResultValue" ) );
	m_inputObjectInfo.setReportAndCopyFlag(false);


	// Register Embedded Objects
	RegisterEmbeddedObject( &m_Y, SvPb::YEId, IDS_OBJECTNAME_Y, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	m_Y.SetDefaultValue( 0 );
	m_Y.setSaveValueFlag(false);

	// Instantiate Dynamic Objects

	// Construct the SVRange via the ClassInfoStruct
	SvIe::SVClassInfoStruct rangeClassInfo;
	SvDef::SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of the SVRange...
	interfaceInfo.m_ObjectType = SvPb::SVValueObjectType;
	interfaceInfo.m_SubType = SvPb::SVLongValueObjectType;
	interfaceInfo.m_EmbeddedID = SvPb::YEId;
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

SVPointYResult::~SVPointYResult()
{

}

bool SVPointYResult::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVResult::CreateObject(rCreateStructure) && nullptr != SvOl::getInput<SvVol::SVPointValueObjectClass>(m_inputObjectInfo);

	m_Y.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

bool SVPointYResult::onRun( RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	// All inputs and outputs must be validated first
	if( __super::onRun( rRunStatus, pErrorMessages ) )
	{
		const SvVol::SVPointValueObjectClass* pValueObject = SvOl::getInput<SvVol::SVPointValueObjectClass>(m_inputObjectInfo, true);
		assert( pValueObject );

		if( nullptr != pValueObject )
		{
			SVPoint<long> Value;
			pValueObject->GetValue(Value);
			m_Y.SetValue(Value.m_y);
			return true;
		}
	}
	rRunStatus.SetInvalid();
	return false;
}

} //namespace SvOp
