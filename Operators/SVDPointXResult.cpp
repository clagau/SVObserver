//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDPointXResult
//* .File Name       : $Workfile:   SVDPointXResult.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:21:06  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVDPointXResult.h"
#include "SVRange.h"
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

SV_IMPLEMENT_CLASS( SVDPointXResultClass, SvPb::DPointXResultClassId);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVDPointXResultClass::SVDPointXResultClass( SVObjectClass* POwner, int StringResourceID )
					:SVResultClass( POwner, StringResourceID )
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVResultObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVResultDPointXObjectType;

	// Identify our input type needs
	m_inputObjectInfo.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDPointValueObjectType);
	m_inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputObjectInfo, _T( "DPointXResult" ) );


	// Register Embedded Objects
	RegisterEmbeddedObject( &x, SvPb::DXEId, IDS_OBJECTNAME_DX, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	x.SetDefaultValue( 0.0, true);
	x.setSaveValueFlag(false);

	// Instantiate Dynamic Objects

	// Construct the SVRangeClass via the ClassInfoStruct
	SvIe::SVClassInfoStruct rangeClassInfo;
	SvDef::SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of the SVRangeClass...
	interfaceInfo.m_ObjectType = SvPb::SVValueObjectType;
	interfaceInfo.m_SubType = SvPb::SVDoubleValueObjectType;
	interfaceInfo.m_EmbeddedID = SvPb::DXEId;
	rangeClassInfo.m_DesiredInputVector.push_back( interfaceInfo );

	// Describe the SVRangeClass ...
	rangeClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVRangeObjectType;
	rangeClassInfo.m_ObjectTypeInfo.m_SubType	= SvPb::SVNotSetSubObjectType;
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

SVDPointXResultClass::~SVDPointXResultClass()
{

}

bool SVDPointXResultClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVResultClass::CreateObject(rCreateStructure);
	bOk &= nullptr != getInputPoint();

	// Set / Reset Printable Flag
	x.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	
	m_isCreated = bOk;

	return bOk;
}

SvVol::SVDPointValueObjectClass* SVDPointXResultClass::getInputPoint()
{
	if( m_inputObjectInfo.IsConnected() )
	{
		return dynamic_cast<SvVol::SVDPointValueObjectClass*> (m_inputObjectInfo.GetInputObjectInfo().getObject());
	}

	return nullptr;
}

bool SVDPointXResultClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	// All inputs and outputs must be validated first
	if( SVResultClass::onRun( rRunStatus, pErrorMessages ) )
	{
		SvVol::SVDPointValueObjectClass* pPoint = getInputPoint();
		assert( pPoint );

		SVPoint<double> dPoint;
		if(nullptr != pPoint)
		{
			pPoint->GetValue(dPoint);
		}
		x.SetValue(dPoint.m_x);

		return true;
	}
	rRunStatus.SetInvalid();
	return false;
}

} //namespace SvOp
