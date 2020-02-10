//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVObjectLibrary/SVClsIds.h"
#include "SVStatusLibrary/SVRunStatus.h"
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

SV_IMPLEMENT_CLASS( SVDPointYResultClass, SvPb::DPointYResultClassId);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVDPointYResultClass::SVDPointYResultClass( SVObjectClass* POwner, int StringResourceID )
					:SVResultClass( POwner, StringResourceID )
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVResultObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVResultDPointYObjectType;

	// Identify our input type needs
	m_inputObjectInfo.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDPointValueObjectType );
	m_inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputObjectInfo, _T( "DPointYResult" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject( &y, SVDYObjectGuid, IDS_OBJECTNAME_DY, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	y.SetDefaultValue( 0.0, true);
	y.setSaveValueFlag(false);

	// Instantiate Dynamic Objects

	// Construct the SVRangeClass via the ClassInfoStruct
	SvIe::SVClassInfoStruct rangeClassInfo;
	SvDef::SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of the SVRangeClass...
	interfaceInfo.ObjectType = SvPb::SVValueObjectType;
	interfaceInfo.SubType = SvPb::SVDoubleValueObjectType;
	interfaceInfo.EmbeddedID = SVDYObjectGuid;
	rangeClassInfo.m_DesiredInputVector.push_back( interfaceInfo );

	// Describe the SVRangeClass ...
	rangeClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVRangeObjectType;
	rangeClassInfo.m_ObjectTypeInfo.SubType	= SvPb::SVNotSetSubObjectType;
	rangeClassInfo.m_ClassId = SvPb::RangeClassId;
	rangeClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVRANGE );

	// Construct it
	SVRangeClass* pRange = dynamic_cast<SVRangeClass*>(rangeClassInfo.Construct()); 
	if (pRange)
	{
		Add(pRange);
	}

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVDPointYResultClass::~SVDPointYResultClass()
{

}

bool SVDPointYResultClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVResultClass::CreateObject(rCreateStructure);
	bOk &= nullptr != getInputPoint();

	// Set / Reset Printable Flag
	y.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

SvVol::SVDPointValueObjectClass* SVDPointYResultClass::getInputPoint()
{
	if( m_inputObjectInfo.IsConnected() )
	{
		return dynamic_cast<SvVol::SVDPointValueObjectClass*> (m_inputObjectInfo.GetInputObjectInfo().getObject());
	}
	
	return nullptr;
}

bool SVDPointYResultClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	// All inputs and outputs must be validated first
	if( __super::onRun( rRunStatus, pErrorMessages ) )
	{
		SvVol::SVDPointValueObjectClass* pPoint = getInputPoint();
		assert( pPoint );

		SVPoint<double> dPoint;
		if (nullptr != pPoint)
		{
			pPoint->GetValue(dPoint);
		}
		y.SetValue(dPoint.m_y);
	
		return true;
	}
	rRunStatus.SetInvalid();
	return false;
}

} //namespace SvOp
