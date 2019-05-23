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
#include "SVObjectLibrary/SVClsIds.h"
#include "SVStatusLibrary/SVRunStatus.h"
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

SV_IMPLEMENT_CLASS( SVPointYResultClass, SVPointYResultClassGuid );

SVPointYResultClass::SVPointYResultClass( SVObjectClass* POwner, int StringResourceID )
					:SVResultClass( POwner, StringResourceID )
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVResultObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVResultPointYObjectType;

	// Identify our input type needs
	
	m_inputObjectInfo.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVPointValueObjectType );
	m_inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputObjectInfo, _T( "PointYResultValue" ) );


	// Register Embedded Objects
	RegisterEmbeddedObject( &m_Y, SVYObjectGuid, IDS_OBJECTNAME_Y, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	m_Y.SetDefaultValue( 0 );
	m_Y.setSaveValueFlag(false);

	// Instantiate Dynamic Objects

	// Construct the SVRangeClass via the ClassInfoStruct
	SvIe::SVClassInfoStruct rangeClassInfo;
	SvDef::SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of the SVRangeClass...
	interfaceInfo.ObjectType = SvPb::SVValueObjectType;
	interfaceInfo.SubType = SvPb::SVLongValueObjectType;
	interfaceInfo.EmbeddedID = SVYObjectGuid;
	rangeClassInfo.m_DesiredInputVector.push_back( interfaceInfo );

	// Describe the SVRangeClass ...
	rangeClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVRangeObjectType;
	rangeClassInfo.m_ObjectTypeInfo.SubType	= SvPb::SVNotSetSubObjectType;
	rangeClassInfo.m_ClassId = SVRangeClassGuid;
	rangeClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVRANGE );

	// Construct it
	SVRangeClass* pRange = (SVRangeClass* )rangeClassInfo.Construct(); 
	if( pRange )
		Add( pRange );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVPointYResultClass::~SVPointYResultClass()
{

}

bool SVPointYResultClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVResultClass::CreateObject(rCreateStructure) && nullptr != SvOl::getInput<SvVol::SVPointValueObjectClass>(m_inputObjectInfo);

	m_Y.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

bool SVPointYResultClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
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
