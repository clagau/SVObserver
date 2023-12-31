//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDPointYResult
//* .File Name       : $Workfile:   SVDPointYResult.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:21:22  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVDPointYResult.h"
#include "SVRange.h"
#include "InspectionEngine/RunStatus.h"
#include "SVValueObjectLibrary/SVDPointValueObjectClass.h"
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

SV_IMPLEMENT_CLASS( SVDPointYResult, SvPb::DPointYResultClassId);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVDPointYResult::SVDPointYResult( SVObjectClass* POwner, int StringResourceID )
					:SVResult( POwner, StringResourceID )
{
	// Identify yourself
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVResultObjectType;
	m_ObjectTypeInfo.m_SubType = SvPb::SVResultDPointYObjectType;

	// Identify our input type needs
	m_inputObject.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDPointValueObjectType );
	registerInputObject( &m_inputObject, _T( "DPointYResult" ), SvPb::ResultInputEId);
	m_inputObject.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_Y, SvPb::DYEId, IDS_OBJECTNAME_DY, false, SvOi::SVResetItemNone, false );

	// Set Embedded defaults
	m_Y.SetDefaultValue( 0.0, true);
	m_Y.setSaveValueFlag(false);

	// Instantiate Dynamic Objects

	// Construct the SVRange via the ClassInfoStruct
	SvIe::SVClassInfoStruct rangeClassInfo;
	SvDef::SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of the SVRange...
	interfaceInfo.m_ObjectType = SvPb::SVValueObjectType;
	interfaceInfo.m_SubType = SvPb::SVDoubleValueObjectType;
	interfaceInfo.m_EmbeddedID = SvPb::DYEId;
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

SVDPointYResult::~SVDPointYResult()
{

}

bool SVDPointYResult::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVResult::CreateObject(rCreateStructure);
	bOk &= nullptr != getInputPoint();

	// Set / Reset Printable Flag
	m_Y.SetObjectAttributesAllowed(SvPb::ObjectAttributes::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_Y.SetObjectAttributesAllowed(SvPb::ObjectAttributes::shortMode, SvOi::SetAttributeType::AddAttribute);

	m_isCreated = bOk;

	return bOk;
}

SvVol::SVDPointValueObjectClass* SVDPointYResult::getInputPoint()
{
	return m_inputObject.getInput<SvVol::SVDPointValueObjectClass>();
}

bool SVDPointYResult::onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	// All inputs and outputs must be validated first
	if( __super::onRun( rRunStatus, pErrorMessages ) )
	{
		SvVol::SVDPointValueObjectClass* pPoint = getInputPoint();
		Log_Assert( pPoint );

		SVPoint<double> dPoint;
		if (nullptr != pPoint)
		{
			pPoint->GetValue(dPoint);
		}
		m_Y.SetValue(dPoint.m_y);
	
		return true;
	}
	rRunStatus.SetInvalid();
	return false;
}

} //namespace SvOp
